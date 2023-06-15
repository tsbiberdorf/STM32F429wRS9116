/*************************************************************************
 * 
 * Copyright (c) 2019 Redpine Signals Incorporated. All Rights Reserved.
 * 
 * NOTICE:  All  information  contained  herein is, and  remains  the  property of 
 * Redpine Signals Incorporated. The intellectual and technical concepts contained
 * herein  are  proprietary to  Redpine Signals Incorporated and may be covered by 
 * U.S. and Foreign Patents, patents in process, and are protected by trade secret
 * or copyright law. Dissemination  of this  information or reproduction  of  this
 * material is strictly forbidden unless prior written permission is obtained from
 * Redpine Signals Incorporated.
 */


/**
 * Include files
 */
#include "rsi_driver.h"
#define RSI_SAFE_UPGRADE_ADDR    0x1d408
#define RSI_SECURE_ZONE_ADDR     0x1d418
#define RSI_SAFE_UPGRADE         BIT(12)
#define RSI_SECURE_ZONE          BIT(20)
#define PING_BUFF                0
#define PONG_BUFF                1
#ifndef RSI_M4_INTERFACE
#ifndef RSI_UART_INTERFACE



/**===========================================================================
 * @fn          int16_t rsi_secure_ping_pong_wr( uint32 ping_pong, uint16* src_addr, uint16 size_param)
 * @brief       Writes the given data to the specified register address in the WiFi Module
 * @param[in]   uint32         ping_pong:    ping or pong buffer write
 * @param[in]   uint16         size_param:          Number of bytes to read. (def: 2 since we have 16 bit regs)
 * @param[out]  uint8*         src_addr:        pointer contain the buffer of content to be written
 * @return      int16_t             Indicates the success/failure of operation
 *              < 0  = Failure 
 *              > 0  = Number of bytes written
 *
 * @section Description
 * Writes the given data in ping or pong buffer
 *
 */

#if RSI_FAST_FW_UP
int16_t rsi_secure_ping_pong_wr(uint32_t ping_pong, uint8_t *src_addr, uint16_t size_param)
{
  int16_t      retval;
  rsi_frame_desc_t frame_desc;

  *(uint16_t *)frame_desc.frame_len_queue_no = size_param;

  //! Write out the Command Frame Descriptor
  retval = rsi_spi_frame_dsc_wr(&frame_desc);

  if (retval != 0x00)
  {
#ifdef RSI_DEBUG_PRINT
    RSI_DPRINT(RSI_PL4,"Descriptor write failErr=%02x", (uint16)retval);
#endif
    return retval;
  }

  //! Write the Command Frame Data
  if(size_param)
  {
    size_param= (size_param+3)&~3;
    retval = rsi_spi_frame_data_wr(size_param, src_addr, 0, NULL);
    if (retval != 0x00)
    {
#ifdef RSI_DEBUG_PRINT
      RSI_DPRINT(RSI_PL4,"FramePayload writeErr=%02x", (uint16)retval);
#endif
      return retval;
    }
  }
  return retval;
}
#endif

/*===========================================================*/
/**
 * @fn          int16 rsi_bl_module_power_cycle(void)
 * @brief       MODULE POWER ON, Power cycles the module
 * @param[in]   none
 * @param[out]  none
 * @return      errCode
 *              -1 = Error
 *              0  = OK
 * @description This API is used to power cycle the module.
 * This API is valid only if there is a power gate, external to the module,
 * which is controlling the power to the module using a GPIO signal of the MCU.
 */
int16_t rsi_bl_module_power_cycle(void)
{

  //! configure Reset pin in GPIO mode
  rsi_hal_config_gpio(RSI_HAL_RESET_PIN,RSI_HAL_GPIO_OUTPUT_MODE,RSI_HAL_GPIO_LOW);

  //! reset/drive low value on the GPIO
  rsi_hal_clear_gpio(RSI_HAL_RESET_PIN);


  //! delay for 100 milli seconds
  rsi_delay_ms(100);

  //! Set/drive high value on the GPIO
  rsi_hal_set_gpio(RSI_HAL_RESET_PIN);
  
	return RSI_SUCCESS;
}


/*==============================================*/
/**
 * @fn          int16_t rsi_waitfor_boardready(void)
 * @brief       Waits to receive board ready from WiFi module
 * @param[in]   none
 * @param[out]  none
 * @return      errCode
 *              0  = SUCCESS
 *              < 0 = Failure 
 *              -3 = Board ready not received
 *              -4 = Bootup options last configuration not saved
 *              -5 = Bootup options checksum failed
 *              -6 = Bootloader version mismatch 
 * @section description 
 * This API is used to check board ready from WiFi module.
 */

int16_t rsi_bl_waitfor_boardready(void)
{
  int16_t retval = 0;
  uint16_t read_value = 0;

  retval = rsi_bootloader_instructions(RSI_REG_READ, &read_value);

  if(retval < 0)
  {
    return retval;
  }
  if ((read_value & 0xFF00) == (RSI_HOST_INTERACT_REG_VALID & 0xFF00))
  {
    if((read_value & 0xFF) == RSI_BOOTUP_OPTIONS_LAST_CONFIG_NOT_SAVED)
    {
			return RSI_ERROR_BOOTUP_OPTIONS_NOT_SAVED;
    }
    else if((read_value & 0xFF) == RSI_BOOTUP_OPTIONS_CHECKSUM_FAIL)
    {
			return RSI_ERROR_BOOTUP_OPTIONS_CHECKSUM_FAIL;
    }
    else if((read_value & 0xFF) == RSI_BOOTLOADER_VERSION_1P0)
    {
       rsi_driver_cb_non_rom->rom_version_info = RSI_ROM_VERSION_1P0;
    }
    else if((read_value & 0xFF) == RSI_BOOTLOADER_VERSION_1P1)
    {
    	rsi_driver_cb_non_rom->rom_version_info = RSI_ROM_VERSION_1P1;
    }
#if RSI_BOOTLOADER_VERSION_CHECK
    else if((read_value & 0xFF) == RSI_BOOTLOADER_VERSION)
    {
    }
    else
    {

			return RSI_ERROR_BOOTLOADER_VERSION_NOT_MATCHING;
    }
#endif

		return RSI_SUCCESS;
  }

	return RSI_ERROR_WAITING_FOR_BOARD_READY;
}

/*==============================================*/
/**
 * @fn          int16_t rsi_bl_select_option(uint8_t cmd)
 * @brief       Sends cmd to select option to load or update configuration 
 * @param[in]   uint8_t cmd, type of configuration to be saved
 * @param[out]  none
 * @return      errCode
 < 0 = Command issue failed
 *              0  = SUCCESS
 * @section description 
 * This API is used to send firmware load request to WiFi module or update default configurations.
 */
int16_t rsi_bl_select_option(uint8_t cmd)
{
  uint16_t   boot_cmd = 0;
  int16_t    retval = 0;
  uint16_t   read_value = 0;
  uint32_t timer_instance;

  retval = rsi_mem_wr(RSI_HOST_INTF_REG_OUT,2, (uint8_t *)&boot_cmd);
  if(retval < 0)
  {
    return retval;
  }

  if(cmd == BURN_NWP_FW)
  {
    boot_cmd = RSI_HOST_INTERACT_REG_VALID_FW | cmd;
  }
  else
  {
    boot_cmd = RSI_HOST_INTERACT_REG_VALID | cmd;
  }
  retval   = rsi_bootloader_instructions(RSI_REG_WRITE, &boot_cmd);
  if(retval < 0)
  {
    return retval;
  }

  rsi_init_timer(&timer_instance, 300);

  while((cmd != LOAD_NWP_FW) && (cmd != LOAD_DEFAULT_NWP_FW_ACTIVE_LOW))
  {
    retval = rsi_bootloader_instructions(RSI_REG_READ, &read_value);
    if(retval < 0)
    {
      return retval;
    }
    if(cmd == BURN_NWP_FW)
    {
    	if (read_value == (RSI_HOST_INTERACT_REG_VALID |RSI_SEND_RPS_FILE ))
    	    {
    	      break;
    	    }
    }

    else if (read_value == (RSI_HOST_INTERACT_REG_VALID | cmd))
    {
      break;
    }
    if(rsi_timer_expired(&timer_instance))
    {
			return RSI_ERROR_FW_LOAD_OR_UPGRADE_TIMEOUT;
    }
	}
	if((cmd == LOAD_NWP_FW) || (cmd == LOAD_DEFAULT_NWP_FW_ACTIVE_LOW))
  {
    rsi_init_timer(&timer_instance, 3000);
    do{
      retval = rsi_bootloader_instructions(RSI_REG_READ, &read_value);
      if(retval < 0)
      {
        return retval;
      }
      if ((read_value & 0xF000) == (RSI_HOST_INTERACT_REG_VALID_FW & 0xF000))
      {
        if((read_value & 0xFF) == VALID_FIRMWARE_NOT_PRESENT)
        {
#ifdef RSI_DEBUG_PRINT
          RSI_DPRINT("VALID_FIRMWARE_NOT_PRESENT\n");
#endif
          return RSI_ERROR_VALID_FIRMWARE_NOT_PRESENT;
        }
        if((read_value & 0xFF) == RSI_INVALID_OPTION)
        {
#ifdef RSI_DEBUG_PRINT
          RSI_DPRINT("INVALID CMD\n");
#endif

          return RSI_ERROR_INVALID_OPTION;
        }
        if((read_value & 0xFF) == RSI_CHECKSUM_SUCCESS )
        {
#ifdef RSI_DEBUG_PRINT
          RSI_DPRINT("LOAD SUCCESS\n");
#endif
          break;
        }
      }
      if(rsi_timer_expired(&timer_instance))
      {
        return RSI_ERROR_FW_LOAD_OR_UPGRADE_TIMEOUT;
      }

    }while(1);
  }
  return retval;
}

/*==============================================*/
/**
 * @fn          int16_t rsi_bl_upgrade_firmware(uint8_t *firmware_image , uint32_t fw_image_size, uint8_t flags)
 * @brief       Upgrades firmware to WiFi module
 * @param[in]    uint8_t flags 
 * @param[in]   uint8_t *firmware_image, pointer to firmware
 * @param[in]   uint32_t fw_image_size, size of firmware image
 * @param[out]  none
 * @return      errCode
 *              <0 = Command issue failed
 *              0  = SUCCESS
 * @section description 
 * This API is used to upgrade firmware to WiFi module.
 */
int16_t rsi_bl_upgrade_firmware(uint8_t *firmware_image , uint32_t fw_image_size, uint8_t flags)
{
  static uint16_t boot_cmd;
  uint16_t read_value = 0;
  uint32_t offset = 0;
  int16_t  retval = 0;
  uint32_t boot_insn = 0,poll_resp = 0;
  uint32_t timer_instance;

  //! If it is a start of file set the boot cmd to pong valid
  if(flags & RSI_FW_START_OF_FILE)
  {
    boot_cmd = RSI_HOST_INTERACT_REG_VALID | RSI_PONG_VALID;
  }

  //! check for invalid packet
  if((fw_image_size%(RSI_MIN_CHUNK_SIZE) != 0) && (!(flags & RSI_FW_END_OF_FILE)))
  {
		return RSI_ERROR_INVALID_PACKET;
  }

  //! loop to execute multiple of 4K chunks
  while(offset < fw_image_size)
  {
    switch (boot_cmd)
    {
      case (RSI_HOST_INTERACT_REG_VALID | RSI_PING_VALID):
        boot_insn = RSI_PONG_WRITE;
        poll_resp = RSI_PING_AVAIL;
        boot_cmd = RSI_HOST_INTERACT_REG_VALID |RSI_PONG_VALID;
        break;

      case (RSI_HOST_INTERACT_REG_VALID | RSI_PONG_VALID):
        boot_insn = RSI_PING_WRITE;
        poll_resp = RSI_PONG_AVAIL;
        boot_cmd = RSI_HOST_INTERACT_REG_VALID | RSI_PING_VALID;
        break;
    }

    retval = rsi_bootloader_instructions(boot_insn, (uint16_t *)((uint8_t *)firmware_image + offset));
    if(retval < 0)
    {
      return retval;
    }

    rsi_init_timer(&timer_instance, 1000);

    while(1)
    {
      retval = rsi_bootloader_instructions(RSI_REG_READ, &read_value);
      if(retval < 0)
      {
        return retval;
      }
      if (read_value == (RSI_HOST_INTERACT_REG_VALID | poll_resp))
      {
        break;
      }
      if(rsi_timer_expired(&timer_instance))
       {
				return RSI_ERROR_FW_UPGRADE_TIMEOUT;
       }
    }
    offset += RSI_MIN_CHUNK_SIZE;
  }

  //! For last chunk set boot cmd as End of file reached
  if(flags & RSI_FW_END_OF_FILE)
  {
    boot_cmd = RSI_HOST_INTERACT_REG_VALID | RSI_EOF_REACHED;

    retval = rsi_bootloader_instructions(RSI_REG_WRITE, &boot_cmd);
    if(retval < 0)
    {
      return retval;
    }
    rsi_init_timer(&timer_instance, 40000);
    //! check for successful firmware upgrade
    do 
    {
      retval = rsi_bootloader_instructions(RSI_REG_READ, &read_value);
      if(retval < 0)
      {
        return retval;
      }
      if(rsi_timer_expired(&timer_instance))
      {
				return RSI_ERROR_FW_UPGRADE_TIMEOUT;
      }

    }while(read_value != (RSI_HOST_INTERACT_REG_VALID | RSI_FWUP_SUCCESSFUL));
  }
  return retval;
}

int32_t rsi_set_fast_fw_up(void)
{
  uint32_t  read_data = 0;
  int32_t   retval = 0;
  retval = rsi_mem_rd(RSI_SAFE_UPGRADE_ADDR,4,(uint8_t *)&read_data);
	if(retval < 0)
	{
		return retval;
	}
  //!disabling safe upgradation bit
  if((read_data & RSI_SAFE_UPGRADE))
  {
    read_data &= ~(RSI_SAFE_UPGRADE) ;
    retval = rsi_mem_wr(RSI_SAFE_UPGRADE_ADDR,4,(uint8_t *)&read_data);
    if(retval < 0)
    {
      return retval;
    }
  }
	return retval;
}

/*==============================================*/
/**
 * @fn          int32_t rsi_get_rom_version(void)
 * @brief       API to get the rom version
 * @param[in]   none
 * @param[out]  none
 * @return      rom version
 *              RSI_ROM_VERSION_1P0 - 1
 *              RSI_ROM_VERSION_1P1 - 2
 * @section description
 * This API is used to get the rom version of the module.
 */
int32_t rsi_get_rom_version(void)
{
	return rsi_driver_cb_non_rom->rom_version_info;
}

/*==============================================*/
/**
 * @fn          int16_t rsi_bootloader_instructions(uint8_t type, uint16_t *data)
 * @brief       Sends boot instructions to WiFi module
 * @param[in]   uint8_t type, type of the insruction to perform
 * @param[in]   uint32_t *data, pointer to data which is to be read/write
 * @param[out]  none
 * @return      errCode
 *              < 0  = Command issued failure/Invalid command 
 *                0  = SUCCESS
 *              > 0  = Read value
 * @section description 
 * This API is used to send boot instructions to WiFi module.
 */

int16_t rsi_bootloader_instructions(uint8_t type, uint16_t *data)
{
  int16_t   retval = 0;
  uint32_t  cmd = 0;
#if !RSI_FAST_FW_UP
  uint32_t  j = 0;
#endif
  uint16_t  local = 0;
  uint16_t  read_data = 0;
  uint32_t timer_instance;

  switch(type)
  {
    case RSI_REG_READ:
      retval = rsi_mem_rd(RSI_HOST_INTF_REG_OUT,2,(uint8_t *)&read_data);
      *data = read_data;
      break;

    case RSI_REG_WRITE:
      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN,2, (uint8_t *)data);
      break;
    case RSI_PING_WRITE:
#if RSI_FAST_FW_UP
		  retval = rsi_mem_wr(0x18000 , 4096, (uint8_t *)((uint32_t)data ));
#else
    for (j = 0; j<2048; j++)
      {
	    //! If M4 is writting into TA memory
		//! If Host is writting into TA memory
#ifdef RSI_M4_INTERFACE
		  retval = rsi_mem_wr(0x418000 + (j*2), 2, (uint8_t *)((uint32_t)data + (j*2)));
#else
		  retval = rsi_mem_wr(0x18000 + (j*2), 2, (uint8_t *)((uint32_t)data + (j*2)));
#endif
        if(retval < 0)
        {
          return retval;
        }
      }
#endif
      local = (RSI_PING_AVAIL | RSI_HOST_INTERACT_REG_VALID); 
      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&local);
      break;
    case RSI_PONG_WRITE:
#if RSI_FAST_FW_UP
        retval = rsi_mem_wr(0x19000 , 4096 ,(uint8_t *)((uint32_t)data));
#else
      for (j = 0; j<2048; j++){
	    //! If M4 is writting into TA memory
		//! If Host is writting into TA memory
#ifdef RSI_M4_INTERFACE
        retval = rsi_mem_wr(0x419000 + (j*2), 2 ,(uint8_t *)((uint32_t)data + (j*2)));
#else
        retval = rsi_mem_wr(0x19000 + (j*2), 2 ,(uint8_t *)((uint32_t)data + (j*2)));
#endif
        if(retval < 0)
        {
          return retval;
        }
      }
#endif
      //! Perform the write operation
      local = (RSI_PONG_AVAIL | RSI_HOST_INTERACT_REG_VALID); 
      
      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&local);
      break;
    case BURN_NWP_FW:
      
      cmd = BURN_NWP_FW | RSI_HOST_INTERACT_REG_VALID;
      
      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&cmd);
      if(retval < 0)
      {
        return retval;
      }

      rsi_init_timer(&timer_instance, 300);

      do
      {
        retval = rsi_mem_rd(RSI_HOST_INTF_REG_OUT, 2, (uint8_t *)&read_data);
        if(retval < 0)
        {
          return retval;
        }
        if(rsi_timer_expired(&timer_instance))
        {
	  return RSI_ERROR_FW_LOAD_OR_UPGRADE_TIMEOUT;
        }
      }while(read_data != (RSI_SEND_RPS_FILE | RSI_HOST_INTERACT_REG_VALID));
      break;
    case LOAD_NWP_FW:
      cmd = LOAD_NWP_FW | RSI_HOST_INTERACT_REG_VALID;
      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&cmd);
      break;
    case LOAD_DEFAULT_NWP_FW_ACTIVE_LOW:
      cmd = LOAD_DEFAULT_NWP_FW_ACTIVE_LOW | RSI_HOST_INTERACT_REG_VALID;
      retval = rsi_mem_wr(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&cmd);
      break;
    default:
		retval = RSI_ERROR_INVALID_PARAM ;
      break;
  }
  return retval;
}

#ifndef LINUX_PLATFORM
#if ((defined RSI_SPI_INTERFACE || defined RSI_SDIO_INTERFACE))
/*==============================================*/
/**
 * @fn          int32_t rsi_get_ram_dump(uint32_t addr, uint16_t length, uint8_t *buf)
 * @brief       This API is used to get ram dump 
 * @param[in]   addr   address of memory location in RS9116 
 * @param[in]   length length of the content to read
 * @param[out]  buf    buffer to keep the read content
 * @return      errCode
 *                0  = SUCCESS
 *              > 0  = Read value
 * @section description 
 * This API is used to get ram dump.
 * Note: caller should ensure the buf length based on length parameter
 */

int32_t rsi_get_ram_dump(uint32_t addr, uint16_t length, uint8_t *buf)
{
  uint32_t retval = 0;
  if(buf == NULL)
  {
    return -1;
  }
  if(!length || length > 4096)
  {
    return -2;
  }
  retval = rsi_mem_rd(addr,length,buf);
  return retval;
}
#endif
#endif
#endif
#endif

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
 * Includes
 */
#include "rsi_driver.h"
#ifdef SIM_9118
#include "rsi_board.h"
#endif
#ifdef RSI_M4_INTERFACE
#ifdef ROM_WIRELESS
#include "rsi_chip.h"
#endif
#include "core_cm4.h"
#include "rsi_board.h"
#endif
#ifdef LINUX_PLATFORM
#include<stdint.h>
#include <sys/ioctl.h>
#endif
#include "rsi_hal.h"
#include "rsi_wlan_non_rom.h"


extern rsi_socket_info_non_rom_t *rsi_socket_pool_non_rom;
/*==============================================*/
/**
 * @fn          int32_t rsi_device_init(uint8_t select_option)
 * @brief       This API is used to set the bootup options of the device
 * @param[in]   select_option,type of query to the Bootloader 
 * @param[out]  None
 * @return    
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This API is used to set the bootup options of the device 
 */

int32_t rsi_device_init(uint8_t select_option)
{
  int32_t status = RSI_SUCCESS;
#if (defined (RSI_SPI_INTERFACE) || defined (RSI_SDIO_INTERFACE))	
  uint32_t timer_instance;
#endif	
#if defined(RSI_M4_INTERFACE) || defined(LINUX_PLATFORM)
  uint8_t skip_bootload_sequence = 0;
  int32_t retval=0;
#endif
#if (RSI_SPI_INTERFACE || RSI_M4_INTERFACE || RSI_UART_INTERFACE || RSI_SDIO_INTERFACE )
  //! Board Initialization
	rsi_hal_board_init();
#endif

#ifdef RSI_M4_INTERFACE

  SysTick_Config(SystemCoreClock / 1000);
#ifdef RSI_WITH_OS
  //! Set P2P Intr priority
  NVIC_SetPriority(SysTick_IRQn, SYSTICK_INTR_PRI);
#endif
  if(!(P2P_STATUS_REG & TA_is_active))
  {
#ifdef DEBUG_UART
    DEBUGOUT("\n wakeup TA \n");
#endif
    P2P_STATUS_REG |= M4_wakeup_TA;
    skip_bootload_sequence = 1;
  }
#ifdef DEBUG_UART
  DEBUGOUT("\n Waiting for TA wakeup \n");
#endif
  while(!(P2P_STATUS_REG & TA_is_active));
#ifdef DEBUG_UART
  DEBUGOUT("\n TA wokeup \n");
#endif

  if(!skip_bootload_sequence)
  {
    do
    {
      retval = rsi_waitfor_boardready();
      if(retval == RSI_ERROR_IN_OS_OPERATION )
      {
#ifdef DEBUG_UART
      DEBUGOUT("\n Waiting for TA wakeup \n");
#endif
      }
      if((retval < 0) && (retval != RSI_ERROR_WAITING_FOR_BOARD_READY) && (retval != RSI_ERROR_IN_OS_OPERATION))
      {
        return retval;
      }
    }while((retval == RSI_ERROR_WAITING_FOR_BOARD_READY) && (retval == RSI_ERROR_IN_OS_OPERATION));
    retval = rsi_select_option(select_option);
    if(retval < 0)
    {
      return retval;
    }
}
    //! Updating the TX & RX DMA descriptor address
    rsi_update_tx_dma_desc(skip_bootload_sequence);
    rsi_update_rx_dma_desc();
#endif
  
#ifdef LINUX_PLATFORM  
#if (defined (RSI_USB_INTERFACE) || defined (RSI_SDIO_INTERFACE))
  if(!skip_bootload_sequence)
  {
  do
  {
    retval = rsi_waitfor_boardready();
      if((retval < 0) && (retval != RSI_ERROR_WAITING_FOR_BOARD_READY) && (retval != RSI_ERROR_IN_OS_OPERATION))
    {
        return retval;
    }
    }while(retval == RSI_ERROR_WAITING_FOR_BOARD_READY );
  retval = rsi_select_option(select_option);
  if(retval < 0)
  {
      return retval;
  }
  }
#endif
#else
#ifdef RSI_SDIO_INTERFACE
  //! power cycle the module
  status = rsi_bl_module_power_cycle();
  if(status != RSI_SUCCESS)
  {
    return status;
  }
  //! delay for 100 milli seconds
  rsi_delay_ms(100);
  //! SPI interface initialization
  status = rsi_sdio_iface_init();

  if(status != RSI_SUCCESS)
  {
    return status;
  }
#endif
#ifdef RSI_SPI_INTERFACE
  //! power cycle the module
  status = rsi_bl_module_power_cycle();
  if(status != RSI_SUCCESS)
  {
    return status;
  }
  rsi_delay_ms(100);
  //! SPI interface initialization
  status = rsi_spi_iface_init();

  if(status != RSI_SUCCESS)
  {
    return status;
  }
#endif
#if (defined (RSI_SPI_INTERFACE) || defined (RSI_SDIO_INTERFACE))
  rsi_init_timer(&timer_instance, RSI_BOARD_READY_WAIT_TIME);   //!40000
  do {
    status = rsi_bl_waitfor_boardready();

    if((status < 0) && (status != RSI_ERROR_WAITING_FOR_BOARD_READY))
    {
      return status;
    }

    if(rsi_timer_expired(&timer_instance))
    {
      return RSI_ERROR_BOARD_READY_TIMEOUT;
    }

  }while(status == RSI_ERROR_WAITING_FOR_BOARD_READY);
#endif
#ifdef RSI_SPI_INTERFACE
  if(select_option == LOAD_NWP_FW )
   {
     rsi_set_intr_type(RSI_ACTIVE_HIGH_INTR);
   }
   else if(select_option == LOAD_DEFAULT_NWP_FW_ACTIVE_LOW)
   {
     
     status = rsi_set_intr_type(RSI_ACTIVE_LOW_INTR);
      if(status < 0)
      {
        return status;
      }
   }
#endif
#if (defined (RSI_SPI_INTERFACE) || defined (RSI_SDIO_INTERFACE))
#if RSI_FAST_FW_UP
  status = rsi_set_fast_fw_up();
    if(status != RSI_SUCCESS)
    {
      return status;
    }
#endif
  status = rsi_bl_select_option(select_option);
  if(status < 0)
  {
    return status;
  }
#endif
#endif

#ifdef RSI_M4_INTERFACE
   rsi_m4_ta_interrupt_init();
	if(!(M4SS_P2P_INTR_SET_REG & RX_BUFFER_VALID))
	{	
   		rsi_submit_rx_pkt();
	}	
#else
#if ((defined RSI_SPI_INTERFACE) && (RSI_SPI_HIGH_SPEED_ENABLE))
  //! enable high speed SPI
  status = rsi_spi_high_speed_enable();
  if(status < 0)
  {
    return status;
  }
  //! Switch host clock to high frequency
	rsi_switch_to_high_clk_freq();
#endif
#ifdef RSI_SDIO_INTERFACE
    //! Switch host clock to high frequency
	rsi_switch_to_high_clk_freq();
#endif
  //! Configure interrupt
  rsi_hal_intr_config(rsi_interrupt_handler);

  //! Unmask interrupts
  rsi_hal_intr_unmask();
#endif
#ifdef RSI_WITH_OS
  rsi_delay_ms(1000);
#endif

  return status;
}



/*==============================================*/
/**
 *
 * @fn         rsi_device *rsi_device_deinit()
 * @brief      De-Initializes the module
 *             Resets the module
 *             Need to control the RESET_PS pin to reset the module
 * @param[in] None
 * @param[out] None
 * @return
 *              0 - If success
 *             -1 - Failure
 *
 * @section description
 * This fucntion de-intializes the module
 *
 */
int32_t rsi_device_deinit(void)
{
#ifdef RSI_WLAN_ENABLE    
  uint8_t i;
#endif
  if(rsi_driver_cb != NULL)
  {
#ifndef RSI_M4_INTERFACE
    //! Mask the interrupt
    rsi_hal_intr_mask();
#else
    mask_ta_interrupt(RX_PKT_TRANSFER_DONE_INTERRUPT|TX_PKT_TRANSFER_DONE_INTERRUPT);
#endif 
  }
#ifdef RSI_SPI_INTERFACE
  //! power cycle the module
  rsi_bl_module_power_cycle();
#endif
  //! delay for 100 milli seconds
  rsi_delay_ms(100);
  if(rsi_driver_cb != NULL)
  {
    rsi_driver_cb->common_cb->state = RSI_COMMON_STATE_NONE;
#ifdef RSI_WLAN_ENABLE       	
    rsi_driver_cb->wlan_cb->state  = RSI_WLAN_STATE_NONE;
#endif	
  }
#ifdef RSI_WLAN_ENABLE       
 
  if(rsi_socket_pool != NULL)
  {
    //! added this loop for socket pool not memset/clear sometime while deinit/reset. For more info.
    for(i = 0; i < NUMBER_OF_SOCKETS; i++)
    {
      //! Memset socket info
      memset(&rsi_socket_pool[i], 0, sizeof(rsi_socket_info_t));
    }
  }
  if(rsi_socket_pool_non_rom != NULL)
  {
    //! added this loop for socket pool not memset/clear sometime while deinit/reset. For more info.
    for(i = 0; i < NUMBER_OF_SOCKETS; i++)
    {
      //! Memset socket info
      memset(&rsi_socket_pool_non_rom[i], 0, sizeof(rsi_socket_info_non_rom_t));
    }
  }

#endif
  return RSI_SUCCESS;

}

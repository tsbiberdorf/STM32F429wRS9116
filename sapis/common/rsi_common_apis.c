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
#ifdef RSI_M4_INTERFACE
#include "rsi_ipmu.h"
#endif
#include "rsi_driver.h"
#include "rsi_timer.h"
#ifdef RSI_BT_ENABLE
#include "rsi_bt.h"
#include "rsi_bt_config.h"
#if (BT_A2DP_SOURCE_SBC_CODEC || UNIFIED_PROTOCOL || COEX_MAX_APP || COEX_TEST_FRAMEWORK)
#include "rsi_bt_sbc_codec.h"
#endif
#endif
#ifdef RSI_BLE_ENABLE
#include "rsi_ble.h"
#endif
#ifdef RSI_ANT_ENABLE
#include "rsi_ant.h"
#endif
#include "rsi_wlan_non_rom.h"
extern rsi_socket_info_non_rom_t *rsi_socket_pool_non_rom;
extern rsi_socket_select_info_t *rsi_socket_select_info;
#ifdef PROCESS_SCAN_RESULTS_AT_HOST
extern struct wpa_scan_results_arr *scan_results_array;
#endif
/*
 * Global Variables
 * */
rsi_driver_cb_t *rsi_driver_cb = NULL;
#ifdef RSI_M4_INTERFACE 
extern efuse_ipmu_t global_ipmu_calib_data;
#endif
extern global_cb_t *global_cb;
extern rom_apis_t  *rom_apis;
extern void rom_init(void);
int32_t rsi_driver_memory_estimate(void);
//uint32_t efuse_size = 0;
/*==============================================*/
/**
 * @fn          int32_t rsi_common_get_status()
 * @brief       Return common block status
 * @param[in]   None, 
 * @param[out]  None
 * @return      Common block status
 *
 *
 * @section description
 * This funcion returns common block status
 *
 *
 */

int32_t rsi_common_get_status(void)
{
  return rsi_driver_cb->common_cb->status;
}

/*==============================================*/
/**
 * @fn          void rsi_common_set_status()
 * @brief       Return common block status
 * @param[in]   status, set status of common control block 
 * @param[out]  None
 * @return      Common block status
 *
 *
 * @section description
 * This funcion sets the common block status
 *
 *
 */

void rsi_common_set_status(int32_t status)
{
  rsi_driver_cb->common_cb->status = status;
}

/*==============================================*/
/**
 *
 * @fn         rsi_driver_cb_t *rsi_driver_init(uint8_t *buffer, uint32_t length)
 * @brief      Initicalizes the driver components
 * @param[in]  buffer, application buffer pointer to initialize driver components 
 * @param[in]  length, length of the memory provided by application
 * @param[out] None
 * @return
 *       <=length - If success, returns the memory used, which is less or equal to buffer length provided
 *        >length - If fails, returns total memory required by driver
 *             -1 - If SPI/UART intialization fails
 *             -2 - If no of sockets exceeded
 *
 * @section description
 * This fucntion intializes driver components
 *
 */
uint8_t     *buffer_addr = NULL;
int32_t rsi_driver_init(uint8_t *buffer, uint32_t length)
{
#if (defined RSI_WLAN_ENABLE	) || (defined RSI_UART_INTERFACE ) | (defined LINUX_PLATFORM )
  int32_t         status = RSI_SUCCESS;
#endif	
	
  uint32_t        actual_length  = 0;

  if(((uint32_t )buffer & 3) != 0)
  {
    //! Making buffer 4 byte aligned 
    length -= (4 - ((uint32_t )buffer & 3));
    buffer = (uint8_t *)(((uint32_t )buffer + 3) & ~3); 
  }

  //! Memset user buffer
  memset(buffer, 0, length);

  actual_length += rsi_driver_memory_estimate();

  //! If length is not sufficient
  if(length < actual_length)
  {
    return actual_length;
  }
  buffer_addr = buffer;

  //! Store length minus any alignment bytes to first 32 bit address in buffer.
  *(uint32_t*)buffer = length;
  buffer += sizeof(uint32_t);
  
  //! Designate memory for driver cb
  rsi_driver_cb = (rsi_driver_cb_t *)buffer;
  buffer += sizeof(rsi_driver_cb_t);
  global_cb = (global_cb_t *)buffer;
  buffer += sizeof(global_cb_t);
  rom_apis = (rom_apis_t *)buffer;
  buffer += sizeof(rom_apis_t);
#ifdef RSI_WLAN_ENABLE
  //! Memory for sockets
  rsi_socket_pool = (rsi_socket_info_t *)buffer;
  buffer += RSI_SOCKET_INFO_POOL_SIZE; 
  rsi_socket_pool_non_rom = (rsi_socket_info_non_rom_t *)buffer;
  buffer += RSI_SOCKET_INFO_POOL_ROM_SIZE; 
  rsi_socket_select_info = (rsi_socket_select_info_t *)buffer;
  buffer += RSI_SOCKET_SELECT_INFO_POOL_SIZE; 
  rsi_wlan_cb_non_rom = (rsi_wlan_cb_non_rom_t *)buffer;
  buffer += RSI_WLAN_CB_NON_ROM_POOL_SIZE; 
#ifdef PROCESS_SCAN_RESULTS_AT_HOST
  scan_results_array = (struct wpa_scan_results_arr *)buffer;
  buffer += sizeof(struct wpa_scan_results_arr);
#endif

  //! Check for max no of sockets 
  if(RSI_NUMBER_OF_SOCKETS > (10 + RSI_NUMBER_OF_LTCP_SOCKETS))
  {
    status = RSI_ERROR_INVALID_PARAM;
    return status;
  }
#endif

//!This will be disabled in M4 A11 condition,rest all it will be enabled.
#if !(defined(RSI_M4_INTERFACE) && ! defined (A11_ROM))
	rsi_driver_cb->event_list = (rsi_event_cb_t *)buffer;
	buffer += RSI_EVENT_INFO_POOL_SIZE;
#endif

  rom_init();

  //! Designate memory for rx_pool
#if !((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM))
  rsi_pkt_pool_init(&rsi_driver_cb->rx_pool, buffer, RSI_DRIVER_RX_POOL_SIZE, RSI_DRIVER_RX_PKT_LEN);
  buffer += RSI_DRIVER_RX_POOL_SIZE;
#endif
  //! Designate memory for common_cb
  rsi_driver_cb_non_rom =(rsi_driver_cb_non_rom_t *)buffer;
  buffer += sizeof(rsi_driver_cb_non_rom_t);

  //! Designate memory for common_cb
  rsi_driver_cb->common_cb =(rsi_common_cb_t *)buffer;
  buffer += sizeof(rsi_common_cb_t);

  //! Initialize common cb
  rsi_common_cb_init(rsi_driver_cb->common_cb);

  //! Designate pool for common block
  rsi_pkt_pool_init(&rsi_driver_cb->common_cb->common_tx_pool, buffer, RSI_COMMON_POOL_SIZE, RSI_COMMON_CMD_LEN);
  buffer += RSI_COMMON_POOL_SIZE;


  //! Designate memory for wlan block
  rsi_driver_cb->wlan_cb =(rsi_wlan_cb_t *)buffer;
  buffer += sizeof(rsi_wlan_cb_t);

#ifdef RSI_M4_INTERFACE
	//! Designate memory for efuse ipmu block
    rsi_driver_cb->common_cb->ipmu_calib_data_cb=(efuse_ipmu_t *)buffer;
    //ipmu_calib_data_cb = (efuse_ipmu_t *)buffer;
		//efuse_size = sizeof(efuse_ipmu_t);
    buffer += sizeof(efuse_ipmu_t);
#endif
#ifdef RSI_WLAN_ENABLE
  //! Initialize wlan cb
  rsi_wlan_cb_init(rsi_driver_cb->wlan_cb);
#endif

  //! Designate memory for wlan_cb pool
  rsi_pkt_pool_init(&rsi_driver_cb->wlan_cb->wlan_tx_pool, buffer, RSI_WLAN_POOL_SIZE, RSI_WLAN_CMD_LEN);
  buffer += RSI_WLAN_POOL_SIZE;

  //! Initialize scheduler
  rsi_scheduler_init(&rsi_driver_cb->scheduler_cb);

  //! Initialize events
  rsi_events_init();

  rsi_queues_init(&rsi_driver_cb->wlan_tx_q);

  rsi_queues_init(&rsi_driver_cb->common_tx_q);
#ifdef RSI_M4_INTERFACE
  rsi_queues_init(&rsi_driver_cb->m4_tx_q);

  rsi_queues_init(&rsi_driver_cb->m4_rx_q);

#endif
#ifdef RSI_ZB_ENABLE
  rsi_driver_cb->zigb_cb =(rsi_zigb_cb_t *)buffer;
  buffer += sizeof(rsi_zigb_cb_t);

  //! Initialize zigb cb
  rsi_zigb_cb_init(rsi_driver_cb->zigb_cb);

  //! Designate memory for zigb_cb buffer pool
  rsi_pkt_pool_init(&rsi_driver_cb->zigb_cb->zigb_tx_pool, buffer, RSI_ZIGB_POOL_SIZE, RSI_ZIGB_CMD_LEN);
  buffer += RSI_ZIGB_POOL_SIZE;

 

#ifdef ZB_MAC_API
  rsi_driver_cb->zigb_cb->zigb_global_mac_cb = (rsi_zigb_global_mac_cb_t*)buffer;
  buffer += sizeof(rsi_zigb_global_mac_cb_t);
  //! Fill in zigb_global_cb
  buffer += rsi_zigb_global_mac_cb_init(buffer);
#else
  rsi_driver_cb->zigb_cb->zigb_global_cb = (rsi_zigb_global_cb_t*)buffer;
  buffer += sizeof(rsi_zigb_global_cb_t);

  //! Fill in zigb_global_cb
  buffer += rsi_zigb_global_cb_init(buffer);
#endif
  rsi_queues_init(&rsi_driver_cb->zigb_tx_q);
#ifdef ZB_DEBUG
  printf("\n ZIGB POOL INIT \n");
#endif
#endif

#if defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE) || defined(RSI_ANT_ENABLE)
  //! Designate memory for bt_common_cb
  rsi_driver_cb->bt_common_cb = (rsi_bt_cb_t *)buffer;
  buffer += ((sizeof(rsi_bt_cb_t) + 3) & ~3);

  //! Initialize bt_common_cb
  rsi_bt_cb_init(rsi_driver_cb->bt_common_cb);

  //! Designate memory for bt_common_cb pool
  rsi_pkt_pool_init(&rsi_driver_cb->bt_common_cb->bt_tx_pool, buffer, RSI_BT_COMMON_POOL_SIZE, RSI_BT_COMMON_CMD_LEN);
  buffer += ((RSI_BT_COMMON_POOL_SIZE + 3) & ~3);


  rsi_queues_init(&rsi_driver_cb->bt_single_tx_q);
#endif

#ifdef RSI_BT_ENABLE
  //! Designate memory for bt_classic_cb
  rsi_driver_cb->bt_classic_cb = (rsi_bt_cb_t *)buffer;
  buffer += ((sizeof(rsi_bt_cb_t) + 3) & ~3);

  //! Initialize bt_classic_cb
  rsi_bt_cb_init(rsi_driver_cb->bt_classic_cb);

  //! Designate memory for bt_classic_cb pool
  rsi_pkt_pool_init(&rsi_driver_cb->bt_classic_cb->bt_tx_pool, buffer, RSI_BT_CLASSIC_POOL_SIZE, RSI_BT_CLASSIC_CMD_LEN);
  buffer += ((RSI_BT_CLASSIC_POOL_SIZE + 3) & ~3);
#endif

#ifdef RSI_BLE_ENABLE
  //! Designate memory for ble_cb
  rsi_driver_cb->ble_cb = (rsi_bt_cb_t *)buffer;
  buffer += ((sizeof(rsi_bt_cb_t) + 3) & ~3);

  //! Initialize ble_cb
  rsi_bt_cb_init(rsi_driver_cb->ble_cb);

  //! Designate memory for ble_cb pool
  rsi_pkt_pool_init(&rsi_driver_cb->ble_cb->bt_tx_pool, buffer, RSI_BLE_POOL_SIZE, RSI_BLE_CMD_LEN);
  buffer += ((RSI_BLE_POOL_SIZE + 3) & ~3);
#endif

#ifdef RSI_ANT_ENABLE
  //! Designate memory for ant_cb
  rsi_driver_cb->ant_cb = (rsi_bt_cb_t *)buffer;
  buffer += ((sizeof(rsi_bt_cb_t) + 3) & ~3);

  //! Initialize ant_cb
  rsi_bt_cb_init(rsi_driver_cb->ant_cb);

  //! Designate memory for ant_cb pool
  rsi_pkt_pool_init(&rsi_driver_cb->ant_cb->bt_tx_pool, buffer, RSI_ANT_POOL_SIZE, RSI_ANT_CMD_LEN);
  buffer += ((RSI_ANT_POOL_SIZE + 3) & ~3);

  rsi_queues_init(&rsi_driver_cb->ant_tx_q);
#endif

#ifdef SAPIS_BT_STACK_ON_HOST
  //! Designate memory for bt_classic_cb
  rsi_driver_cb->bt_ble_stack_cb = (rsi_bt_cb_t *)buffer;
  buffer += sizeof(rsi_bt_cb_t);

  //! Initialize bt_classic_cb
  rsi_bt_cb_init(rsi_driver_cb->bt_ble_stack_cb);

  //! Designate memory for bt_classic_cb pool
  rsi_pkt_pool_init(&rsi_driver_cb->bt_ble_stack_cb->bt_tx_pool, buffer, RSI_BT_CLASSIC_POOL_SIZE, RSI_BT_CLASSIC_CMD_LEN);
  buffer += RSI_BT_CLASSIC_POOL_SIZE;


  rsi_queues_init(&rsi_driver_cb->bt_ble_stack_tx_q);

#endif

#if defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE) || defined(RSI_ANT_ENABLE)
  //! Designate memory for bt_common_cb
  rsi_driver_cb->bt_global_cb = (rsi_bt_global_cb_t*)buffer;
  buffer += sizeof(rsi_bt_global_cb_t);

  //! Fill in bt_global_cb
  buffer += rsi_bt_global_cb_init(rsi_driver_cb, buffer);
#endif

  if(length < (uint32_t)(buffer - buffer_addr))
  {
    return buffer - buffer_addr;
  }


#ifndef LINUX_PLATFORM
#ifdef RSI_SPI_INTERFACE
  rsi_timer_start(RSI_TIMER_NODE_0, RSI_HAL_TIMER_TYPE_PERIODIC, RSI_HAL_TIMER_MODE_MILLI,1,rsi_timer_expiry_interrupt_handler);
#endif 

#if (defined(RSI_SPI_INTERFACE) || defined(RSI_SDIO_INTERFACE))
 //! Configure power save GPIOs
  rsi_powersave_gpio_init();
#endif
#endif

#ifdef LINUX_PLATFORM
#if (defined (RSI_USB_INTERFACE) || defined (RSI_SDIO_INTERFACE))
  status = rsi_linux_app_init();
    if(status != RSI_SUCCESS)
    {
    return status;
    }

#endif
#endif

#ifdef RSI_UART_INTERFACE
  //! UART initialisation
  status = rsi_uart_init();
  if(status != RSI_SUCCESS)
  {
    return status;
  }
#endif

  return actual_length;
}

/*==============================================*/

/**
 * @fn         rsi_wireless_init(uint16_t opermode, uint16_t coex_mode)
 * @brief      Initialize WiSeConnect software
 * @param[in]  opermode , WLAN operating mode
 * @param[in]  coex_mode, co-ex mode 
 * @return     int32_t 
 *             0  = success
 *             <0 = failure 
 *
 * @section description
 * This function is used to initialize WiSeConnect software module.
 * Application should call this function 
 *
 */
int32_t rsi_wireless_init(uint16_t opermode,uint16_t coex_mode)
{
  rsi_pkt_t      *pkt;
  rsi_opermode_t *rsi_opermode;
  int32_t        status = RSI_SUCCESS;
  uint32_t timer_instance;
#ifdef RSI_M4_INTERFACE	
//  int32_t        ipmu_status = RSI_SUCCESS;

#endif
  //! Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  common_cb->ps_coex_mode = coex_mode; 
  common_cb->ps_coex_mode &= ~BIT(0); 
	
#if (defined(RSI_UART_INTERFACE) && defined(RSI_STM32))
  common_cb->state = RSI_COMMON_CARDREADY;	
#else
  rsi_init_timer(&timer_instance,RSI_CARD_READY_WAIT_TIME);
	//! if state is not in card ready received state
	while(common_cb->state == RSI_COMMON_STATE_NONE)
	{
#ifndef RSI_WITH_OS
		//! Wait untill receives card ready
		rsi_scheduler(&rsi_driver_cb->scheduler_cb);
		
		if(rsi_timer_expired(&timer_instance))
		{
			return RSI_ERROR_CARD_READY_TIMEOUT;
		}
#else
    if (rsi_semaphore_wait(&common_cb->common_card_ready_sem, RSI_CARD_READY_WAIT_TIME) != RSI_ERROR_NONE)
    {
      rsi_common_set_status(RSI_ERROR_RESPONSE_TIMEOUT);
      return RSI_ERROR_RESPONSE_TIMEOUT;
    }
#endif
	}
#endif
  if(wlan_cb->auto_config_state != RSI_WLAN_STATE_NONE)
  {
    while(1)
    {
      //! Check auto config state

      if((wlan_cb->auto_config_state == RSI_WLAN_STATE_AUTO_CONFIG_DONE) || (wlan_cb->auto_config_state == RSI_WLAN_STATE_AUTO_CONFIG_FAILED))
      {
        if(wlan_cb->state >= RSI_WLAN_STATE_INIT_DONE) {
          common_cb->ps_coex_mode |= BIT(0);
        }
        wlan_cb->auto_config_state = 0;
        break;
      }
      //! signal the wlan semaphore
      if(rsi_semaphore_wait(&rsi_driver_cb_non_rom->wlan_cmd_sem,RSI_AUTO_JOIN_RESPONSE_WAIT_TIME)!= RSI_ERROR_NONE)
      {
        rsi_common_set_status(RSI_ERROR_RESPONSE_TIMEOUT);
        return RSI_ERROR_RESPONSE_TIMEOUT;
      }
    }
#ifdef RSI_WLAN_ENABLE 
status = rsi_common_get_status() ;    
      if(status)
      {
        //! return error
        return status;
      }
status = rsi_wlan_get_status();
      if(status)
      {
        //! return error
        return status;
      }
#endif
    //! auto configuration status.
    return RSI_USER_STORE_CFG_STATUS;
  }
  else if(common_cb->state != RSI_COMMON_CARDREADY)
  {
    //! Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  
  if(rsi_check_and_update_cmd_state(COMMON_CMD,IN_USE) == RSI_SUCCESS)
  {

  //! allocate command buffer
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);
  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);
    //! return packet allocation failure
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  //! memset the packet data
  memset(&pkt->data, 0, sizeof(rsi_opermode_t));

  //! Take the user provided data and fill it in opermode mode structure
  rsi_opermode = (rsi_opermode_t *)pkt->data;

  //! Save opermode command
  wlan_cb->opermode = opermode;

  //! Fill coex and opermode parameters
  rsi_uint32_to_4bytes(rsi_opermode->opermode, (coex_mode <<16 | opermode));

  //! send opermode command to driver
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_OPERMODE, pkt);
  
  //! wait on common semaphore
  rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_OPERMODE_RESPONSE_WAIT_TIME);

  //!Changing the common state to allow state
  rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

  }

  else
  {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

  //! get common command response stattus
  status = rsi_common_get_status();

#if defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE) || defined(RSI_ANT_ENABLE)
  if(!status)
  {
    if ( (coex_mode == RSI_OPERMODE_WLAN_BLE) ||
        (coex_mode ==  RSI_OPERMODE_WLAN_BT_CLASSIC)
#if defined(RSI_ANT_ENABLE)
        || (coex_mode ==  RSI_OPERMODE_ANT_GARDEN)
#endif
        )
    {
      //WC waiting for BT Classic/ZB/BLE card ready
      rsi_bt_common_init();
    }
  }
#endif
	
#ifdef RSI_M4_INTERFACE	
	//ipmu_status = rsi_cmd_m4_ta_secure_handshake(2,0,NULL,sizeof(efuse_ipmu_t),(uint8_t *)&global_ipmu_calib_data);
	//RSI_IPMU_UpdateIpmuCalibData(&global_ipmu_calib_data);
#endif 
  
	//! Return the status
  return status;
}
/*==============================================*/

#ifdef RSI_CHIP_MFG_EN

#define RSI_WAIT_TIMEOUT   50000000
#define RSI_INC_TIMER timer_count++
int32_t rsi_wait_for_card_ready(uint32_t queue_type)
{
	uint32_t timer_count = 0;
	uint32_t status = RSI_SUCCESS;

	rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
	rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;
	volatile rsi_common_state_t *state_p;

	if(queue_type == RSI_COMMON_Q) {
		state_p = &common_cb->state;
	} else {
		state_p = &wlan_cb->state;
	}
	//! if state is not in card ready received state
	while(*state_p == RSI_COMMON_STATE_NONE)
	{
		if(RSI_INC_TIMER > RSI_WAIT_TIMEOUT) {
			status = RSI_FAILURE;
			break;
		}
#ifndef RSI_WITH_OS
		//! Wait untill receives card ready
		rsi_scheduler(&rsi_driver_cb->scheduler_cb);
#else
		rsi_semaphore_wait(&common_cb->common_card_ready_sem, RSI_WAIT_FOREVER);
#endif
	}
	return status;
}

int32_t rsi_common_dev_params()
{
	int32_t    status = RSI_SUCCESS;
	rsi_pkt_t             *pkt;
	common_dev_config_params_t *rsi_common_dev_params_p;

	//! Get common cb structure pointer
	rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  if(rsi_check_and_update_cmd_state(COMMON_CMD,IN_USE) == RSI_SUCCESS)
  {

	//! allocate command buffer  from common pool
	pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

	//! If allocation of packet fails
	if(pkt == NULL)
	{
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

		//! return packet allocation failure error
		return RSI_ERROR_PKT_ALLOCATION_FAILURE;
	}

	//! Take the user provided data and fill it in debug uart print select structure
	rsi_common_dev_params_p = (common_dev_config_params_t *)pkt->data;

	rsi_common_dev_params_p->lp_sleep_handshake = 0x0;
	rsi_common_dev_params_p->ulp_sleep_handshake = 0x1;
	rsi_common_dev_params_p->sleep_config_param = 0x0;
	rsi_common_dev_params_p->host_wakeup_intr_enable = 0xf;
	rsi_common_dev_params_p->host_wakeup_intr_active_high = 0x4;
	rsi_common_dev_params_p->lp_wakeup_threshold = 0x100;
	rsi_common_dev_params_p->ulp_wakeup_threshold = 0x2030200;
	rsi_common_dev_params_p->wakeup_threshold = 0x0;
	rsi_common_dev_params_p->unused_soc_gpio_bitmap = 0x10000;
	rsi_common_dev_params_p->unused_ulp_gpio_bitmap = 0x13;
	rsi_common_dev_params_p->ext_pa_or_bt_coex_en = 0x0;
	rsi_common_dev_params_p->opermode = 0x0;
	rsi_common_dev_params_p->driver_mode = 0x0;
	rsi_common_dev_params_p->no_of_stations_supported = 0x1;
	rsi_common_dev_params_p->peer_distance = 0x0;
	rsi_common_dev_params_p->bt_feature_bitmap = 0x13;

	//! Send firmware version query request
	status = rsi_driver_common_send_cmd(RSI_COMMON_DEV_CONFIG, pkt);

  //!Changing the common state to allow state
  rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

  }

  else
  {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

	//! Return the status
	return status;
}
#endif
/**
 * @fn         rsi_cmd_uart_flow_ctrl(uint8_t uartflow_en)
 * @brief      uart flow control enabling or disabling
 * @param[in]  uartflow_en ,0 - UART flow control disable
 *                          1 - Use Pin set number 1 for CTS/RTS
 *                          2 - USe Pin set number 2 foe CTS/RTS
 * @return     int32_t 
 *             0  = success
 *             <0 = failure 
 *
 * @section description
 * This function is used to uart flow control enabling or disabling.
 * Application should call this function 
 *
 */
int32_t rsi_cmd_uart_flow_ctrl(uint8_t uartflow_en)
{
  rsi_pkt_t       *pkt;
 
  int32_t          status = RSI_SUCCESS;

  //! Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
  
  if(rsi_check_and_update_cmd_state(COMMON_CMD,IN_USE) == RSI_SUCCESS)
  {
  //! allocate command buffer
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }
  
  pkt->data[0] = uartflow_en;
  //! send  antenna select command
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_UART_FLOW_CTRL_ENABLE, pkt);

  //! wait on common semaphore
  rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_UART_FLOW_CTRL_RESPONSE_WAIT_TIME);

  //!Changing the common state to allow state
  rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

  }

  else
  {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

  //! get common command response stattus
  status = rsi_common_get_status();

  //! Return the status
  return status;

}
/*==============================================*/
/**
 * @fn
 * @brief
 * @param[in]   , 
 * @param[out]
 * @return
 *
 *
 * @section description
 * This 
 *
 *
 */

#ifdef RSI_M4_INTERFACE
int32_t rsi_cmd_m4_ta_secure_handshake(uint8_t sub_cmd_type, uint8_t input_len,uint8_t *input_data, uint8_t output_len, uint8_t *output_data)
{
  rsi_pkt_t       *pkt;
 
  int32_t          status = RSI_SUCCESS;

  //! Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  
  if(rsi_check_and_update_cmd_state(COMMON_CMD,IN_USE) == RSI_SUCCESS)
  {

  //! allocate command buffer
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }
  
  //! Take the sub_cmd_type for TA and M4 commands
  pkt->data[0] = sub_cmd_type;

  pkt->data[1] = input_len;

  memcpy(&pkt->data[2],input_data,input_len);

  //! attach the buffer given by user
  common_cb->app_buffer = output_data;

  //! length of the buffer provided by user
  common_cb->app_buffer_length = output_len;

  //! send  antenna select command
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_TA_M4_COMMANDS, pkt);

   //! wait on common semaphore
  rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_TA_M4_COMMAND_RESPONSE_WAIT_TIME);

  //!Changing the common state to allow state
  rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

  }

  else
  {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

  //! get common command response stattus
  status = rsi_common_get_status();

  //! Return the status
  return status;

}
#endif

 



/*==============================================*/
/**
 * @fn           int32_t rsi_wireless_deinit();
 * @brief        Resets the module 
 * @param[in]    none 
 * @return       int32_t 
 *               0  =  success
 *               <0 = failure 
 *
 * @section description
 * This function resets the module     
 */

int32_t rsi_wireless_deinit(void)
{
  int32_t   status = RSI_SUCCESS;
 #ifndef RSI_M4_INTERFACE
	int32_t length =0; 
	#endif
#ifdef RSI_M4_INTERFACE
  rsi_pkt_t *pkt;
  //! Get common cb structure pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
  
  if(rsi_check_and_update_cmd_state(COMMON_CMD,IN_USE) == RSI_SUCCESS)
  {

  //! allocate command buffer  from wlan pool
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }
  
  //! send softreset command
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_SOFT_RESET, pkt);


  //!Changing the common state to allow state
  rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

  }

  else
  {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }
#ifndef RSI_M4_INTERFACE
  //! Mask interrupts
  rsi_hal_intr_mask();
#endif
#ifdef RSI_SPI_INTERFACE
  //! poll for interrupt status
  while(!rsi_hal_intr_pin_status());

  //! SPI interface initialization
  status = rsi_spi_iface_init();
  if(status != RSI_SUCCESS)
  {
    //! Return the status
    return status;
  }
#endif
#ifndef RSI_M4_INTERFACE
  //! Unmask interrupts
  rsi_hal_intr_unmask();
#endif

#ifdef RSI_UART_INTERFACE
  rsi_uart_deinit();

#endif
  
  //! Wait on common semaphore 
  if(rsi_semaphore_wait(&rsi_driver_cb->common_cb->common_sem, RSI_DEINIT_RESPONSE_WAIT_TIME) != RSI_ERROR_NONE)
    {
      rsi_common_set_status(RSI_ERROR_RESPONSE_TIMEOUT);
      return RSI_ERROR_RESPONSE_TIMEOUT;
    }

  //! get common status
  status = rsi_common_get_status();
#else
#if (!FIRMWARE_UPGRADE && !FIRMWARE_UPGRADE_VIA_BOOTLOADER)

  if( buffer_addr != NULL )
  {
    length = *(uint32_t *)buffer_addr;
    //! Driver initialization
    status = rsi_driver_init(buffer_addr, length);
    if((status < 0) || (status > length))
    {
      return status;
    }
  }
  else
  {
     return -1; 
  }
#endif
  //! state update
  rsi_driver_cb->common_cb->state = RSI_COMMON_STATE_NONE;
#ifdef RSI_WLAN_ENABLE       	
  rsi_driver_cb->wlan_cb->state  = RSI_WLAN_STATE_NONE;
#endif
  //! Redpine module initialization
  status = rsi_device_init(LOAD_NWP_FW);
  if(status != RSI_SUCCESS)
  {
    return status;
  }

#endif
    return status;
}

/*==============================================*/
/**
 * @fn             int32_t rsi_wireless_antenna(uint8_t type)
 * @brief          This function is used to select antenna on the device
 * @param[in]   ,  type : INTERNAL_OR_RF_OUT1 , UFL_OR_RF_OUT2  
 * @param[out]     None 
 * @return       int32_t 
 *               0  =  success
 *               <0 = failure 
 *
 * @section description
 * This Function is used to select antenna type on the device
 *
 */
int32_t rsi_wireless_antenna(uint8_t type,uint8_t gain_2g, uint8_t gain_5g)
{

  rsi_pkt_t       *pkt;
  rsi_antenna_select_t *rsi_antenna;
  int32_t          status = RSI_SUCCESS;
	
//! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  //! pre-condition for antenna selection.
  if((wlan_cb->state < RSI_WLAN_STATE_INIT_DONE))
  {
    //!command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  
  if(rsi_check_and_update_cmd_state(COMMON_CMD,IN_USE) == RSI_SUCCESS)
  {

  //! allocate command buffer
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }
  
  //! Take the user provided data and fill it in antenna select structure
  rsi_antenna = (rsi_antenna_select_t *)pkt->data;

  //! antenna type
  rsi_antenna->antenna_value = type;

  //! antena gain in 2.4GHz
  rsi_antenna->gain_2g       = gain_2g;

  //! antenna gain in 5GHz 
  rsi_antenna->gain_5g       = gain_5g;

  //! send  antenna select command
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ANTENNA_SELECT, pkt);

  //! wait on common semaphore
  rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_ANTENNA_SEL_RESPONSE_WAIT_TIME );

  //!Changing the common state to allow state
  rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

  }

  else
  {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

  //! get common command response stattus
  status = rsi_common_get_status();

  //! Return the status
  return status;

}

/*==============================================*/
/**
 * @fn             int32_t rsi_send_feature_frame()
 * @brief          This function is used to send feature frame to connection cmd parser
 * @return       int32_t 
 *               0  =  success
 *               <0 = failure 
 *
 * @section description
 * This Function is used to send feature frame to connection command parser
 *
 */
int32_t rsi_send_feature_frame(void)
{

  rsi_pkt_t       *pkt;
  rsi_feature_frame_t *rsi_feature_frame;
  int32_t          status = RSI_SUCCESS;

  //! Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  if(rsi_check_and_update_cmd_state(COMMON_CMD,IN_USE) == RSI_SUCCESS)
  {

  //! allocate command buffer
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  //! Take the user provided data and fill it in antenna select structure
  rsi_feature_frame = (rsi_feature_frame_t *)pkt->data;

  //! pll mode value
  rsi_feature_frame->pll_mode = PLL_MODE;

  //! rf type
  rsi_feature_frame->rf_type = RF_TYPE;

  //! wireless mode
  rsi_feature_frame->wireless_mode = WIRELESS_MODE;

  //! enable ppp
  rsi_feature_frame->enable_ppp = ENABLE_PPP;
 
  //! afe type
  rsi_feature_frame->afe_type = AFE_TYPE;

  //! feature enables
  rsi_feature_frame->feature_enables = FEATURE_ENABLES;

  //! send antenna select command
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_FEATURE_FRAME, pkt);

  //! wait on common semaphore
  rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_FEATURE_FRAME_RESPONSE_WAIT_TIME);

  //!Changing the common state to allow state
  rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

  }

  else
  {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

  //! get common command response stattus
  status = rsi_common_get_status();

  //! Return the status
  return status;

}
/*==============================================*/
/**
 * @fn          int32_t rsi_get_fw_version(uint8_t *response, uint16_t length)
 * @brief       Returns enquired data
 *                
 * @param[out]  application buffer to get the command response payload
 * @param[in]   length of the application buffer to copy command response 
 * @return      command response status
 *
 * @section description
 * The functionality of this function depends on the ion
 *
 * This function returns common status
 *
 */
int32_t rsi_get_fw_version(uint8_t *response, uint16_t length)
{
  int32_t    status = RSI_SUCCESS;
  rsi_pkt_t             *pkt;

  //! Get common cb structure pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  //! if state is not in card ready received state
  if(common_cb->state == RSI_COMMON_STATE_NONE)
  {
    while(common_cb->state != RSI_COMMON_CARDREADY)
    { 
#ifndef RSI_WITH_OS
      rsi_scheduler(&rsi_driver_cb->scheduler_cb);
#endif
    }
  }
   
  if(rsi_check_and_update_cmd_state(COMMON_CMD,IN_USE) == RSI_SUCCESS)
  {

  //! allocate command buffer  from common pool
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }
  
  if(response != NULL)
  {
    //! attach the buffer given by user
    common_cb->app_buffer = response;

    //! length of the buffer provided by user 
    common_cb->app_buffer_length = length; 
  }
  else
  {
    //! Assign NULL to the app_buffer to avoid junk
    common_cb->app_buffer = NULL;

    //! length of the buffer to 0 
    common_cb->app_buffer_length = 0; 

  }

  //! Send firmware version query request
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_FW_VERSION, pkt);

   //! wait on common semaphore
  rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_FWUP_RESPONSE_WAIT_TIME);

  //!Changing the common state to allow state
  rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

  }

  else
  {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

  //! get common command response stattus
  status = rsi_common_get_status();

  //! Return the status
  return status;

}

/*==============================================*/
/**
 * @fn          int32_t rsi_common_debug_log(uint32_t assertion_type, uint32_t assertion_level)
 * @brief       enable debug logs.
 *                
 * @param[in]   Assertion_type (Possible values 0 - 15, BIT(0):SME/APME, BIT(1):UMAC, BIT(2):TCP/IP ,BIT(3):Enables assertion indication and provides ram dump in critical assertion.
 * @param[in]   Assertion_level (Possible values 0 - 15). value 1 is least value,15 is the highest level,0 is to disable all the prints.
 * @return      command response status
 *
 * @section description
 * The functionality of this function enable the uart debug prints 
 *
 * 
 */
int32_t rsi_common_debug_log(int32_t assertion_type, int32_t assertion_level)
{
  int32_t    status = RSI_SUCCESS;
  rsi_pkt_t             *pkt;
  rsi_debug_uart_print_t *debug_uart_print;
  //! Get common cb structure pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
	
  if(((assertion_type > 15) ||(assertion_type < 0)) || 
      ((assertion_level < 0) || (assertion_level > 15)))
  { 
   return RSI_ERROR_INVALID_PARAM;
  }
  
  if(rsi_check_and_update_cmd_state(COMMON_CMD,IN_USE) == RSI_SUCCESS)
  {

  //! allocate command buffer  from common pool
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }
  
  //! Take the user provided data and fill it in debug uart print select structure
   debug_uart_print = (rsi_debug_uart_print_t *)pkt->data;

  //! assertion_type
  debug_uart_print->assertion_type = assertion_type;

  //! rf type
  debug_uart_print->assertion_level = assertion_level;
	
  //! Send firmware version query request
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_DEBUG_LOG, pkt);

  //! wait on common semaphore
  rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_DEBUG_LOG_RESPONSE_WAIT_TIME);

  //!Changing the common state to allow state
  rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

  }

  else
  {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

  //! get common command response stattus
  status = rsi_common_get_status();
  
  //! Return the status
  return status;

}

/*==============================================*/
/**
 * @fn             int32_t rsi_switch_proto(uint8_t type)
 * @brief          This Function is used to enable or disable BT 
 * @param[in]   ,  type 
 * @param[out]     None 
 * @return       int32_t 
 *               0  =  success
 *               <0 = failure 
 *
 * @section description
 * This Function is used to enable or disable BT 
 *
 */
int32_t rsi_switch_proto(uint8_t type, void(*callback)(uint16_t mode, uint8_t *bt_disabled_status))
{
  rsi_pkt_t       *pkt;
  rsi_switch_proto_t *ptr;
  int32_t          status = RSI_SUCCESS;
	
  //! Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  if ((type == 1) && (callback != NULL)) {
    /* in ENABLE case, callback must be NULL */
    return RSI_ERROR_INVALID_PARAM;
  }
 
  
  if(rsi_check_and_update_cmd_state(COMMON_CMD,IN_USE) == RSI_SUCCESS)
  {

  //! allocate command buffer
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }
  //! Take the user provided data and fill it in  swicth proto structure
  ptr = (rsi_switch_proto_t *)pkt->data;

  //! type
  ptr->mode_value = type;

  common_cb->sync_mode = type;
#ifdef RSI_WLAN_ENABLE
  if(callback != NULL) 
  {
    //! Set socket asynchronous receive callback
    rsi_wlan_cb_non_rom->switch_proto_callback = callback;

  } else
  {
	  //! Set socket asynchronous receive callback
	  rsi_wlan_cb_non_rom->switch_proto_callback = NULL;

  }
#endif

  //! send switch proto command
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_SWITCH_PROTO, pkt);

  //! wait on common semaphore
  rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_SWITCH_PROTO_RESPONSE_WAIT_TIME);

  //!Changing the common state to allow state
  rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

  }

  else
  {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

  //! get common command response stattus
  status = rsi_common_get_status();

  //! Return the status
  return status;

}

/*==============================================*/
/**
 * @fn          void rsi_wireless_driver_task(void)
 * @brief       This function handles driver's events
 * @param[in]   None 
 * @param[out]  None
 * @return      void
 *
 *
 * @section description
 * This API is used to handle driver's events for non-OS platforms
 *
 *
 */

void rsi_wireless_driver_task(void)
{
#ifdef RSI_WITH_OS
  while(1)
#endif
  {
    rsi_scheduler(&rsi_driver_cb->scheduler_cb);
#ifdef RSI_WITH_OS
#if (BT_A2DP_SOURCE_SBC_CODEC || UNIFIED_PROTOCOL || COEX_MAX_APP || COEX_TEST_FRAMEWORK)
    //rsi_bt_a2dp_sbc_encode_task();
#endif
#endif

  }
}

//======================================================
/**
 *
 * @fn         rsi_driver_cb_t *rsi_driver_deinit()
 * @brief      De-Initializes the driver components.Clears all the memory 
 *             given for driver operations in rsi_driver_init() API
 *             in OS case USer need to take care of OS variables initialised in 
 *             driver_init .This API has to be called by the thread/task/Master 
 *             thread which is not dependent on OS variables allocated/initialised
 *             in rsi_driver_init() API
 * @param[in]   None
 * @param[out]  None
 * @return
 *              0 - success
 *              1 - Failure 
 * @section description
 * This fucntion de-intializes driver components
 *
 */
int32_t rsi_driver_deinit(void)
{
  int32_t status = RSI_SUCCESS;
  uint32_t actual_length=0;
  //! check buffer is enough or not for driver components
  actual_length += rsi_driver_memory_estimate();
  if(buffer_addr == NULL)
  {
    return RSI_FAILURE; 
  }
#ifndef RSI_M4_INTERFACE
  rsi_hal_intr_mask();
#else
 mask_ta_interrupt(RX_PKT_TRANSFER_DONE_INTERRUPT|TX_PKT_TRANSFER_DONE_INTERRUPT);
#endif
  rsi_semaphore_destroy(&rsi_driver_cb->scheduler_cb.scheduler_sem);
#ifdef WAKEUP_GPIO_INTERRUPT_METHOD
  rsi_semaphore_destroy(&rsi_driver_cb->common_cb->wakeup_gpio_sem);
#endif
#ifdef RSI_WITH_OS  
  rsi_semaphore_destroy(&rsi_driver_cb->common_cb->common_card_ready_sem);
#endif
  status = rsi_semaphore_destroy(&rsi_driver_cb->common_cb->common_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  rsi_mutex_destroy(&rsi_driver_cb->common_cb->common_mutex);
  rsi_mutex_destroy(&rsi_driver_cb_non_rom->tx_mutex);
  status = rsi_semaphore_destroy(&rsi_driver_cb->rx_pool.pkt_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->common_cb->common_tx_pool.pkt_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb_non_rom->common_cmd_send_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb_non_rom->common_cmd_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
#ifdef RSI_WLAN_ENABLE
  //! Create wlan semaphore
  status = rsi_semaphore_destroy(&rsi_driver_cb_non_rom->nwk_cmd_send_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb_non_rom->nwk_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb_non_rom->wlan_cmd_send_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb_non_rom->wlan_cmd_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->wlan_cb->wlan_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->wlan_cb->wlan_tx_pool.pkt_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  rsi_mutex_destroy(&rsi_driver_cb->wlan_cb->wlan_mutex);
  rsi_socket_pool = NULL;
  rsi_socket_pool_non_rom = NULL;
#endif
#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_ANT_ENABLE)
  //! Create bt semaphore
  status = rsi_semaphore_destroy(&rsi_driver_cb->bt_common_cb->bt_cmd_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->bt_common_cb->bt_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->bt_common_cb->bt_tx_pool.pkt_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
#ifdef SAPIS_BT_STACK_ON_HOST
  status = rsi_semaphore_destroy(&rsi_driver_cb->bt_ble_stack_cb->bt_tx_pool.pkt_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
#endif
#ifdef RSI_BT_ENABLE
  status = rsi_semaphore_destroy(&rsi_driver_cb->bt_classic_cb->bt_cmd_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->bt_classic_cb->bt_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->bt_classic_cb->bt_tx_pool.pkt_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
#endif
#ifdef RSI_BLE_ENABLE
  status = rsi_semaphore_destroy(&rsi_driver_cb->ble_cb->bt_cmd_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->ble_cb->bt_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->ble_cb->bt_tx_pool.pkt_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
#endif
#ifdef RSI_ANT_ENABLE
  status = rsi_semaphore_destroy(&rsi_driver_cb->ant_cb->bt_cmd_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
#endif
#endif
#ifdef RSI_ZB_ENABLE
  status = rsi_semaphore_destroy(&rsi_driver_cb->zigb_cb->zigb_tx_pool.pkt_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->zigb_cb->zigb_sem);
  if(status != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  rsi_mutex_destroy(&rsi_driver_cb->zigb_cb->zigb_mutex);
#endif
  //!memsetting the global structure
  memset((uint8_t *)buffer_addr,0,actual_length);

  //! Clear the driver control block, assume application will free the buffer
  buffer_addr = NULL;

  return RSI_SUCCESS;

}
/*=============================================================================*/
/**
 * @fn              int32_t rsi_driver_version(uint8_t *request)
 * @brief           get current driver version
 * @param[in]       uint8_t *request,pointer to fill driver version
 * @return          0- Success.
 */
#define RSI_DRIVER_VERSION "1610.2.0.0.0024"
int32_t rsi_driver_version(uint8_t *request)
{
  memcpy(request ,(uint8_t *)RSI_DRIVER_VERSION,sizeof(RSI_DRIVER_VERSION));
  return RSI_SUCCESS;
}

/*==============================================*/
/**
 * @fn          int32_t rsi_set_rtc_timer()
 * @brief       This API is used to set the host rtc timer 
 * @param[in]   uint8_t *timer,pointer to fill rtc time 
 * This API is used to set the rtc timer  
 *
 */
int32_t rsi_set_rtc_timer(module_rtc_time_t *timer)
{
  int32_t    status = RSI_SUCCESS;
  rsi_pkt_t             *pkt;
  module_rtc_time_t        *rtc;
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  if(rsi_check_and_update_cmd_state(COMMON_CMD,IN_USE) == RSI_SUCCESS)
  {

    //! allocate command buffer  from common pool
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    //! If allocation of packet fails
    if(pkt == NULL)
    {
      //!Changing the common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);
      //! return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    //! Take the user provided data and fill it in module rtc time structure
    rtc = (module_rtc_time_t *)pkt->data;
   
    memcpy(&rtc->tm_sec, &timer->tm_sec, 4);
    memcpy(&rtc->tm_min, &timer->tm_min, 4);
    memcpy(&rtc->tm_hour, &timer->tm_hour, 4);
    memcpy(&rtc->tm_mday, &timer->tm_mday, 4);
    memcpy(&rtc->tm_mon, &timer->tm_mon, 4);
    memcpy(&rtc->tm_year, &timer->tm_year, 4);
    memcpy(&rtc->tm_wday, &timer->tm_wday, 4);

    //! Send set rtc timer request
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_SET_RTC_TIMER, pkt);

    //! wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_SET_RTC_TIMER_RESPONSE_WAIT_TIME);

    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

  }

  else
  {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

  //! get common command response stattus
  status = rsi_common_get_status();

  //! Return the status
  return status;

}

//*==============================================*/
/**
 * @fn          int32_t rsi_get_ram_log(uint32_t addr,uint16_t length)
 * @brief       This API is used to get ram dump 
 * @param[in]   uint32_t addr  
 * @param[in]   uint16_t length
 * This API is used to get ram log on UART  
 *
 */
int32_t rsi_get_ram_log(uint32_t addr, uint32_t length)
{
  int32_t    status = RSI_SUCCESS;
  rsi_pkt_t             *pkt;
  rsi_ram_dump_t        *ram;
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  if(rsi_check_and_update_cmd_state(COMMON_CMD,IN_USE) == RSI_SUCCESS)
  {

  //! allocate command buffer  from common pool
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  //! Take the user provided data and fill it in ram_dump structure
  ram = (rsi_ram_dump_t *)pkt->data;
  //! address
  ram->addr = addr;
  //! length
  ram->length = length;
  //! Send ram dump request
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_GET_RAM_DUMP, pkt);

  //! wait on common semaphore
  rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_GET_RAM_DUMP_RESPONSE_WAIT_TIME);

  //!Changing the common state to allow state
  rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

  }

  else
  {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

  //! get common command response stattus
  status = rsi_common_get_status();

  //! Return the status
  return status;

}

/*==============================================*/
/**
 *
 * @fn         int32_t rsi_driver_memory_estimate()
 * @brief      Provides the length of the application
 * @param[in]  None
 * @param[out] None
 * @return
 *       <=length - If success, returns the memory used, which is less or equal to buffer length provided
 *        >length - If fails, returns total memory required by driver
 * @section description
 * This fucntion Provides the length of the application
 *
 */
int32_t rsi_driver_memory_estimate(void)
{
  uint32_t        actual_length  = 0;

  //! Calculate the Memory length of the application
  actual_length += RSI_DRIVER_POOL_SIZE ;
  return actual_length;
}

void rsi_uregister_events_callbacks(void (*callback_handler_ptr)(uint32_t event_num))
{
  global_cb_p->rsi_driver_cb->unregistered_event_callback = callback_handler_ptr;
}

//*==============================================*/
/**
 * @fn          int32_t rsi_get_rtc_timer()
 * @brief       This API is used to set the host rtc timer 
 * @param[in]   None 
 * This API is used to get the rtc timer  
 *
 */
int32_t rsi_get_rtc_timer(module_rtc_time_t *response)
{
  int32_t    status = RSI_SUCCESS;
  rsi_pkt_t             *pkt;
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  if(rsi_check_and_update_cmd_state(COMMON_CMD,IN_USE) == RSI_SUCCESS)
	{
  //! allocate command buffer  from common pool
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //! unlock mutex
    rsi_mutex_unlock(&common_cb->common_mutex);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }
  
  if(response != NULL)
  {
    //! attach the buffer given by user
    common_cb->app_buffer = (uint8_t *)response;

    //! length of the buffer provided by user 
    common_cb->app_buffer_length = sizeof(module_rtc_time_t); 
  }
  else
  {
    //! Assign NULL to the app_buffer to avoid junk
    common_cb->app_buffer = NULL;

    //! length of the buffer to 0 
    common_cb->app_buffer_length = 0; 

  }

  //! Send get rtc timer request
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_GET_RTC_TIMER, pkt);
//! wait on common semaphore
  rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_GET_RTC_TIMER_RESPONSE_WAIT_TIME);

  //!Changing the common state to allow state
  rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

  }

  else
  {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

  //! get common command response stattus
  status = rsi_common_get_status();
	
  //! Return the status
  return status;

}
#ifdef CONFIGURE_GPIO_FROM_HOST
//*==============================================*/
/**
 * @fn          int32_t rsi_gpio_pininit(uint8_t pin_num,uint8_t configuration)
 * @brief       This function is used to configure TA GPIO's using Command from host 
 * @param[in]   pin_num - : GPIO Number : Valid values  0 - 15 
 *   
 *  configuartion
//! BIT[4]     - 1 - input mode 0 - Output mode
//! BIT[0 - 1] - Drive strength : 0 - 2mA / 1 - 4mA / 2 - 8mA / 3 -12mA
//! BIT[6 - 7] - 0 - Hi-Z / 1- Pull-up / 2- Pull-down
 */

int32_t rsi_gpio_pininit(uint8_t pin_num,uint8_t configuration)
{
  int32_t    status = RSI_SUCCESS;
  rsi_pkt_t             *pkt;
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
  rsi_gpio_pin_config_t *config_gpio;
  //! take lock on common control block
  rsi_mutex_lock(&common_cb->common_mutex);
  
  //! allocate command buffer  from common pool
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //! unlock mutex
    rsi_mutex_unlock(&common_cb->common_mutex);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  config_gpio = (rsi_gpio_pin_config_t *)pkt->data;
  
  config_gpio->config_mode = RSI_CONFIG_GPIO;
  config_gpio->pin_num = pin_num;
  config_gpio->output_value = 0;
  config_gpio->config_values = configuration;

  //! Send GPIO configuration request
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_GPIO_CONFIG, pkt);

  //! free the packet
  rsi_pkt_free(&common_cb->common_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&common_cb->common_mutex);

  //! Return the status
  return status;

}


//*==============================================*/
/**
 * @fn          int32_t rsi_gpio_writepin(uint8_t pin_num,uint8_t value)
 * @brief       This function is used to write the  TA GPIO's high or low using Command from host 
 * @param[in]   pin_num - : GPIO Number : Valid values  0 - 15 
 *              value  : Value to be driven on GPIO
 *                         1 - Drive high
 *                         0 - Drive Low 
 *   
 */

int32_t rsi_gpio_writepin(uint8_t pin_num,uint8_t value)
{
  int32_t    status = RSI_SUCCESS;
  rsi_pkt_t             *pkt;
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
  rsi_gpio_pin_config_t *config_gpio;

  //! take lock on common control block
  rsi_mutex_lock(&common_cb->common_mutex);
  
  //! allocate command buffer  from common pool
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //! unlock mutex
    rsi_mutex_unlock(&common_cb->common_mutex);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }
  config_gpio = (rsi_gpio_pin_config_t *)pkt->data;
  
  config_gpio->config_mode = RSI_SET_GPIO;
  config_gpio->pin_num = pin_num;
  config_gpio->output_value = value;
  config_gpio->config_values = 0;

 
  //! Send GPIO configuration request
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_GPIO_CONFIG, pkt);

  //! free the packet
  rsi_pkt_free(&common_cb->common_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&common_cb->common_mutex);

  //! Return the status
  return status;

}

//*==============================================*/
/**
 * @fn          int32_t rsi_gpio_readpin(uint8_t pin_num,uint8_t *gpio_value)
 * @brief       This function is used to read the TA GPIO's using Command from host 
 * @param[in]   pin_num - : GPIO Number : Valid values  0 - 15 
 *              gpio_value  : Address of variable to store the value
 *                         
 *                       
 *   
 */

int32_t rsi_gpio_readpin(uint8_t pin_num,uint8_t *gpio_value)
{
  int32_t    status = RSI_SUCCESS;
  rsi_pkt_t             *pkt;
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
  rsi_gpio_pin_config_t *config_gpio;

  //! take lock on common control block
  rsi_mutex_lock(&common_cb->common_mutex);
  
  //! allocate command buffer  from common pool
  pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //! unlock mutex
    rsi_mutex_unlock(&common_cb->common_mutex);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }
  config_gpio = (rsi_gpio_pin_config_t *)pkt->data;
  
  config_gpio->config_mode = RSI_GET_GPIO;
  config_gpio->pin_num = pin_num;
  config_gpio->output_value = 0;
  config_gpio->config_values = 0;

  //! attach the buffer given by user
    common_cb->app_buffer = (uint8_t *)gpio_value;
  //! length of the buffer provided by user 
  common_cb->app_buffer_length = 1; 

  //! Send GPIO configuration request
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_GPIO_CONFIG, pkt);
 
  //! free the packet
  rsi_pkt_free(&common_cb->common_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&common_cb->common_mutex);

  //! Return the status
  return status;

}

#endif


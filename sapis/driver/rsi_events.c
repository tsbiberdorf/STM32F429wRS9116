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
#ifdef RSI_WITH_OS
#include "FreeRTOS.h"
#include "semphr.h"
#endif

#if (defined(RSI_WITH_OS) && defined(MXRT_595s))
extern rsi_task_handle_t driver_task_handle;
/**
 * Global Variables
 */

/*====================================================*/
/**
 * @fn          void rsi_set_event_non_rom( uint32_t event_num)
 * @brief       Sets an event
 * @param[in]   uint32_t event_num, event number to be set
 * @param[in]   global_cb_t *global_cb_p,pointer to the common buffer
 * @return      void
 * @section description 
 * This function sets an event
 */
void rsi_set_event_non_rom(uint32_t event_num)
{
  rsi_driver_cb_t *rsi_driver_cb = global_cb_p->rsi_driver_cb;

#ifdef RSI_M4_INTERFACE
  //! mask P2P interrupt
  RSI_MASK_TA_INTERRUPT();
#endif

  //! Set the event bit in bitmap
  rsi_driver_cb->scheduler_cb.event_map |= BIT(event_num);

#ifdef RSI_M4_INTERFACE
  //! unmask P2P interrupt
  RSI_UNMASK_TA_INTERRUPT();
#endif  

  if(global_cb_p->os_enabled == 1)
  {
    //! notify to driver task
    rsi_task_notify( driver_task_handle, BIT(event_num) );
  }
}

/*====================================================*/
/**
 * @fn          void rsi_set_event_from_isr_non_rom(uint32_t event_num)
 * @brief       Sets an event from ISR
 * @param[in]   uint32_t event_num, event number to be set
 * @param[in]   global_cb_t *global_cb_p,pointer to the common buffer
 * @return      void
 * @section description
 * This function sets an event from ISR
 */
void rsi_set_event_from_isr_non_rom(uint32_t event_num)
{
  rsi_driver_cb_t *rsi_driver_cb = global_cb_p->rsi_driver_cb;

  //! Set the event bit in scheduler bitmap
  rsi_driver_cb->scheduler_cb.event_map |= BIT(event_num);

  rsi_task_notify_from_isr(driver_task_handle, BIT(event_num), NULL);
}

/*====================================================*/
/**
 * @fn          void rsi_unmask_event_from_isr_non_rom(uint32_t event_num)
 * @brief       Unmasks an event from ISR
 * @param[in]   uint32_t event_num, event number to unmask
 * @param[in]   global_cb_t *global_cb_p,pointer to the common buffer
 * @return      void
 * @section description
 * This function is used to unmask an event from ISR
 */

void rsi_unmask_event_from_isr_non_rom(uint32_t event_num)
{
  //! set event bit in mask bitmap
  global_cb_p->rsi_driver_cb->scheduler_cb.mask_map |= BIT(event_num);

  //! notify to driver task
  rsi_task_notify_from_isr(driver_task_handle, BIT(event_num), NULL);
}

/*====================================================*/
/**
 * @fn          void rsi_unmask_event_non_rom(uint32_t event_num)
 * @brief       Unmasks an event
 * @param[in]   uint32_t event_num, event number to unmask
 * @param[in]   global_cb_t *global_cb_p,pointer to the common buffer
 * @return      void
 * @section description 
 * This function is used to unmask an event
 */
void rsi_unmask_event_non_rom(uint32_t event_num)
{
#ifdef RSI_M4_INTERFACE
  //! mask P2P interrupt
  RSI_MASK_TA_INTERRUPT();
#endif

  //! set event bit in mask bitmap
  global_cb_p->rsi_driver_cb->scheduler_cb.mask_map |= BIT(event_num);

#ifdef RSI_M4_INTERFACE
  //! unmask P2P interrupt
  RSI_UNMASK_TA_INTERRUPT();
#endif

  //! notify to driver task
  rsi_task_notify( driver_task_handle, BIT(event_num) );
}
#endif
/*====================================================*/
/**
 * @fn          void rsi_set_event(uint32_t event_num)
 * @brief       Sets an event
 * @param[in]   uint32_t event_num, event number to be set
 * @return      void
 * @section description 
 * This function sets an event
 */
void rsi_set_event(uint32_t event_num)
{
#if (defined(RSI_WITH_OS) && defined(MXRT_595s))
  rsi_set_event_non_rom(event_num);
#elif ROM_WIRELESS
  ROMAPI_WL->rsi_set_event(global_cb_p, event_num);
#else
  api_wl->rsi_set_event(global_cb_p, event_num);
#endif
}

/*====================================================*/
/**
 * @fn          void rsi_clear_event(uint32_t event_num)
 * @brief       Clears an event
 * @param[in]   uint32_t event_num, event number to clear
 * @return      void
 * @section description 
 * This function clears an event
 */
void rsi_clear_event(uint32_t event_num)
{
#ifdef ROM_WIRELESS
  ROMAPI_WL->rsi_clear_event(global_cb_p, event_num);
#else
  api_wl->rsi_clear_event(global_cb_p, event_num);
#endif
}

/*====================================================*/
/**
 * @fn          void rsi_mask_event(uint32_t event_num)
 * @brief       Masks an event
 * @param[in]   uint32_t event_num, event number to mask
 * @return      void
 * @section description 
 * This function masks an event
 */
void rsi_mask_event(uint32_t event_num)
{
#ifdef ROM_WIRELESS
  ROMAPI_WL->rsi_mask_event(global_cb_p, event_num);
#else
  api_wl->rsi_mask_event(global_cb_p, event_num);
#endif
}

/*====================================================*/
/**
 * @fn          void rsi_unmask_event(uint32_t event_num)
 * @brief       Unmasks an event
 * @param[in]   uint32_t event_num, event number to unmask
 * @return      void
 * @section description 
 * This function is used to unmask an event
 */
void rsi_unmask_event(uint32_t event_num)
{
#if (defined(RSI_WITH_OS) && defined(MXRT_595s))
  rsi_unmask_event_non_rom(event_num);
#elif ROM_WIRELESS
  ROMAPI_WL->rsi_unmask_event(global_cb_p, event_num);
#else
  api_wl->rsi_unmask_event(global_cb_p, event_num);
#endif
}

/*====================================================*/
/**
 * @fn          void rsi_unmask_event_from_isr(uint32_t event_num)
 * @brief       Unmasks an event from isr context
 * @param[in]   uint32_t event_num, event number to unmask
 * @return      void
 * @section description 
 * This function is used to unmask an event
 */
void rsi_unmask_event_from_isr(uint32_t event_num)
{
#if (defined(RSI_WITH_OS) && defined(MXRT_595s))
  rsi_unmask_event_from_isr_non_rom(event_num);
#elif ROM_WIRELESS
  ROMAPI_WL->rsi_unmask_event_from_isr(global_cb_p, event_num);
#else
  api_wl->rsi_unmask_event_from_isr(global_cb_p, event_num);
#endif
}

/*====================================================*/
/**
 * @fn          void rsi_find_event(uint32_t event_map)
 * @brief       Returns the event set from the event map
 * @param[in]   uint32_t event_map, event map
 * @return      void
 * @section description 
 * This function is used find the event which is set from the map
 */
uint32_t rsi_find_event(uint32_t event_map)
{
#ifdef ROM_WIRELESS
  return ROMAPI_WL->rsi_find_event(global_cb_p, event_map);
#else
  return api_wl->rsi_find_event(global_cb_p, event_map);
#endif
}

/*====================================================*/
/**
 * @fn          uint16_t rsi_register_event(uint32_t event_id, void (*event_handler_ptr)(void))
 * @brief       Registers the event
 * @param[in]   uint32_t event_id, event number which needs to be registered
 * @param[in]   void (*event_handler_ptr)(void), event handler which needs to be registered for a given event
 * @return      
 *                1 -  error
 *                0 - Success
 * @section description 
 * This function is used to register the event
 */
uint16_t rsi_register_event(uint32_t event_id, void (*event_handler_ptr)(void))
{
#ifdef ROM_WIRELESS
  return ROMAPI_WL->rsi_register_event(global_cb_p, event_id, event_handler_ptr);
#else
  return api_wl->rsi_register_event(global_cb_p, event_id, event_handler_ptr);
#endif
}

/*====================================================*/
/**
 * @fn          void rsi_set_event_from_isr(uint32_t event_num)
 * @brief       Sets the event from isr context
 * @param[in]   uint32_t event_id, event number which needs to be registered
 * @return      
 *                None
 * @section description 
 * This function is used to set the event from isr
 */
 
void rsi_set_event_from_isr(uint32_t event_num)
{
#if (defined(RSI_WITH_OS) && defined(MXRT_595s))
  rsi_set_event_from_isr_non_rom(event_num);
#elif ROM_WIRELESS
  ROMAPI_WL->rsi_set_event_from_isr(global_cb_p, event_num);
#else
  api_wl->rsi_set_event_from_isr(global_cb_p, event_num);
#endif
}

/*====================================================*/
/**
 * @fn          void rsi_events_init(void)
 * @brief       Initializes events
 * @param[in]   None
 * @param[out]  None
 * @return      void
 * @section description 
 * This function is used to initialize the events
 */
void rsi_events_init(void)
{
  //! Initialze RX event
  rsi_register_event(RSI_RX_EVENT, rsi_rx_event_handler);

  //! Initialze TX event
  rsi_register_event(RSI_TX_EVENT, rsi_tx_event_handler);
	
#ifndef RSI_M4_INTERFACE
#ifdef RSI_WLAN_ENABLE
	//! Initialze Socket event
	rsi_register_event(RSI_SOCKET_EVENT, rsi_socket_event_handler);
#endif  
#endif  
  
#ifdef RSI_ENABLE_DEMOS
#if (BT_A2DP_SOURCE_WIFI_HTTP_S_RX || BLE_DUAL_MODE_BT_A2DP_SOURCE_WIFI_HTTP_S_RX || BT_A2DP_SOURCE_WIFI_HTTP_S_RX_DYN_COEX)
  //! Initialize application event1
  rsi_register_event(RSI_APP_EVENT1, rsi_app_event1_handler);

  //! Initialize application event2
  rsi_register_event(RSI_APP_EVENT2, rsi_app_event2_handler);
#endif
#endif
	

}




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
/**
 * Global Variables
 */

#if (defined(RSI_WITH_OS) && defined(MXRT_595s))
volatile uint32_t rsi_driver_eventmap;
#endif
/*====================================================*/
/**
 * @fn          uint16_t rsi_scheduler_init(rsi_scheduler_cb_t *scheduler_cb)
 * @brief       Initializes scheduler
 * @param[in]   rsi_scheduler_cb_t scheduler_cb, pointer to scheduler structure
 * @return      void
 * @section description 
 * This function is used initialize scheduler
 */
void rsi_scheduler_init(rsi_scheduler_cb_t *scheduler_cb)
{
#ifdef ROM_WIRELESS
  ROMAPI_WL->rsi_scheduler_init(global_cb_p, scheduler_cb);
#else
  api_wl->rsi_scheduler_init(global_cb_p, scheduler_cb);
#endif
  return;
}


uint32_t rsi_get_event(rsi_scheduler_cb_t *scheduler_cb)
{
#if ((defined RSI_SPI_INTERFACE) || ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM)))
  return rsi_get_event_non_rom(scheduler_cb);
#elif ROM_WIRELESS
  return ROMAPI_WL->rsi_get_event(global_cb_p, scheduler_cb);
#else
  return api_wl->rsi_get_event(global_cb_p,scheduler_cb);
#endif
}
  

/*====================================================*/
/**
 * @fn          void rsi_scheduler(rsi_scheduler_cb_t *scheduler_cb)
 * @brief       scheduler to handle events
 * @param[in]   rsi_scheduler_cb_t *scheduler_cb, pointer to scheduler cb structure
 * @return      void
 * @section description 
 * This scheduler function handles events
 */
void rsi_scheduler(rsi_scheduler_cb_t *scheduler_cb)
{
#if (defined(RSI_WITH_OS) && defined(MXRT_595s))
  rsi_scheduler_non_rom(scheduler_cb);
#elif ROM_WIRELESS
  ROMAPI_WL->rsi_scheduler(global_cb_p, scheduler_cb);
#else
  api_wl->rsi_scheduler(global_cb_p, scheduler_cb);
#endif
}



uint32_t rsi_get_event_non_rom(rsi_scheduler_cb_t *scheduler_cb)
{
  uint32_t       active_int_event_map;
  
  rsi_reg_flags_t flags;
  
  //! Disable all the interrupts 
  flags = RSI_CRITICAL_SECTION_ENTRY();
 
  //! Get current event map after applying mask
  active_int_event_map = (scheduler_cb->event_map & scheduler_cb->mask_map);
#if ((defined RSI_SPI_INTERFACE) || ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM)))
  if(rsi_hal_intr_pin_status() && 
      (!(active_int_event_map & BIT(RSI_RX_EVENT))))
  {
    //! Disable the interrupt 
    rsi_hal_intr_mask();
    //! Set event RX pending from device
    rsi_set_event(RSI_RX_EVENT);
  }
#endif 

  //! Enable all the interrupts
  RSI_CRITICAL_SECTION_EXIT(flags);
  
  return active_int_event_map;
  
}

#if (defined(RSI_WITH_OS) && defined(MXRT_595s))
/*====================================================*/
/**
 * @fn          void rsi_scheduler_non_rom(rsi_scheduler_cb_t *scheduler_cb)
 * @brief       scheduler to handle events
 * @param[in]   global_cb_p,pointer to the global control block
 * @param[in]   rsi_scheduler_cb_t *scheduler_cb, pointer to scheduler cb structure
 * @return      void
 * @section description 
 * This scheduler function handles events
 */
void rsi_scheduler_non_rom(rsi_scheduler_cb_t *scheduler_cb)
{
  uint32_t       event_no;
  rsi_event_cb_t *temp_event;

  rsi_driver_cb_t *rsi_driver_cb = global_cb_p->rsi_driver_cb;
  uint32_t MaxBlockTime = 0xFFFFFFFF;
  rsi_base_type_t xResult;
  //! Wait for events
  xResult = rsi_task_notify_wait( 0,    /* Don't clear bits on entry. */
      MaxBlockTime,   /* Clear all bits on exit. */
      (uint32_t *)&rsi_driver_eventmap,   /* Stores the notified value. */
      MaxBlockTime);
  if(xResult == 1)
  { 
    while(rsi_get_event(scheduler_cb))
    {
      //! Find event event 
      event_no = rsi_find_event((scheduler_cb->event_map & scheduler_cb->mask_map));

      //! Get event handler
      temp_event = &rsi_driver_cb->event_list[event_no];

      if(temp_event->event_handler)
      {
        //! Call event handler
        temp_event->event_handler();
      }
      else 
      {

        rsi_clear_event(event_no);  //Clear unregistered events
        global_cb_p->rsi_driver_cb->unregistered_event_callback(event_no);
      }

      if(global_cb_p->os_enabled != 1)
      {
        break;
      }
    }
  }  
}
#endif 

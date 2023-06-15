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
 #ifndef ROM_WIRELESS
#include "rsi_driver.h"
/**
 * Global Variables
 */

/*====================================================*/
/**
 * @fn          void ROM_WL_rsi_scheduler_init(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb)
 * @brief       Initializes scheduler
 * @param[in]   global_cb_p,pointer to the global control block
 * @param[in]   rsi_scheduler_cb_t scheduler_cb, pointer to scheduler structure
 * @return      void
 * @section description 
 * This function is used initialize scheduler
 */
void ROM_WL_rsi_scheduler_init(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb)
{
  //! Set no event
  scheduler_cb->event_map = 0x0;

  //! Set no mask
  scheduler_cb->mask_map = 0xFFFFFFFF;
 
  if(global_cb_p->os_enabled == 1)
  {
    //! create semaphore
    RSI_SEMAPHORE_CREATE(&scheduler_cb->scheduler_sem, 0);
  }

  return;
}


uint32_t ROM_WL_rsi_get_event(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb)
{
  uint32_t       active_int_event_map;
  
  rsi_reg_flags_t flags;
  
  //! Disable all the interrupts 
  flags = RSI_CRITICAL_SECTION_ENTRY();
 
  //! Get current event map after applying mask
  active_int_event_map = (scheduler_cb->event_map & scheduler_cb->mask_map);

  //! Enable all the interrupts
  RSI_CRITICAL_SECTION_EXIT(flags);
  
  return active_int_event_map;
  
}
  

/*====================================================*/
/**
 * @fn          void ROM_WL_rsi_scheduler(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb)
 * @brief       scheduler to handle events
 * @param[in]   global_cb_p,pointer to the global control block
 * @param[in]   rsi_scheduler_cb_t *scheduler_cb, pointer to scheduler cb structure
 * @return      void
 * @section description 
 * This scheduler function handles events
 */
void ROM_WL_rsi_scheduler(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb)
{
  uint32_t       event_no;
  rsi_event_cb_t *temp_event;

  rsi_driver_cb_t *rsi_driver_cb = global_cb_p->rsi_driver_cb;
 
  while(ROM_WL_rsi_get_event(global_cb_p, scheduler_cb))
  {
    //! Find event event 
    event_no = ROM_WL_rsi_find_event(global_cb_p, (scheduler_cb->event_map & scheduler_cb->mask_map));

    //! Get event handler
    temp_event = &rsi_driver_cb->event_list[event_no];

    if(temp_event->event_handler)
    {
      //! Call event handler
      temp_event->event_handler();
    }
    else 
    {

      ROM_WL_rsi_clear_event(global_cb_p, event_no);  //Clear unregistered events
      global_cb_p->rsi_driver_cb->unregistered_event_callback(event_no);
    }

    if(global_cb_p->os_enabled != 1)
    {
      break;
    }
  }
  
  if(global_cb_p->os_enabled == 1)
  {
    RSI_SEMAPHORE_WAIT(&scheduler_cb->scheduler_sem, 0);
  }
}
#endif

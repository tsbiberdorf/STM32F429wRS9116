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

#ifndef RSI_SCHEDULER_H
#define RSI_SCHEDULER_H
/******************************************************
 * *                      Macros
 * ******************************************************/
/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

//! scheduler control block structure 
typedef struct rsi_scheduler_cb_s
{
  //! This bitmap is used to set and clear events 
  volatile uint32_t event_map;

  //! This bitmap is used to mask and unmask events 
  volatile uint32_t mask_map;

	//! scheduler semaphore
	rsi_semaphore_handle_t scheduler_sem;
}rsi_scheduler_cb_t;


/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
void rsi_scheduler_init(rsi_scheduler_cb_t *scheduler_cb);
void rsi_scheduler(rsi_scheduler_cb_t *scheduler_cb);
uint32_t rsi_get_event_non_rom(rsi_scheduler_cb_t *scheduler_cb);
#endif

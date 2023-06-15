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

#ifndef  RSI_TIMER_H
#define  RSI_TIMER_H
/******************************************************
 * *                      Macros
 * ******************************************************/
#define RSI_TIMER_NODE_0 0
/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/
typedef uint32_t rsi_timer_instance_t;
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
void rsi_timer_expiry_interrupt_handler(void);
uint32_t rsi_timer_read_counter(void);
uint32_t rsi_hal_gettickcount(void);
void rsi_init_timer(rsi_timer_instance_t *rsi_timer, uint32_t duration);
int32_t rsi_timer_expired(rsi_timer_instance_t *timer);
uint32_t rsi_timer_left(rsi_timer_instance_t *timer);

#endif

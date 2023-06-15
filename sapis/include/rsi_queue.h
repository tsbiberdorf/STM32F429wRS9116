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

#ifndef RSI_QUEUE_H
#define RSI_QUEUE_H
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
/******************************************************
 * *                    Structures
 * ******************************************************/

//! queue structure 
typedef struct rsi_queue_cb_s
{
  //! queue head 
  rsi_pkt_t *head;               

  //! queue tail
  rsi_pkt_t *tail;               

  //! number of packets pending in queue
  volatile uint16_t pending_pkt_count;         

  //! mask status of the queue
  uint16_t queue_mask;              
  
  //! Mutex handle to queues
  rsi_mutex_handle_t queue_mutex;

}rsi_queue_cb_t;

/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
extern void rsi_queues_init(rsi_queue_cb_t *queue);
extern void rsi_enqueue_pkt(rsi_queue_cb_t *queue,rsi_pkt_t *pkt);
extern rsi_pkt_t *rsi_dequeue_pkt(rsi_queue_cb_t *queue);
extern uint32_t rsi_check_queue_status(rsi_queue_cb_t *queue);
extern void rsi_block_queue(rsi_queue_cb_t *queue);
extern void rsi_unblock_queue(rsi_queue_cb_t *queue);
#endif

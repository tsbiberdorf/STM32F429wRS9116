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

#ifndef RSI_PKT_MGMT_H
#define RSI_PKT_MGMT_H

#include <stdint.h>
#include <rsi_os.h>
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
//! driver TX/RX packet structure
typedef struct rsi_pkt_s
{
  //! next packet pointer 
  struct rsi_pkt_s *next;

  //! host descriptor 
  uint8_t desc[16];

  //! payload 
  uint8_t data[1];
}rsi_pkt_t;

//! packet pool structure  
typedef struct rsi_pkt_pool_s
{
  //! Pool total packets count
  uint16_t size;

  //! Pool avaialble packets count
  uint16_t avail;

  //! Pool pointer
  void     **pool;
  
  rsi_semaphore_handle_t pkt_sem;

}rsi_pkt_pool_t;

/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
int32_t rsi_pkt_pool_init(rsi_pkt_pool_t *pool_cb, uint8_t *buffer, uint32_t total_size, uint32_t pkt_size);
rsi_pkt_t * rsi_pkt_alloc(rsi_pkt_pool_t *pool_cb);
int32_t rsi_pkt_free(rsi_pkt_pool_t *pool_cb,rsi_pkt_t *pkt);
uint32_t rsi_is_pkt_available(rsi_pkt_pool_t *pool_cb);
#endif

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
 * */
#include "rsi_driver.h"

/*==============================================*/
/**
 * @fn         int32_t rsi_pkt_pool_init(rsi_pkt_pool_t *pool_cb, uint8_t *buffer, uint32_t total_size, uint32_t pkt_size)
 * @brief      Initilalizes packet pool
 * @param[in]  pool_cb , packet pool pointer which needs to be initialized  
 * @param[in]  buffer  , buffer pointer to create pool 
 * @param[in]  total_size , size of pool to create 
 * @param[in]  pkt_size , size of each packet in pool
 * @param[out] None
 * @return    
 *             <0  - If fails
 *             =0  - If success
 *
 * @section description
 * This function initializes/creates packet pool from the provided buffer
 *
 *
 */
int32_t rsi_pkt_pool_init(rsi_pkt_pool_t *pool_cb, uint8_t *buffer, uint32_t total_size, uint32_t pkt_size)
{
#ifdef ROM_WIRELESS
  return ROMAPI_WL->rsi_pkt_pool_init(global_cb_p, pool_cb, buffer, total_size, pkt_size);
#else
  return api_wl->rsi_pkt_pool_init(global_cb_p, pool_cb, buffer, total_size, pkt_size);
#endif
}

/*==============================================*/
/**
 * @fn          rsi_pkt_t * rsi_pkt_alloc(rsi_pkt_pool_t *pool_cb)
 * @brief       Allocates packet from packet pool
 * @param[in]   pool_cb , packet pool pointer from which packet needs to be allocated 
 * @param[out]  None
 * @return
 *              = NULL - If allocation fails
 *              = Allocated packet pointer if success
 *
 *
 * @section description
 * This function allocates packet from a given packet pool 
 *
 */

rsi_pkt_t * rsi_pkt_alloc(rsi_pkt_pool_t *pool_cb)
{
#ifdef ROM_WIRELESS
  return ROMAPI_WL->rsi_pkt_alloc(global_cb_p, pool_cb);
#else
  return api_wl->rsi_pkt_alloc(global_cb_p, pool_cb);
#endif
}

/*==============================================*/
/**
 * @fn         int32_t rsi_pkt_free(rsi_pkt_pool_t *pool_cb, rsi_pkt_t *pkt)
 * @brief      Frees the packet
 * @param[in]  pool_cb, packet pool to which packet needs to be freed 
 * @param[in]  pkt    , packet pointer which needs to be freed
 * @param[out] None
 * @return
 *             <0 - If fails
 *             =0 - If success
 *
 *
 * @section description
 * This function frees the packet
 *
 */

int32_t rsi_pkt_free(rsi_pkt_pool_t *pool_cb,rsi_pkt_t *pkt)
{
#ifdef ROM_WIRELESS
  return ROMAPI_WL->rsi_pkt_free(global_cb_p, pool_cb , pkt);
#else
  return api_wl->rsi_pkt_free(global_cb_p, pool_cb, pkt);
#endif
}

/*==============================================*/
/**
 * @fn         uint32_t rsi_is_pkt_available(rsi_pkt_pool_t *pool_cb)
 * @brief      Returns number of packets available in a pool
 * @param[in]  pool_cb , pool pointer to check available pakcets count 
 * @param[out] None
 * @return 
 *             >=0 Number of packet in pool
 *
 * @section description
 * This function returns number of avaiable packets in pool
 *
 *
 */

uint32_t rsi_is_pkt_available(rsi_pkt_pool_t *pool_cb)
{
#ifdef ROM_WIRELESS
  return ROMAPI_WL->rsi_is_pkt_available(global_cb_p, pool_cb);
#else
  return api_wl->rsi_is_pkt_available(global_cb_p, pool_cb);
#endif
}

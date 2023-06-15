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
#ifndef ROM_WIRELESS
#include "rsi_driver.h"

/*==============================================*/
/**
 * @fn         int32_t  ROM_WL_rsi_pkt_pool_init(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb, uint8_t *buffer, uint32_t total_size, uint32_t pkt_size)

 * @brief      Initilalizes packet pool
 * @param[in]   global_cb_p,pointer to the global control block
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
int32_t ROM_WL_rsi_pkt_pool_init(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb, uint8_t *buffer, uint32_t total_size, uint32_t pkt_size)
{
  uint32_t pool_size;
  uint32_t number_of_pkts;
  uint32_t buffer_index = 1;

  //! Memset pool structure
  memset(pool_cb, 0, sizeof(rsi_pkt_pool_t));

  //! Calculate number of packets can be created with given buffer
  number_of_pkts = (total_size / pkt_size);

  //! Increase number of packet by 1 to not use zeroth index
  number_of_pkts++;

  if((total_size - (number_of_pkts * 4)) < pkt_size)
  {
    //! If atleast one packet accomadation is not possible then return error
    return -1;
  }

  //! Save pool pointer
  pool_cb->pool = (void *)buffer;

  //! Calculate pool array size
  pool_size = (number_of_pkts * 4);

  //! Increase buffer pointer by pool size
  //! Buffer contains pool array then actual buffer
  buffer += pool_size;

  //! Decrease total size by pool size
  total_size -= pool_size;

  //! While total size is available fill pool array
  while(total_size > pkt_size)
  {
    //! Fill buffer pointer to current index
    pool_cb->pool[buffer_index++] = buffer;

    //! Increase available count
    pool_cb->avail++;

    //! Increase pool size
    pool_cb->size++;

    //! Move buffer pointer to point next buffer
    buffer += pkt_size;

    //! Decrease total size by packet size
    total_size -= pkt_size;
  }
  if(RSI_SEMAPHORE_CREATE(&pool_cb->pkt_sem,0) != RSI_ERROR_NONE)
  {
    return RSI_ERROR_SEMAPHORE_CREATE_FAILED;
  }
  RSI_SEMAPHORE_POST(&pool_cb->pkt_sem);
  //! Return success
  return 0;
}

/*==============================================*/
/**
 * @fn          rsi_pkt_t * ROM_WL_rsi_pkt_alloc_non_blocking(rsi_pkt_pool_t *pool_cb)
 * @brief       Allocates packet from packet pool
 * @param[in]   global_cb_p,pointer to the global control block
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

rsi_pkt_t * ROM_WL_rsi_pkt_alloc_non_blocking(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb)
{
  rsi_reg_flags_t flags;
  rsi_pkt_t       *pkt = NULL;
/**  
  //! Wait on wlan semaphore
  if(RSI_SEMAPHORE_WAIT(&pool_cb->pkt_sem,RSI_PKT_ALLOC_RESPONSE_WAIT_TIME) != RSI_ERROR_NONE)
  {
    return NULL;
  }
**/
  //! Disable interrupts
  flags = RSI_CRITICAL_SECTION_ENTRY();

  if(pool_cb->avail == 0)
  {
    //! Retore interrupts
    RSI_CRITICAL_SECTION_EXIT(flags);

    //! If available count is zero return NULL
    return NULL;
  }

  //! If packets are available then return packet pointer from pool
  pkt = (rsi_pkt_t *)(pool_cb->pool[pool_cb->avail]);

  //! Decrease available count 
  pool_cb->avail--;

  //! initialize next with NULL
  pkt->next = NULL;

  //! Retore interrupts
  RSI_CRITICAL_SECTION_EXIT(flags);

  //! Return pakcet pointer 
  return pkt;
}

/*==============================================*/
/**
 * @fn         int32_t ROM_WL_rsi_pkt_free_non_blocking(rsi_pkt_pool_t *pool_cb, rsi_pkt_t *pkt)
 * @brief      Frees the packet
 * @param[in]   global_cb_p,pointer to the global control block
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

int32_t ROM_WL_rsi_pkt_free_non_blocking(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb,rsi_pkt_t *pkt)
{
  rsi_reg_flags_t flags;

  //! Disable interrupts
  flags = RSI_CRITICAL_SECTION_ENTRY();

  if(pool_cb->avail == pool_cb->size)
  {
    //! Retore interrupts
    RSI_CRITICAL_SECTION_EXIT(flags);

    //! If available and size are equal then return an error
    return -1;
  }

  //! Increase pool available count
  pool_cb->avail++;

  //! Fill packet pinter into pool array 
  pool_cb->pool[pool_cb->avail] = pkt;

  //! Retore interrupts
  RSI_CRITICAL_SECTION_EXIT(flags);
/**  
  //! Release mutex lock
  RSI_SEMAPHORE_POST(&pool_cb->pkt_sem);
*/
  return RSI_SUCCESS;
}

/*==============================================*/
/**
 * @fn          rsi_pkt_t * ROM_WL_rsi_pkt_alloc(rsi_pkt_pool_t *pool_cb)
 * @brief       Allocates packet from packet pool
 * @param[in]   global_cb_p,pointer to the global control block
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

rsi_pkt_t * ROM_WL_rsi_pkt_alloc(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb)
{
  rsi_reg_flags_t flags;
  rsi_pkt_t       *pkt = NULL;
  
  //! Wait on wlan semaphore
  if(RSI_SEMAPHORE_WAIT(&pool_cb->pkt_sem,RSI_PKT_ALLOC_RESPONSE_WAIT_TIME) != RSI_ERROR_NONE)
  {
    return NULL;
  }

  //! Disable interrupts
  flags = RSI_CRITICAL_SECTION_ENTRY();

  if(pool_cb->avail == 0)
  {
    //! Retore interrupts
    RSI_CRITICAL_SECTION_EXIT(flags);

    //! If available count is zero return NULL
    return NULL;
  }

  //! If packets are available then return packet pointer from pool
  pkt = (rsi_pkt_t *)(pool_cb->pool[pool_cb->avail]);

  //! Decrease available count 
  pool_cb->avail--;

  //! initialize next with NULL
  pkt->next = NULL;

  //! Retore interrupts
  RSI_CRITICAL_SECTION_EXIT(flags);

  //! Return pakcet pointer 
  return pkt;
}

/*==============================================*/
/**
 * @fn         int32_t ROM_WL_rsi_pkt_free(rsi_pkt_pool_t *pool_cb, rsi_pkt_t *pkt)
 * @brief      Frees the packet
 * @param[in]   global_cb_p,pointer to the global control block
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

int32_t ROM_WL_rsi_pkt_free(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb,rsi_pkt_t *pkt)
{
  rsi_reg_flags_t flags;

  //! Disable interrupts
  flags = RSI_CRITICAL_SECTION_ENTRY();

  if(pool_cb->avail == pool_cb->size)
  {
    //! Retore interrupts
    RSI_CRITICAL_SECTION_EXIT(flags);

    //! If available and size are equal then return an error
    return -1;
  }

  //! Increase pool available count
  pool_cb->avail++;

  //! Fill packet pinter into pool array 
  pool_cb->pool[pool_cb->avail] = pkt;

  //! Retore interrupts
  RSI_CRITICAL_SECTION_EXIT(flags);
  
  //! Release mutex lock
  RSI_SEMAPHORE_POST(&pool_cb->pkt_sem);

  return RSI_SUCCESS;
}

/*==============================================*/
/**
 * @fn         uint32_t ROM_WL_rsi_is_pkt_available(rsi_pkt_pool_t *pool_cb)
 * @brief      Returns number of packets available in a pool
 * @param[in]   global_cb_p,pointer to the global control block
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

uint32_t ROM_WL_rsi_is_pkt_available(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb)
{
  rsi_reg_flags_t flags;
  uint32_t        available_count = 0;

  //! Disable interrupts
  flags = RSI_CRITICAL_SECTION_ENTRY();

  //! Get available packet count
  available_count = pool_cb->avail;

  //! Retore interrupts
  RSI_CRITICAL_SECTION_EXIT(flags);

  //! Return available count
  return available_count;

}
#endif

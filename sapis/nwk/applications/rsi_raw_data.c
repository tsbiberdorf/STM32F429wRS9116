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


/*==============================================*/
/**
 * @fn         rsi_send_raw_data(uint8_t* buffer, uint32_t length)
 * @brief      This API is used to send RAW data to Module 
 * @param[in]  buffer, This is the pointer to the buffer to send 
 * @param[in]  length, This is the length of the buffer to send
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * 
 *
 */

#include "rsi_driver.h"
int32_t rsi_send_raw_data(uint8_t* buffer, uint32_t length)
{
  int32_t  status = RSI_SUCCESS;
  uint8_t  *host_desc;
  rsi_pkt_t *pkt;

  //! If buffer is not valid
  if((buffer == NULL) ||(length == 0))
  {
    //! return packet allocation failure error
    return RSI_ERROR_INVALID_PARAM;
  }

  //! Allocate packet to send data
  pkt = rsi_pkt_alloc(&rsi_driver_cb->wlan_cb->wlan_tx_pool);

  if(pkt == NULL)
  {
    //! return packet allocation failure error
		return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  //! Get host descriptor pointer
  host_desc = pkt->desc;

  //! Memset host descriptor
  memset(host_desc, 0, RSI_HOST_DESC_LENGTH);

  //! Fill host descriptor
  rsi_uint16_to_2bytes(host_desc, (length & 0xFFF));

  //! Fill packet type
  host_desc[1] |= (RSI_WLAN_DATA_Q << 4);

    //! Fill frame type
  host_desc[2] = 0x1;

  //! Copy data to be sent
  memcpy(pkt->data, buffer, length);  

  //! Enqueue packet to WLAN TX queue
  rsi_enqueue_pkt(&rsi_driver_cb->wlan_tx_q, pkt);

  //! Set TX packet pending event
  rsi_set_event(RSI_TX_EVENT);
  
  rsi_semaphore_wait(&rsi_driver_cb_non_rom->send_data_sem, RSI_RAW_DATA_RESPONSE_WAIT_TIME); 
 
  status = rsi_wlan_get_status();
  
  //! Return status
  return status;

}


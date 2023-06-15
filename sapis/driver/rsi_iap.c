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

#include "rsi_driver.h"
#ifdef RSI_WAC_MFI_ENABLE
/*==============================================*/
/**
 * @fn
 * @brief
 * @param[in]   , 
 * @param[out]
 * @return
 *
 *
 * @section description
 * This 
 *
 *
 */

int32_t rsi_iap_init()
{

  rsi_pkt_t       *pkt;

  int32_t status = 0;

  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  if(rsi_check_and_update_cmd_state(COMMON_CMD,IN_USE) == RSI_SUCCESS)
  {
  //! allocate command buffer  from common pool
  pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }
    
  //! send power save command
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_IAP_INIT, pkt);
  
  //! wait on common semaphore
  rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_IAP_RESPONSE_WAIT_TIME);

  //!Changing the common state to allow state
  rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

  }

  else
  {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }
  //! get common command response stattus
  status = rsi_common_get_status();

  //! return status
  return status;
}



/*==============================================*/
/**
 * @fn                int8_t rsi_iap_generate_signature(uint16_t digest_length, int8_t *digest, uint16_t buffer_len, uint8_t *buffer)
 * @brief             This API generates signature for the digest given by feeding it to the IAP chip and this signature is used in apple authentication process
 * @param[in]         digest_length,length of the digest
 * @param[in]         digest, pointer to the digest buffer
 * @param[in]         buffer_len,length of the buffer which is given to copy signature data
 * @param[in]         buffer, pointer to the buffer
 * @param[out]         
 * @return            success = 0  
 *                    else Failure 
 *
 * @section description
 * This API generates signature for the digest given by feeding it to the IAP chip and this signature is used in apple authentication process
 *
 *
 */

int32_t rsi_iap_generate_signature(uint16_t digest_length, int8_t *digest, uint16_t buffer_len, uint8_t *buffer)
{

  rsi_pkt_t       *pkt;
  int32_t          status = 0;
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;
  rsi_mfi_auth_create_request_t *auth_req = NULL;

  if(rsi_check_and_update_cmd_state(COMMON_CMD,IN_USE) == RSI_SUCCESS)
  {

  //! allocate command buffer  from common pool
  pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }
    
  //! Memset the packet
  memset(pkt,0,(digest_length + 4));

  auth_req  =  (rsi_mfi_auth_create_request_t *)pkt->data;

  //! copy Digest data length 
  rsi_uint32_to_4bytes(auth_req->digest_length,digest_length);

  //! Copy Digest 
  memcpy(auth_req->digest, digest, digest_length);

  //! point the buffer to the app buffer
  rsi_common_cb->app_buffer = buffer; 

  rsi_common_cb->app_buffer_length = buffer_len; 

  //! send IAP generate signature  command
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_IAP_GENERATE_SIGATURE, pkt);
  
  //! wait on common semaphore
  rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_IAP_GEN_SIG_RESPONSE_WAIT_TIME);

  //!Changing the common state to allow state
  rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

  }

  else
  {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }
  //! get common command response stattus
  status = rsi_common_get_status();

  //! return status
  return status;

}


/*==============================================*/
/**
 * @fn                     int8_t rsi_iap_get_certificate(uint16_t buffer_len, uint8_t *buffer)                   
 * @brief                  This API is used to get the certificate of the IAP chip
 * @param[in]              buffer_len, length of the buffer 
 * @param[out]             buffer, pointer to the buffer in which certificate is to be copied
 * @return              
 * 
 *
 * @section description
 * This 
 *
 *
 */
int32_t rsi_iap_get_certificate(uint16_t buffer_len, uint8_t *buffer)
{

  rsi_pkt_t       *pkt;

  int32_t status = 0;

  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  if(rsi_check_and_update_cmd_state(COMMON_CMD,IN_USE) == RSI_SUCCESS)
  {
  //! allocate command buffer  from common pool
  pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }
    
  //! point the buffer to the app buffer
  rsi_common_cb->app_buffer = buffer; 

  rsi_common_cb->app_buffer_length = buffer_len; 

  //! send IAP generate signature  command
  status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_IAP_GET_CERTIFICATE, pkt);
  
  //! wait on common semaphore
  rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_IAP_GET_CERT_RESPONSE_WAIT_TIME);

  //!Changing the common state to allow state
  rsi_check_and_update_cmd_state(COMMON_CMD,ALLOW);

  }

  else
  {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }
  //! get common command response stattus
  status = rsi_common_get_status();

  //! return status
  return status;

}
#endif

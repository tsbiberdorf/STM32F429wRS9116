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
#include "rsi_firmware_upgradation.h"

/*==============================================*/
/**
 * @fn          int32_t rsi_fwup(uint8_t type, uint8_t *content, uint16_t length)
 * @brief       This is a helper function for actual APIs
 * @param[in]   type, firmware upgrade chunk type 
 * @param[in]   content, firmware content
 * @param[in]   length, length of the content
 * @param[out]  None 
 * @return      int32_t
 *              0  =  success
 *              3  =  Firmware upgradation completed successfully
 *              <0 =  failure 
 *
 * @section description
 * This is a helper function and called in actual firmware upgradation APIs
 * This API will take care of filling upgradation request and sends it to device
 *
 */
static int32_t rsi_fwup(uint8_t type, uint8_t *content, uint16_t length)
{
  int32_t status = RSI_SUCCESS;
  
  rsi_pkt_t       *pkt;
 
  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_req_fwup_t *fwup = NULL;

  //! Check if length exceeds
  if(length > RSI_MAX_FWUP_CHUNK_SIZE)
  {
    return RSI_ERROR_INVALID_PARAM;
  }

 if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
 { 
 
  //! allocate command buffer  from wlan pool
  pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
  
  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the nwk state to allow 
    rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 
    //! return packet allocation failure error
		return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  fwup =  (rsi_req_fwup_t *)pkt->data;

  //! Fill packet type
  rsi_uint16_to_2bytes(fwup->type, type);

  //! Fill packet length
  rsi_uint16_to_2bytes(fwup->length, length);

  //! Fill packet content
  memcpy(fwup->content, content, length);

  if(rsi_wlan_cb_non_rom->nwk_callbacks.rsi_wireless_fw_upgrade_handler == NULL)
  {
#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
  }
  //! send set FTP Create command 
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FWUP, pkt);

  if(rsi_wlan_cb_non_rom->nwk_callbacks.rsi_wireless_fw_upgrade_handler == NULL)
  {
    //! wait on nwk semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_FWUP_RESPONSE_WAIT_TIME); 

    //! get wlan/network command response status
    status =  rsi_wlan_get_nwk_status();
    //!Changing the nwk state to allow 
    rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 
  }
 }
 else
 {
   //! return nwk command error
   return RSI_ERROR_NWK_CMD_IN_PROGRESS;
 }
  

  //! Return the status if error in sending command occurs
  return status;

}

/*==============================================*/
/**
 * @fn          int32_t rsi_fwup_start(uint8_t *rps_header)
 * @brief       This API is used to send the RPS header content of the firmware file
 * @param[in]   rps_header, pointer to the rps header content
 * @param[out]  None
 * @return      int32_t
 *              0  =  success
 *              <0 =  failure 
 *
 * @section description
 * This API is used to send the RPS header content of the firmware file
 *
 */
int32_t rsi_fwup_start(uint8_t *rps_header)
{
  int32_t status = RSI_SUCCESS;

  status = rsi_fwup(RSI_FWUP_RPS_HEADER, rps_header, RSI_RPS_HEADER_SIZE);

  return status;
}

/*==============================================*/
/**
 * @fn          int32_t rsi_fwup_load(uint8_t *content, uint16_t length)
 * @brief       This API is used to send the RPS file content
 * @param[in]   content, pointer to the firmware file content
 * @param[out]  None
 * @return      int32_t
 *              0  =  success
 *              3  =  Firmware upgradation is completed successfully
 *              <0 =  failure 
 *
 * @section description
 * This API is used to send the firmware file content
 *
 */
int32_t rsi_fwup_load(uint8_t *content, uint16_t length)
{
  int32_t status = RSI_SUCCESS;

  status = rsi_fwup(RSI_FWUP_RPS_CONTENT, content, length);

  return status;

}

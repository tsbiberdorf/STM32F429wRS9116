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
 * @fn          int32_t rsi_ota_firmware_upgradation(uint8_t ip_version, uint8_t *server_ip, uint32_t server_port, uint16_t chunk_number, uint16_t time_out, uint16_t tcp_retry_count);
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

int32_t rsi_ota_firmware_upgradation(uint8_t flags, uint8_t *server_ip, uint32_t server_port, uint16_t chunk_number, uint16_t timeout, 
                                      uint16_t tcp_retry_count, void(*ota_fw_up_response_handler)(uint16_t status, uint16_t chunk_number))
{
  int32_t status = RSI_SUCCESS;
  
  rsi_pkt_t       *pkt;
 
  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_req_ota_fwup_t *otaf_fwup = NULL;
 
 
 if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
 { 
   //! register callback
   if(ota_fw_up_response_handler != NULL)
   {
     //! Register smtp client response notify call back handler
     rsi_wlan_cb_non_rom->nwk_callbacks.rsi_ota_fw_up_response_handler = ota_fw_up_response_handler;
   }
   else
   {
     //!Changing the common state to allow state
     rsi_check_and_update_cmd_state(NWK_CMD,ALLOW);
     //! return invalid command error
     return RSI_ERROR_INVALID_PARAM;
   }

  //! allocate command buffer  from wlan pool
  pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(NWK_CMD,ALLOW);
    //! return packet allocation failure error
		return RSI_ERROR_PKT_ALLOCATION_FAILURE;
   
  }

  otaf_fwup =  (rsi_req_ota_fwup_t *)pkt->data;
  
  //! Check ip version
  if(!(flags & RSI_IPV6)) 
  {
    //! Fill the IP version
    otaf_fwup->ip_version = RSI_IP_VERSION_4;

    //! Set IP address to localhost
    memcpy(otaf_fwup->server_ip_address.ipv4_address, server_ip, RSI_IPV4_ADDRESS_LENGTH);
  }
  else
  {
    //! Fill the IP version
    otaf_fwup->ip_version = RSI_IP_VERSION_6;
    
    //! Set IP address to localhost
    memcpy(otaf_fwup->server_ip_address.ipv6_address, server_ip, RSI_IPV6_ADDRESS_LENGTH);
  }

  //! Fill server port number
  rsi_uint32_to_4bytes(otaf_fwup->server_port, server_port);

  //! Fill chunk number
  rsi_uint16_to_2bytes(otaf_fwup->chunk_number, chunk_number);

  //! Fill timeout
  rsi_uint16_to_2bytes(otaf_fwup->timeout, timeout);
  
  //! Fill tcp retry count
  rsi_uint16_to_2bytes(otaf_fwup->retry_count, tcp_retry_count);


  //! Send OTAF command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_OTA_FWUP, pkt);

 }
 else
 {
   //! return nwk command error
   return RSI_ERROR_NWK_CMD_IN_PROGRESS;
 }
  

  //! Return the status if error in sending command occurs
  return status;

}



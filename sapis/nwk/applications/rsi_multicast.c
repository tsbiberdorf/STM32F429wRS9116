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

/*==============================================*/
/**
 * @fn          int32_t rsi_multicast(uint8_t flags, int8_t *ip_address, uint8_t command_type)
 * @brief       This is a helper function for actual APIs
 * @param[in]   flags,  to select version and security , BIT(0) : 0 - IPv4 , 1 - IPv6
 * @param[in]   ip_address, multicast IP address
 * @param[in]   command_type, type of the command JOIN/LEAVE
 * @param[out]  None
 * @return      int32_t
 *              0  =  success
 *              <0 =  failure 
 *
 * @section description
 * This function is helper function and called in actual multiucast APIs
 * This function will take of filling multicast request and sends it to device
 *
 */

static int32_t rsi_multicast(uint8_t flags, int8_t *ip_address, uint8_t command_type)
{
  int32_t  status = RSI_SUCCESS;

  rsi_pkt_t       *pkt;
  rsi_req_multicast_t *multicast;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

 if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
 { 

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

  multicast =  (rsi_req_multicast_t *)pkt->data;

  //! Fill IP version and IP address
  if(flags & RSI_IPV6)
  {
    //! Fill IPv6 version
    rsi_uint16_to_2bytes(multicast->ip_version, RSI_IP_VERSION_6);

    //! Fill IPv6 address
    memcpy(multicast->multicast_address.ipv6_address, ip_address, RSI_IPV6_ADDRESS_LENGTH);
  }
  else
  {
    //! Fill IPv4 version
    rsi_uint16_to_2bytes(multicast->ip_version, RSI_IP_VERSION_4);

    //! Fill IPv4 address
    memcpy(multicast->multicast_address.ipv4_address, ip_address, RSI_IPV4_ADDRESS_LENGTH);
  }

  //! Fill command type
  rsi_uint16_to_2bytes(multicast->type, command_type);

#ifndef RSI_NWK_SEM_BITMAP
  rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif

  //! send set FTP Create command 
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_MULTICAST, pkt);

  //! wait on nwk semaphore
  rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_MULTICAST_RESPONSE_WAIT_TIME); 
  //! get wlan/network command response status
  status =  rsi_wlan_get_nwk_status();

  //!Changing the nwk state to allow 
  rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 

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
 * @fn          int32_t rsi_multicast_join(uint8_t flags, int8_t *ip_address)
 * @brief       This API is used to join a multicast IP address
 * @param[in]   flags,  to select version and security , BIT(0) : 0 - IPv4 , 1 - IPv6
 * @param[in]   ip_address, multicast IP address to join
 * @param[out]  None
 * @return      int32_t
 *              0  =  success
 *              <0 =  failure 
 *
 * @section description
 * This API is used to join a multicast IP address
 *
 */
int32_t rsi_multicast_join(uint8_t flags, int8_t *ip_address)
{
  int32_t  status = RSI_SUCCESS;

  status = rsi_multicast(flags, ip_address, RSI_MULTICAST_JOIN);

  return status;
}


/*==============================================*/
/**
 * @fn          int32_t rsi_multicast_leave(uint8_t flags, int8_t *ip_address)
 * @brief       This API is used to leave a multicast IP address
 * @param[in]   flags,  to select version and security , BIT(0) : 0 - IPv4 , 1 - IPv6
 * @param[in]   ip_address, multicast IP address to leave
 * @param[out]  None
 * @return      int32_t
 *              0  =  success
 *              <0 =  failure 
 *
 * @section description
 * This API is used to leave a multicast IP address
 *
 */
int32_t rsi_multicast_leave(uint8_t flags, int8_t *ip_address)
{
  int32_t  status = RSI_SUCCESS;

  status = rsi_multicast(flags, ip_address, RSI_MULTICAST_LEAVE);

  return status;
}



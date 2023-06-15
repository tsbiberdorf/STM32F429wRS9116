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
#ifdef RSI_BT_ENABLE
#include "rsi_driver.h" 
#include "rsi_bt.h"
#include "rsi_bt_apis.h"
#include "rsi_bt_config.h"


/*==============================================*/
/**
 * @fn         rsi_bt_pbap_init
 * @brief      sets profile mode
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to set the profile mode.
 */
int32_t rsi_bt_pbap_init(void)
{ 
  rsi_bt_req_profile_mode_t bt_req_pbap_init = {RSI_PBAP_PROFILE_BIT};
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_SET_PROFILE_MODE, &bt_req_pbap_init, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_bt_pbap_connect
 * @brief      requests PBAP connection.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
		  ../bt_ble/rsi_bt_hfp_apis.o
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used to initiate PBAP connection.
 * */
int32_t rsi_bt_pbap_connect(uint8_t *remote_dev_addr)
{
  rsi_bt_req_pbap_connect_t bt_req_pbap_connect = {{0}};
  memcpy (bt_req_pbap_connect.dev_addr, (int8_t *)remote_dev_addr, 6);

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_PBAP_CONNECT, &bt_req_pbap_connect, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_bt_pbap_disconnect
 * @brief      requests PBAP disconnection.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for PBAP disconnection.
 * */
int32_t rsi_bt_pbap_disconnect(uint8_t *remote_dev_addr)
{
  rsi_bt_req_pbap_disconnect_t bt_req_pbap_disconnect = {{0}};
  memcpy (bt_req_pbap_disconnect.dev_addr, (int8_t *)remote_dev_addr, 6);

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_PBAP_DISCONNECT, &bt_req_pbap_disconnect, NULL);
}
/*==============================================*/
/**
 * @fn         rsi_bt_pbap_contacts
 * @brief      requests PBAP contacts.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for PBAP disconnection.
 * */
int32_t rsi_bt_pbap_contacts(uint8_t *remote_dev_addr,uint8_t folder_id, uint8_t nbr_contacts,uint8_t start_ix)
{
  rsi_bt_req_pbap_contacts_t bt_req_pbap_contacts = {{0}};
  bt_req_pbap_contacts.folder_id = folder_id;
  bt_req_pbap_contacts.start_ix = start_ix;
  bt_req_pbap_contacts.nbr_contacts = nbr_contacts;

  memcpy (bt_req_pbap_contacts.dev_addr, (int8_t *)remote_dev_addr, 6);

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_PBAP_CONTACTS, &bt_req_pbap_contacts, NULL);
}



#endif

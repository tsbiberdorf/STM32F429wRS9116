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
#ifdef RSI_BLE_ENABLE	
#include "rsi_driver.h"
#include "rsi_ble.h"
#include "rsi_ble_config.h"
#include "rsi_utils.h"

/*==============================================*/
/**
 * @fn         rsi_ble_set_random_address
 * @brief      request the local device to set a random address
 * @param[in]  none
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to set a random address
 * */
int32_t rsi_ble_set_random_address(void)
{
  rsi_ble_req_rand_t ble_rand = {{0}};

  //! Send set random address command 
  return rsi_bt_driver_send_cmd(RSI_BLE_SET_RANDOM_ADDRESS, &ble_rand, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_set_random_address
 * @brief      request the local device to set a random address
 * @param[in]  none
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to set a random address
 * */
int32_t rsi_ble_set_random_address_with_value(uint8_t *random_addr)
{
  rsi_ble_req_rand_t ble_rand = {{0}};
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(ble_rand.rand_addr, random_addr);
#else 
  memcpy ((int8_t *)&(ble_rand.rand_addr), (int8_t *)random_addr, 6);
#endif  

  //! Send set random address command 
  return rsi_bt_driver_send_cmd(RSI_BLE_SET_RANDOM_ADDRESS, &ble_rand, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_start_advertising
 * @brief      request the local device to start advertising
 * @param[in]  none
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to start advertising
 * */
int32_t rsi_ble_start_advertising(void)
{
  rsi_ble_req_adv_t ble_adv = {0};

  ble_adv.status = RSI_BLE_START_ADV;
  
  ble_adv.adv_type            = RSI_BLE_ADV_TYPE;
  ble_adv.filter_type         = RSI_BLE_ADV_FILTER_TYPE;
  ble_adv.direct_addr_type    = RSI_BLE_ADV_DIR_ADDR_TYPE;
  rsi_ascii_dev_address_to_6bytes_rev(ble_adv.direct_addr, (int8_t *)RSI_BLE_ADV_DIR_ADDR);
  if(ble_adv.adv_int_min == 0)
  {
    ble_adv.adv_int_min       = RSI_BLE_ADV_INT_MIN;
  }
  if(ble_adv.adv_int_max == 0)
  {
    ble_adv.adv_int_max       = RSI_BLE_ADV_INT_MAX;       
  }     
  ble_adv.own_addr_type       = LE_PUBLIC_ADDRESS;
  ble_adv.adv_channel_map     = RSI_BLE_ADV_CHANNEL_MAP;
  //! Send start advertise command 
  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_ADV, &ble_adv, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_start_advertising_with_values
 * @brief      request the local device to start advertising
 * @param[in]  rsi_ble_adv is the structure pointer hold the advertising values
 *             status	           advertising status - disable/enable
 *             adv_type	         advertising type used during advertising
 *             filter_type	     advertising filter type
 *             direct_addr_type	 address type of the device to which directed advertising has to be done
 *             direct_addr	     address of the device to which directed advertising has to be done
 *             adv_int_min	     advertising interval min
 *             adv_int_max	     advertising interval max
 *             own_addr_type	   address of the local device
 *             adv_channel_map	advertising channel map
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to start advertising
 * */
int32_t rsi_ble_start_advertising_with_values(void *rsi_ble_adv)
{
  rsi_ble_req_adv_t ble_adv = {0};
  if(rsi_ble_adv != NULL)
  {
    memcpy(&ble_adv, rsi_ble_adv, sizeof(rsi_ble_req_adv_t));
  }

  ble_adv.status = RSI_BLE_START_ADV;
  
  //! Send start advertise command 
  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_ADV, &ble_adv, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_encrypt
 * @brief      request the local device to encrypt
 * @param[in]  key , data  
 * @param[out] resp
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to Encrypt
 * */
int32_t rsi_ble_encrypt(uint8_t *key,uint8_t *data,uint8_t *resp)
{
   rsi_ble_encrypt_t  ble_en;
	 memcpy(ble_en.key,key,16);
	 memcpy(ble_en.data,data,16);
  
  //! Send Encrypt command
  return rsi_bt_driver_send_cmd(RSI_BLE_ENCRYPT, &ble_en,resp);
}

/*==============================================*/
/**
 * @fn         rsi_ble_stop_advertising
 * @brief      request the local device to stop advertising
 * @param[in]  none
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to stop advertising
 * */
int32_t rsi_ble_stop_advertising(void)
{
  rsi_ble_req_adv_t ble_adv = {0};
  
  ble_adv.status = RSI_BLE_STOP_ADV;

  //! Send stop advertise command
  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_ADV, &ble_adv, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_set_advertise_data
 * @brief      request the local device to set the advertising data
 * @param[in]  data , data to be sent is filled in this
 * @param[in]  data_len , length of data to be sent
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to set advertising information
 * */
int32_t rsi_ble_set_advertise_data(uint8_t *data, uint16_t data_len)
{
  rsi_ble_req_adv_data_t ble_adv_data = {0};
  
  ble_adv_data.data_len = RSI_MIN (data_len, sizeof (ble_adv_data.adv_data));
  memcpy (ble_adv_data.adv_data, data, ble_adv_data.data_len);

  //! Send stop advertise command
  return rsi_bt_driver_send_cmd(RSI_BLE_SET_ADVERTISE_DATA, &ble_adv_data, NULL);

}

/*========================================================*/
/**
 * @fn			rsi_ble_set_scan_response_data
 * @brief		request the local devuce to set the scanresponse data
 * @param[in]  data , data to be sent is filled in this
 * @param[in]  data_len , length of data to be sent
 * @ @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to set scan response data
 * */
int32_t rsi_ble_set_scan_response_data(uint8_t *data, uint16_t data_len)
{
  rsi_ble_req_scanrsp_data_t ble_scanrsp_data = {0}; 
  ble_scanrsp_data.data_len = RSI_MIN (data_len, sizeof (ble_scanrsp_data.scanrsp_data));
  memcpy (ble_scanrsp_data.scanrsp_data, data, ble_scanrsp_data.data_len);

  return rsi_bt_driver_send_cmd(RSI_BLE_SET_SCAN_RESPONSE_DATA, &ble_scanrsp_data, NULL);

}

/*==============================================*/
/**
 * @fn         rsi_ble_start_scanning
 * @brief      request the local device to start scanning
 * @param[in]  none
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to start scanning
 * */
int32_t rsi_ble_start_scanning(void)
{
  rsi_ble_req_scan_t ble_scan = {0};
  
  ble_scan.status = RSI_BLE_START_SCAN;

  ble_scan.scan_type         = RSI_BLE_SCAN_TYPE;
  ble_scan.filter_type       = RSI_BLE_SCAN_FILTER_TYPE;
  ble_scan.scan_int          = LE_SCAN_INTERVAL;
  ble_scan.scan_win          = LE_SCAN_WINDOW;
  ble_scan.own_addr_type     = LE_PUBLIC_ADDRESS;

  //! Send start scanning command
  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_SCAN, &ble_scan, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_start_scanning_with_values
 * @brief      request the local device to start scanning
 * @param[in]  BLE scan parameters structure
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to start scanning
 * */
int32_t rsi_ble_start_scanning_with_values(void *rsi_ble_scan_params)
{
  rsi_ble_req_scan_t ble_scan = {0};

  if (rsi_ble_scan_params != NULL)
  {
    memcpy(&ble_scan, (rsi_ble_req_scan_t *)rsi_ble_scan_params, sizeof(rsi_ble_req_scan_t));
  } 
 
  ble_scan.status = RSI_BLE_START_SCAN;

  //! Send start scanning command
  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_SCAN, &ble_scan, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_stop_scanning
 * @brief      request the local device to stop scanning
 * @param[in]  none
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function requests the local device to stop scanning
 * */
int32_t rsi_ble_stop_scanning(void)
{
  rsi_ble_req_scan_t ble_scan = {0};
  
  ble_scan.status = RSI_BLE_STOP_SCAN;

  //! Send stop scanning command
  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_SCAN, &ble_scan, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_connect_with_params
 * @brief      request to initiate the connection with the remote device
 * @param[in]  addr_type, address type of the remote device
 * @param[in]  remote_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to initiate the connection request
 * */
int32_t rsi_ble_connect_with_params(uint8_t remote_dev_addr_type, int8_t *remote_dev_addr, uint16_t scan_interval, uint16_t scan_window, uint16_t conn_interval_max, uint16_t conn_interval_min, uint16_t conn_latency, uint16_t supervision_tout)
{
  rsi_ble_req_conn_t ble_conn = {0};
  
  ble_conn.dev_addr_type = remote_dev_addr_type;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(ble_conn.dev_addr, remote_dev_addr);
#else 
  memcpy (ble_conn.dev_addr, remote_dev_addr, 6);
#endif  
  ble_conn.le_scan_interval  = scan_interval;
  ble_conn.le_scan_window    = scan_window;
  ble_conn.conn_interval_min = conn_interval_max;
  ble_conn.conn_interval_max = conn_interval_max;
  ble_conn.conn_latency      = conn_latency;
  ble_conn.supervision_tout  = supervision_tout;

  //! Send connect command
  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_CONN, &ble_conn, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_connect
 * @brief      request to initiate the connection with the remote device
 * @param[in]  addr_type, address type of the remote device
 * @param[in]  remote_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to initiate the connection request
 * */
int32_t rsi_ble_connect(uint8_t remote_dev_addr_type, int8_t *remote_dev_addr)
{
  rsi_ble_req_conn_t ble_conn = {0};
  
  ble_conn.dev_addr_type = remote_dev_addr_type;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(ble_conn.dev_addr, remote_dev_addr);
#else 
  memcpy (ble_conn.dev_addr, remote_dev_addr, 6);
#endif  
  ble_conn.le_scan_interval  = LE_SCAN_INTERVAL;
  ble_conn.le_scan_window    = LE_SCAN_WINDOW;
  ble_conn.conn_interval_min = CONNECTION_INTERVAL_MIN;
  ble_conn.conn_interval_max = CONNECTION_INTERVAL_MAX;
  ble_conn.conn_latency      = CONNECTION_LATENCY;
  ble_conn.supervision_tout  = SUPERVISION_TIMEOUT;


  //! Send connect command
  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_CONN, &ble_conn, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_connect_cancel
 * @brief      request the connection cancel with the remote device
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to cancel the connection request
 * */
int32_t rsi_ble_connect_cancel(int8_t *remote_dev_address)
{
  rsi_ble_req_disconnect_t ble_disconnect = {{0}};
  
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(ble_disconnect.dev_addr, remote_dev_address);
#else 
  memcpy (ble_disconnect.dev_addr, remote_dev_address, 6);
#endif  

  //! Send connect cancel command
  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_DISCONNECT, &ble_disconnect, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_disconnect
 * @brief      request the disconnect with the remote device
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to disconnect the physical connection
 * */
int32_t rsi_ble_disconnect(int8_t *remote_dev_address)
{
  rsi_ble_req_disconnect_t ble_disconnect = {{0}};
  
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(ble_disconnect.dev_addr, remote_dev_address);
#else 
  memcpy (ble_disconnect.dev_addr, remote_dev_address, 6);
#endif
  //! Send disconnect command
  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_DISCONNECT, &ble_disconnect, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_get_device_state
 * @brief      request the local device state
 * @param[out] resp, response buffer to hold the response of this API
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to know the local device state
 */
int32_t rsi_ble_get_device_state(uint8_t *resp)
{
  return rsi_bt_driver_send_cmd(RSI_BLE_GET_DEV_STATE, NULL, resp);
}

/*==============================================*/
/**
 * @fn         rsi_ble_set_smp_pairing_cap_data
 * @brief      setting smp cap data to local device
 * @param[in]  io_capability, device input and output capability
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to start the SMP pairing process
 */
int32_t rsi_ble_set_smp_pairing_cap_data (rsi_ble_set_smp_pairing_capabilty_data_t *smp_pair_cap_data)
{
  return rsi_bt_driver_send_cmd(RSI_BLE_SET_SMP_PAIRING_CAPABILITY_DATA, smp_pair_cap_data, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_set_local_irk_value
 * @brief      setting irk value to the local device
 * @param[in]  l_irk  Pointer to local_irk
 * * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to set the local irk value */
int32_t rsi_ble_set_local_irk_value (uint8_t *l_irk)
{
  rsi_ble_set_local_irk_t local_device_irk = {0};

  memcpy (local_device_irk.irk, l_irk, 16);
  
  return rsi_bt_driver_send_cmd(RSI_BLE_CMD_SET_LOCAL_IRK, &local_device_irk, NULL);
}



/*==============================================*/
/**
 * @fn         rsi_ble_conn_param_resp
 * @brief      reply LE conn param resp for remote device connection params request
 * @param[in]  remote_dev_address, remote device address
 * @param[in]  status, accept or reject the remote conn param request
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to request the connection params update with the remote device
 * */
int32_t rsi_ble_conn_param_resp(uint8_t *remote_dev_address, uint8_t status)
{
  rsi_ble_cmd_conn_param_resp_t conn_param_resp_cmd = {0};

#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(conn_param_resp_cmd.dev_addr, remote_dev_address);
#else 
  memcpy  (conn_param_resp_cmd.dev_addr, remote_dev_address, 6);
#endif  
  conn_param_resp_cmd.status = status;
  
  //! Send connect cancel command
  return rsi_bt_driver_send_cmd(RSI_BLE_CONN_PARAM_RESP_CMD, &conn_param_resp_cmd, NULL);
}



/*==============================================*/
/**
 * @fn         rsi_ble_smp_pair_request
 * @brief      request the smp pairing process with remote device
 * @param[in]  remote_dev_address,  remote bd address in string format
 * @param[in]  io_capability, device input and output capability
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to start the SMP pairing process
 */
int32_t rsi_ble_smp_pair_request (uint8_t *remote_dev_address, uint8_t io_capability, uint8_t mitm_req)
{
  rsi_ble_req_smp_pair_t  smp_pair_req = {{0}};

#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(smp_pair_req.dev_addr, remote_dev_address);
#else 
  memcpy (smp_pair_req.dev_addr, (int8_t *)remote_dev_address, 6);
#endif  
  smp_pair_req.io_capability = io_capability;
  smp_pair_req.mitm_req = mitm_req; 

  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_SMP_PAIR, &smp_pair_req, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_ltk_req_reply
 * @brief      request the smp pairing process with remote device
 * @param[in]  remote_dev_address,  remote bd address in string format
 * @param[in]  io_capability, device input and output capability
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to start the SMP pairing process
 */
int32_t rsi_ble_ltk_req_reply (uint8_t *remote_dev_address, uint8_t reply_type, uint8_t *ltk)
{
  rsi_ble_set_le_ltkreqreply_t  le_ltk_req_reply = {{0}};
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(le_ltk_req_reply.dev_addr, remote_dev_address);
#else 
  memcpy (le_ltk_req_reply.dev_addr, (int8_t *)remote_dev_address, 6);
#endif 
  le_ltk_req_reply.replytype = reply_type;
  if(ltk != NULL)
  {
    memcpy(le_ltk_req_reply.localltk, ltk, 16);
  }

  return rsi_bt_driver_send_cmd(RSI_BLE_LE_LTK_REQ_REPLY, &le_ltk_req_reply, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_smp_pair_response
 * @brief      request to send smp pairing response in pairing process with remote device
 * @param[in]  remote_dev_address,  remote bd address in string format
 * @param[in]  io_capability, device input and output capability
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to respond the SMP pairing request
 */
int32_t rsi_ble_smp_pair_response (uint8_t *remote_dev_address, uint8_t io_capability, uint8_t mitm_req)
{
  rsi_ble_smp_response_t  smp_pair_resp = {{0}};

#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(smp_pair_resp.dev_addr, remote_dev_address);
#else 
  memcpy (smp_pair_resp.dev_addr, (int8_t *)remote_dev_address, 6);
#endif  
  smp_pair_resp.io_capability = io_capability;
  smp_pair_resp.mitm_req = mitm_req;

  return rsi_bt_driver_send_cmd(RSI_BLE_SMP_PAIR_RESPONSE, &smp_pair_resp, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_smp_passkey
 * @brief      request to send smp passkey in SMP pairing process with remote device
 * @param[in]  remote_dev_address,  remote bd address in string format
 * @param[in]  passkey, passkey to authenticate remote device
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to respond the SMP passkey request
 */
int32_t rsi_ble_smp_passkey (uint8_t *remote_dev_address, uint32_t passkey)
{
  rsi_ble_smp_passkey_t  smp_passkey = {{0}};
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(smp_passkey.dev_addr, remote_dev_address);
#else 
  memcpy (smp_passkey.dev_addr, (int8_t *)remote_dev_address, 6);
#endif  
  smp_passkey.passkey = passkey;
  return rsi_bt_driver_send_cmd(RSI_BLE_SMP_PASSKEY, &smp_passkey, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_get_le_ping_timeout
 * @brief      request to get the le ping time out from controller.
 * @param[in]  remote_dev_address,  remote bd address in string format
 * @param[out] timeout, timeout value.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to get the timeout value of the le ping.
 */
int32_t rsi_ble_get_le_ping_timeout (uint8_t *remote_dev_address, uint16_t *time_out)
{
  uint16_t status;
  rsi_ble_get_le_ping_timeout_t      leping_cmd;
  rsi_ble_rsp_get_le_ping_timeout_t  le_ping_rsp;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(leping_cmd.dev_addr, remote_dev_address);
#else 
  memcpy (leping_cmd.dev_addr, (int8_t *)remote_dev_address, 6);
#endif  
  status = rsi_bt_driver_send_cmd(RSI_BLE_GET_LE_PING, &leping_cmd, &le_ping_rsp);
  if (status == 0) {
    *time_out = le_ping_rsp.time_out;
  }

  return status;
}

/*==============================================*/
/**
 * @fn         rsi_ble_set_le_ping_timeout
 * @brief      request to get the le ping time out from controller.
 * @param[in]  remote_dev_address,  remote bd address in string format
 * @param[out] timeout, timeout value.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to get the timeout value of the le ping.
 */
int32_t rsi_ble_set_le_ping_timeout (uint8_t *remote_dev_address, uint16_t time_out)
{
  rsi_ble_set_le_ping_timeout_t  leping_cmd;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(leping_cmd.dev_addr, remote_dev_address);
#else 
  memcpy (leping_cmd.dev_addr, (int8_t *)remote_dev_address, 6);
#endif  
  leping_cmd.time_out = time_out;

  return rsi_bt_driver_send_cmd(RSI_BLE_SET_LE_PING, &leping_cmd, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_clear_whitelist
 * @brief      clears all the BD address present in white list
 * @param[in]  none 
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to clear the white list completely.
 */

int32_t rsi_ble_clear_whitelist(void)
{
	rsi_ble_white_list_t le_whitelist = {0};
	le_whitelist.addordeltowhitlist=CLEAR_WHITELIST;

	return rsi_bt_driver_send_cmd(RSI_BLE_LE_WHITE_LIST,&le_whitelist , NULL); 
}

/*==============================================*/
/**
 * @fn         rsi_ble_addto_whitelist
 * @brief      Adds perticular BD address to white list
 * @param[in]  address, address type 
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to add bd address to white list.
 */

int32_t rsi_ble_addto_whitelist( int8_t *dev_address,uint8_t dev_addr_type)
{
	rsi_ble_white_list_t le_whitelist = {0};
	le_whitelist.addordeltowhitlist= ADD_DEVICE_TO_WHITELIST;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(le_whitelist.dev_addr, dev_address);
#else 
	memcpy (le_whitelist.dev_addr, dev_address, 6);
#endif  
	le_whitelist.bdaddressType = dev_addr_type;

	return rsi_bt_driver_send_cmd(RSI_BLE_LE_WHITE_LIST,&le_whitelist , NULL); 
}
/*==============================================*/
/**
 * @fn         rsi_ble_deletefrom_whitelist
 * @brief      Deletes perticular BD address from white list
 * @param[in]  address, address type 
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to delete bd address from white list.
 */

int32_t rsi_ble_deletefrom_whitelist(int8_t *dev_address,uint8_t dev_addr_type)
{
	rsi_ble_white_list_t le_whitelist = {0};
	le_whitelist.addordeltowhitlist=DELETE_DEVICE_FROM_WHITELIST;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(le_whitelist.dev_addr, dev_address);
#else  
	memcpy (le_whitelist.dev_addr, dev_address, 6);
#endif
	le_whitelist.bdaddressType = dev_addr_type;

	return rsi_bt_driver_send_cmd(RSI_BLE_LE_WHITE_LIST,&le_whitelist , NULL); 
}

/*==============================================*/
/**
 * @fn         rsi_ble_add_dev_to_resolvlist
 * @brief      request to add device to resolvlist.
 * @param[in]  remote_dev_address_type, remote_dev_address, peer_irk, local_irk,remote bd address in string format
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 *
 * @section description
 * This function is used to add or delete device to resolvlist.
 *
 */
int32_t rsi_ble_resolvlist (uint8_t process_type,
                            uint8_t remote_dev_addr_type,
                            uint8_t *remote_dev_address,
                            uint8_t *peer_irk,
                            uint8_t *local_irk)
{
  rsi_ble_resolvlist_t  resolvlist;

  resolvlist.process_type         = process_type;
  resolvlist.remote_dev_addr_type = remote_dev_addr_type;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(resolvlist.remote_dev_addr, remote_dev_address);
#else 
  memcpy (resolvlist.remote_dev_addr, (int8_t *)remote_dev_address, 6);
#endif
  memcpy(resolvlist.peer_irk,peer_irk,16);
  memcpy(resolvlist.local_irk,local_irk,16);
  return rsi_bt_driver_send_cmd(RSI_BLE_PROCESS_RESOLV_LIST, &resolvlist, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_get_resolving_list_size
 * @brief      request to get resolving list size.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 */
int32_t rsi_ble_get_resolving_list_size (uint8_t *resp)
{

    return rsi_bt_driver_send_cmd(RSI_BLE_GET_RESOLVING_LIST_SIZE, NULL , resp );
}

/*==============================================*/
/**
 * @fn         rsi_ble_set_addr_resolution_enable
 * @brief      request to enable address resolution, and to set resolvable private address timeout
 * @param[in]  resolution enable and resolvable private address timeout
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 */
int32_t rsi_ble_set_addr_resolution_enable (uint8_t enable,uint16_t tout)
{
  rsi_ble_set_addr_resolution_enable_t addr_resolution_enable;

  addr_resolution_enable.enable = enable;
  addr_resolution_enable.tout   = tout;

  return rsi_bt_driver_send_cmd(RSI_BLE_SET_ADDRESS_RESOLUTION_ENABLE, &addr_resolution_enable, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_set_privacy_mode
 * @brief      request to set privacy mode for particular device 
 * @param[in]  peer address type, peer address ,privacy mode
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 */
int32_t rsi_ble_set_privacy_mode (uint8_t remote_dev_addr_type,
                            uint8_t *remote_dev_address,
                            uint8_t privacy_mode)
{
  rsi_ble_set_privacy_mode_t  set_privacy_mode;
  set_privacy_mode.remote_dev_addr_type = remote_dev_addr_type;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(set_privacy_mode.remote_dev_addr, remote_dev_address);
#else 
  memcpy (set_privacy_mode.remote_dev_addr, (int8_t *)remote_dev_address, 6);
#endif
  set_privacy_mode.privacy_mode = privacy_mode;
  return rsi_bt_driver_send_cmd(RSI_BLE_SET_PRIVACY_MODE, &set_privacy_mode, NULL);
}
/*==============================================*/
/**
 * @fn         rsi_ble_readphy
 * @brief      request the connection cancel with the remote device
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to cancel the connection request
 * */
int32_t rsi_ble_readphy(int8_t *remote_dev_address, rsi_ble_resp_read_phy_t *resp)
{
  rsi_ble_req_read_phy_t ble_read_phy = {{0}};
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(ble_read_phy.dev_addr, remote_dev_address);
#else  
  memcpy (ble_read_phy.dev_addr, remote_dev_address, 6);
#endif

  //! Send connect cancel command
  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_READ_PHY, &ble_read_phy, resp);
}

/*==============================================*/
/**
 * @fn         rsi_ble_setphy
 * @brief      request the connection cancel with the remote device
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to cancel the connection request
 * */
int32_t rsi_ble_setphy(int8_t *remote_dev_address, uint8_t tx_phy, uint8_t rx_phy, uint16_t coded_phy)
{
  rsi_ble_set_phy_t set_phy = {{0}};
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(set_phy.dev_addr, remote_dev_address);
#else 
  memcpy  (set_phy.dev_addr, remote_dev_address, 6);
#endif
  set_phy.all_phy = ALL_PHYS;
  set_phy.tx_phy  = tx_phy;
  set_phy.rx_phy  = rx_phy;
  set_phy.phy_options  = coded_phy;


  //! Send connect cancel command
  return rsi_bt_driver_send_cmd(RSI_BLE_REQ_SET_PHY, &set_phy, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_conn_params_update
 * @brief      request the connection params update with the remote device
 * @param[in]  remote_dev_address, remote device address
 * @param[in]  min_int, minimum connenction interval
 * @param[in]  max_int, maximum connenction interval
 * @param[in]  latency, connenction latency
 * @param[in]  timeout, Supervision Timeout
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to request the connection params update with the remote device
 * */
int32_t rsi_ble_conn_params_update(uint8_t *remote_dev_address, uint16_t min_int, uint16_t max_int, uint16_t latency, uint16_t timeout)
{
  rsi_ble_cmd_conn_params_update_t conn_params_update_cmd = {0};
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(conn_params_update_cmd.dev_addr, remote_dev_address);
#else 
  memcpy  (conn_params_update_cmd.dev_addr, remote_dev_address, 6);
#endif
  conn_params_update_cmd.min_interval = min_int;
  conn_params_update_cmd.max_interval = max_int;
  conn_params_update_cmd.latency = latency;
  conn_params_update_cmd.timeout = timeout;
  
  //! Send connect cancel command
  return rsi_bt_driver_send_cmd(RSI_BLE_CMD_CONN_PARAMS_UPDATE, &conn_params_update_cmd, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_lesetdatalen
 * @brief      To set the required data length
 * * @param[in]  remote_dev_address,tx 0ctets,tx time  
  * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to set txoctets and tx time.
 */
int32_t rsi_ble_set_data_len (uint8_t *remote_dev_address, uint16_t tx_octets ,uint16_t tx_time )
{
  rsi_ble_setdatalength_t  lesetdatalen;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(lesetdatalen.dev_addr, remote_dev_address);
#else 
  memcpy (lesetdatalen.dev_addr, (int8_t *)remote_dev_address, 6);
#endif
  lesetdatalen.txoctets = tx_octets;
  lesetdatalen.txtime =tx_time; 

  return rsi_bt_driver_send_cmd(RSI_BLE_SET_DATA_LEN, &lesetdatalen, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_read_max_data_len
 * @brief      reads the maxtxoctets,maxtx_time,maxrx_octets,maxrx_time
  * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to read the max data length used in controller.
 */
int32_t rsi_ble_read_max_data_len (rsi_ble_read_max_data_length_t *blereaddatalen)
{
    return rsi_bt_driver_send_cmd(RSI_BLE_READ_MAX_DATA_LEN,NULL, blereaddatalen);
}

/*==============================================*/
/**
 * @fn         rsi_ble_rx_test_mode
 * @brief      initiate the ble rx test mode in controller.
  * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to start the le rx test mode in controller.
 */
int32_t rsi_ble_rx_test_mode (uint8_t rx_channel, uint8_t phy, uint8_t modulation)
{
    rsi_ble_rx_test_mode_t rx_test_mode;

    rx_test_mode.rx_channel = rx_channel;
    rx_test_mode.phy        = phy;
    rx_test_mode.modulation = modulation;

    return rsi_bt_driver_send_cmd(RSI_BLE_RX_TEST_MODE, &rx_test_mode, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_tx_test_mode
 * @brief      initiate the ble tx test mode in controller.
  * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to start the le tx test mode in controller.
 */
int32_t rsi_ble_tx_test_mode (uint8_t tx_channel, uint8_t phy, uint8_t tx_len, uint8_t mode)
{
    rsi_ble_tx_test_mode_t tx_test_mode;

    tx_test_mode.tx_channel   = tx_channel;
    tx_test_mode.phy          = phy;
    tx_test_mode.tx_len       = tx_len;
    tx_test_mode.tx_data_mode = mode;

    return rsi_bt_driver_send_cmd(RSI_BLE_TX_TEST_MODE, &tx_test_mode, NULL);
}

/**
 * @fn         rsi_ble_end_test_mode
 * @brief      stop the ble tx / rx test mode in controller.
  * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to stop the le tx / rx test mode in controller.
 */
int32_t rsi_ble_end_test_mode (uint16_t *num_of_pkts)
{
    return rsi_bt_driver_send_cmd(RSI_BLE_END_TEST_MODE, NULL, num_of_pkts);
}

#if 0 
/*==============================================*/
/**
 * @fn         rsi_ant_send_cmd
 * @brief      initiate the ant cmd to the stack
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to start the ble transmit per mode in controller.
 */
int32_t rsi_ant_send_cmd (void *ant_cmd, void *ant_cmd_resp)
{
    return rsi_ant_driver_send_cmd(RSI_ANT_CMD, ant_cmd, ant_cmd_resp);
}
#endif

/*==============================================*/
/**
 * @fn         rsi_ble_per_transmit
 * @brief      initiate the ble transmit per mode in the controller.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to start the ble transmit per mode in controller.
 */
int32_t rsi_ble_per_transmit (struct rsi_ble_per_transmit_s *rsi_ble_per_tx)
{
    return rsi_bt_driver_send_cmd(RSI_BT_REQ_PER_CMD, rsi_ble_per_tx, NULL);
}

/**
 * @fn         rsi_ble_per_receive
 * @brief      initiate the ble receive per mode in the controller.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to start the ble receive per mode in controller.
 */
int32_t rsi_ble_per_receive (struct rsi_ble_per_receive_s *rsi_ble_per_rx)
{
    return rsi_bt_driver_send_cmd(RSI_BT_REQ_PER_CMD, rsi_ble_per_rx, NULL);
}

/**
 * @fn         rsi_ble_vendor_rf_type
 * @brief      This api gives vendor specific command to the controller to set rf type.
 *             RF mode will be decide based on the TXPOWER index. 
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to issue vendor specific command for setting rf type in controller on given inputs.
 * 
 */
int32_t rsi_ble_vendor_rf_type (uint8_t ble_power_index)
{
    rsi_ble_vendor_rf_type_t  ble_vendor_rf_type;
    
    ble_vendor_rf_type.opcode[0] = (BLE_VENDOR_RF_TYPE_CMD_OPCODE & 0xFF);
    ble_vendor_rf_type.opcode[1] = ((BLE_VENDOR_RF_TYPE_CMD_OPCODE >> 8) & 0xFF);
    ble_vendor_rf_type.ble_power_index = ble_power_index;
  
    return rsi_bt_driver_send_cmd(RSI_BT_VENDOR_SPECIFIC, &ble_vendor_rf_type, NULL);
}

/**
 * @fn         rsi_ble_white_list_using_adv_data
 * @brief      This api gives vendor specific command to the controller to set BLE advertise report filter 
 *             based on the ADV packet payload.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to issue vendor specific command for setting the ADV report filter in the controller based on the ADV payload.
 */
int32_t rsi_ble_white_list_using_adv_data (uint8_t enable, uint8_t data_compare_index, uint8_t len_for_compare_data, uint8_t *payload)
{
  rsi_ble_req_whitelist_using_payload_t  whitelist_using_payload = {{0}};

  whitelist_using_payload.opcode[0] = (BLE_VENDOR_WHITELIST_USING_ADV_DATA_PAYLOAD & 0xFF);
  whitelist_using_payload.opcode[1] = ((BLE_VENDOR_WHITELIST_USING_ADV_DATA_PAYLOAD >> 8) & 0xFF);

  whitelist_using_payload.enable = enable;
  whitelist_using_payload.total_len = 31;
  whitelist_using_payload.data_compare_index = data_compare_index;
  whitelist_using_payload.len_for_compare_data = len_for_compare_data;

  memcpy (whitelist_using_payload.adv_data_payload, payload, 31);

  return rsi_bt_driver_send_cmd(RSI_BT_VENDOR_SPECIFIC, &whitelist_using_payload, NULL);
}


/*==============================================*/
/**
 * @fn         BT_LE_ADPacketExtract
 * @brief      This function used to extract device name in advertise report data.
 * @param[in]  pbuf advertise data packet buffer pointer, buf_len buffer length
 * @return     none.
 * @section description
 * This function is used to extract the advertiseed packet.
 */
void BT_LE_ADPacketExtract (uint8_t *remote_name,uint8_t *pbuf,uint8_t buf_len)
{
  uint8_t   adv_len;
  uint8_t   adv_type;
  uint8_t   ix;
  uint8_t   name_len;

  ix = 0;

  while (ix < buf_len) {
    adv_len  = pbuf[ix];
    ix     += sizeof (uint8_t);
    adv_type = pbuf[ix];

    switch (adv_type) {
      case 0x08:
      case 0x09:
        {
          name_len =adv_len - 1;
          memcpy (remote_name, &pbuf [ix + 1], name_len);
          remote_name[name_len] = 0;
          return;
        }
		//! no break
      default:
        break;
    }

    ix += adv_len;
  }
}

/*==============================================*/
/**
 * @fn         rsi_ble_update_directed_address
 * @brief      Update the advertising directed address.
 * @param[in]  remote_dev_addr
 * @return     none
 * @section description
 * This function update the direct address with remote device address
 * */

void rsi_ble_update_directed_address(uint8_t *remote_dev_addr)
{
  rsi_bt_cb_t *le_cb = rsi_driver_cb->ble_cb;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(le_cb->directed_addr, remote_dev_addr);
#else 
  memcpy (le_cb->directed_addr,(int8_t *)remote_dev_addr, 6);
#endif
}

/*==============================================*/
/**
 * @fn         rsi_ble_start_encryption
 * @brief      start the encription process with remote device
 * @param[in]  remote_dev_address,  remote bd address in string format
 * @param[in]  ediv, remote device ediv value.
 * @param[in]  rand, remote device rand value.
 * @param[in]  ltk,  remote device ltk value.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to start the SMP pairing process
 */
int32_t rsi_ble_start_encryption (uint8_t *remote_dev_address, uint16_t ediv, uint8_t *rand, uint8_t *ltk)
{
  rsi_ble_strat_encryption_t  encrypt = {{0}};
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(encrypt.dev_addr, remote_dev_address);
#else 
  memcpy (encrypt.dev_addr, (int8_t *)remote_dev_address, 6);
#endif
  encrypt.ediv = ediv;
  memcpy (encrypt.rand, rand, 8);
  memcpy (encrypt.ltk, ltk, 16);

  return rsi_bt_driver_send_cmd(RSI_BLE_RSP_START_ENCRYPTION, &encrypt, NULL);
}

#endif

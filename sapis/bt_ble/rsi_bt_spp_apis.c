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
 * @fn         rsi_bt_spp_init
 * @brief      sets profile mode
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to set the profile mode.
 */
int32_t rsi_bt_spp_init(void)
{ 
  rsi_bt_req_profile_mode_t bt_req_spp_init = {1};
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_SET_PROFILE_MODE, &bt_req_spp_init, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_bt_spp_connect
 * @brief      requests SPP connection.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used to initiate SPP connection.
 * */
int32_t rsi_bt_spp_connect(uint8_t *remote_dev_addr)
{
  rsi_bt_req_connect_t bt_req_spp_connect = {{0}};
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_spp_connect.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy (bt_req_spp_connect.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_SPP_CONNECT, &bt_req_spp_connect, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_bt_spp_disconnect
 * @brief      requests SPP disconnection.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for SPP disconnection.
 * */
int32_t rsi_bt_spp_disconnect(uint8_t *remote_dev_addr)
{
  rsi_bt_req_disconnect_t bt_req_spp_disconnect = {{0}};
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_spp_disconnect.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy (bt_req_spp_disconnect.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_SPP_DISCONNECT, &bt_req_spp_disconnect, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_bt_spp_transfer
 * @brief      requests SPP transfer.
 * @param[in]  remote_dev_address, remote device address
 * @param[in]  data, data for transmission
 * @param[in]  length, data length for transfer
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for SPP data transfer.
 * */
int32_t rsi_bt_spp_transfer(uint8_t *remote_dev_addr, uint8_t *data, uint16_t length)
{
  uint16_t xfer_len = 0;
  
  rsi_bt_req_spp_transfer_t bt_req_spp_transfer = {0};
  xfer_len = RSI_MIN(length, RSI_BT_MAX_PAYLOAD_SIZE);
  bt_req_spp_transfer.data_length = xfer_len;
 
  memcpy(bt_req_spp_transfer.data, data, xfer_len);

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_SPP_TRANSFER, &bt_req_spp_transfer, NULL);
}
#endif

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
 * @fn         rsi_bt_hfp_init
 * @brief      sets profile mode
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to set the profile mode.
 */
int32_t rsi_bt_hfp_init(void)
{ 
  rsi_bt_req_profile_mode_t bt_req_hfp_init = {RSI_HFP_PROFILE_BIT};
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_SET_PROFILE_MODE, &bt_req_hfp_init, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_bt_hfp_connect
 * @brief      requests HFP connection.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used to initiate HFP connection.
 * */
int32_t rsi_bt_hfp_connect(uint8_t *remote_dev_addr)
{
  rsi_bt_req_hfp_connect_t bt_req_hfp_connect = {{0}};
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_hfp_connect.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy (bt_req_hfp_connect.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_HFP_CONNECT, &bt_req_hfp_connect, NULL);
}

/*==============================================*/
/**
 * @fn         rsi_bt_hfp_disconnect
 * @brief      requests HFP disconnection.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for HFP disconnection.
 * */
int32_t rsi_bt_hfp_disconnect(uint8_t *remote_dev_addr)
{
  rsi_bt_req_hfp_disconnect_t bt_req_hfp_disconnect = {{0}};
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_hfp_disconnect.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy (bt_req_hfp_disconnect.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_HFP_DISCONNECT, &bt_req_hfp_disconnect, NULL);
}
/*==============================================*/
/**
 * @fn         rsi_bt_hfp_phoneoperator
 * @brief      requests HFP phoneoperator .
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for HFP phoneoperator.
 * */
int32_t rsi_bt_hfp_phoneoperator(uint8_t *operator)
{
  
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_HFP_PHONEOPERATOR, NULL, operator);
}
/*==============================================*/
/**
 * @fn         rsi_bt_hfp_callaccept
 * @brief      requests HFP callaccept.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for HFP callaccept.
 * */
int32_t rsi_bt_hfp_callaccept(void)
{
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_HFP_CALLACCEPT, NULL, NULL);
}
/*==============================================*/
/**
 * @fn         rsi_bt_hfp_callreject
 * @brief      requests HFP callreject.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for HFP callreject.
 * */
int32_t rsi_bt_hfp_callreject(void)
{
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_HFP_CALLREJECT, NULL, NULL);
}
/*==============================================*/
/**
 * @fn         rsi_bt_hfp_dialnum
 * @brief      requests HFP dialnum.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for HFP dialnum.
 * */
int32_t rsi_bt_hfp_dialnum(uint8_t *phone_no)
{
 rsi_bt_req_hfp_dialnum_t  bt_req_hfp_dialnum ={{0}};
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_HFP_DIALNUM,&bt_req_hfp_dialnum , NULL);
}
/*==============================================*/
/**
 * @fn         rsi_bt_hfp_dialmem
 * @brief      requests HFP dialmem.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for HFP dialmem.
 * */
int32_t rsi_bt_hfp_dialmem(uint8_t *mem_ix)
{
 rsi_bt_req_hfp_dialmem_t  bt_req_hfp_dialmem ={{0}};
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_HFP_DIALMEM,&bt_req_hfp_dialmem , NULL);
}

/*==============================================*/
/**
 * @fn         rsi_bt_hfp_redial
 * @brief      requests HFP redial.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for HFP redial.
 * */
int32_t rsi_bt_hfp_redial(void)
{
   return rsi_bt_driver_send_cmd(RSI_BT_REQ_HFP_REDIAL, NULL, NULL);
}
/*==============================================*/
/**
 * @fn         rsi_bt_hfp_voicerecognitionactive
 * @brief      requests HFP voicerecognitionactive.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for HFP voicerecognitionactive.
 * */
int32_t rsi_bt_hfp_voicerecognitionactive(void)
{
  
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_HFP_VOICERECOGNITIONACTIVE, NULL, NULL);
}
/*==============================================*/
/**
 * @fn         rsi_bt_hfp_voicerecognitiondeactive
 * @brief      requests HFP voicerecognitiondeactive.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for HFP voicerecognitiondeactive.
 * */
int32_t rsi_bt_hfp_voicerecognitiondeactive(void)
{
   return rsi_bt_driver_send_cmd(RSI_BT_REQ_HFP_VOICERECOGNITIONDEACTIVE, NULL, NULL);
}
/*==============================================*/
/**
 * @fn         rsi_bt_hfp_spkgain
 * @brief      requests HFP spkgain.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for HFP spkgain.
 * */
int32_t rsi_bt_hfp_spkgain(uint8_t spk_gain)
{
   rsi_bt_req_hfp_spkgain_t  bt_req_hfp_spkgain = {{0}};
    
   bt_req_hfp_spkgain.spk_gain=spk_gain;

 return rsi_bt_driver_send_cmd(RSI_BT_REQ_HFP_SPKGAIN, &bt_req_hfp_spkgain, NULL);
}
/*==============================================*/
/**
 * @fn         rsi_bt_hfp_micgain
 * @brief      requests HFP micgain.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for HFP micgain.
 * */
int32_t rsi_bt_hfp_micgain(uint8_t mic_gain)
{
   rsi_bt_req_hfp_micgain_t bt_req_hfp_micgain = {{0}};

   bt_req_hfp_micgain.mic_gain=mic_gain;

 return rsi_bt_driver_send_cmd(RSI_BT_REQ_HFP_MICGAIN, &bt_req_hfp_micgain, NULL);
}
/*==============================================*/
/**
 * @fn         rsi_bt_hfp_getcalls
 * @brief      requests HFP getcalls.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for HFP getcalls.
 * */
int32_t rsi_bt_hfp_getcalls(void)
{
  
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_HFP_GETCALLS, NULL, NULL);
}
/*==============================================*/
/**
 * @fn         rsi_bt_hfp_audio
 * @brief      requests HFP audio.
 * @param[in]  remote_dev_address, remote device address
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This API is used for HFP audio.
 * */
int32_t rsi_bt_hfp_audio(void)
{
  
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_HFP_AUDIO, NULL, NULL);
}

#endif

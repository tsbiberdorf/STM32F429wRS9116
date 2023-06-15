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

#ifndef RSI_BT_COMMON_APIS_H
#define RSI_BT_COMMON_APIS_H

#include<rsi_bt_common.h>
#include<rsi_data_types.h>
#include<rsi_utils.h>

/******************************************************
 * *                      Macros
 * ******************************************************/
//! success return value
#define RSI_SUCCESS                                   0

//! failure return value
#define RSI_FAILURE                                  -1


/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/

/******************************************************
 * *                 Type Definitions
 * ******************************************************/

/******************************************************
 * *                    Structures
 * ******************************************************/

/******************************************************
 * *                 Global Variables
 * ******************************************************/

/******************************************************
 * *              BT Common API's Declarations
 * ******************************************************/
/*==============================================*/
/**
 * @fn          int32_t rsi_bt_set_bd_addr(uint8_t *dev_addr)
 * @param[in]   dev_addr 
 * @return      int32_t 
 *              0  =  success
 *              !0 = failure 
 * @section description
 * This function is used to set the local device BD addr
 */
int32_t rsi_bt_set_bd_addr(uint8_t *dev_addr);

/*==============================================*/
/**
 * @fn          int32_t rsi_bt_ber_enable(struct rsi_bt_ber_cmd_s *rsi_bt_ber_cmd)
 * @param[in]  rsi_bt_ber_cmd_stats pointer address which contains the command structure
 * @return      command response status
 *
 *
 */
int32_t rsi_bt_ber_enable_or_disable(struct rsi_bt_ber_cmd_s *ber_cmd);

/*==============================================*/
/**
 * @fn         rsi_bt_set_local_name
 * @brief      sets the local BT/BLE device name
 * @param[in]  local_name, name to be set to the local BT/BLE device
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function sets the given name to the local BT/BLE device
 */
int32_t rsi_bt_set_local_name(uint8_t *local_name);

/*==============================================*/
/**
 * @fn         rsi_bt_get_local_name
 * @brief      requests the local BT/BLE device name
 * @param[out] resp, response buffer to hold the response of this API
 * @return     int32_t 
 *             0  = success
 *             !0 = failure 
 * @section description
 * This function is used know the name of the local BT/BLE device
 */
int32_t rsi_bt_get_local_name(rsi_bt_resp_get_local_name_t *bt_resp_get_local_name);

/*==============================================*/
/**
 * @fn         rsi_bt_get_rssi 
 * @brief      request the RSSI of the connected BT/BLE device
 * @param[out] resp, response buffer to hold the response of this API
 * @return     int32_t 
 *             0  =  success
 *             !0 = failure 
 * @section description
 * This function is used know the RSSI of the connected BT/BLE device
 */
int32_t rsi_bt_get_rssi(uint8_t *remote_dev_addr, int8_t *resp);

/*==============================================*/
/**
 * @fn         rsi_bt_get_local_device_address
 * @brief      request the local BT/BLE device address 
 * @param[out] resp, response buffer to hold the response of this API
 * @return     int32_t 
 *             0  = success
 *             !0 = failure 
 * @section description
 * This function is used to know the local BT/BLE device address
 */
int32_t rsi_bt_get_local_device_address(uint8_t *resp);

/*==============================================*/
/**
 * @fn         rsi_bt_init
 * @brief      Initializes the BT/BLE device
 * @param[in]  void
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to initialise the BT/BLE device.
 * Its recommended to use this API after rsi_bt_deinit API
 */
int32_t rsi_bt_init(void);

/*==============================================*/
/**
 * @fn         rsi_bt_deinit
 * @brief      de-initializes/stops the BT/BLE device
 * @param[in]  void
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to de-initialize the BT/BLE device.
 * rsi_bt_init API shall be used after rsi_bt_deinit API
 */
int32_t rsi_bt_deinit(void);

/*==============================================*/
/**
 * @fn         rsi_bt_set_antenna
 * @brief      sets the antenna of the local BT/BLE device
 * @param[in]  antenna_value, either internal/external antenna
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to select either internal/external antenna of the
 * BT/BLE device
 */
int32_t rsi_bt_set_antenna(uint8_t antenna_value);

/*==============================================*/
/**
 * @fn         rsi_bt_set_feature_bitmap
 * @brief      set or enable the  BT or BLE features at firmware
 * @param[in]  feature_bit_map, features bit map list
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to set or select the features bit map
 */
int32_t rsi_bt_set_feature_bitmap(uint32_t feature_bit_map);


/*==============================================*/
/**
 * @fn         rsi_bt_set_antenna_tx_power_level
 * @brief      sets the antenna transmit power of the local BT/BLE device
 * @param[in]  protocol_mode, BT or LE
 *             power_level, 
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 */
int32_t rsi_bt_set_antenna_tx_power_level(uint8_t protocol_mode, int8_t tx_power);


/*==============================================*/
/**
 * @fn         rsi_bt_get_stack_version 
 * @brief      request the RSSI of the connected BT/BLE device
 * @param[out] resp, response buffer to hold the response of this API
 * @return     int32_t 
 *             0  =  success
 *             !0 = failure 
 * @section description
 * This function is used know the RSSI of the connected BT/BLE device
 */
int32_t rsi_bt_get_bt_stack_version(rsi_bt_resp_get_bt_stack_version_t *bt_resp_get_bt_stack_version);
 


int32_t rsi_bt_power_save_profile(uint8_t psp_mode, uint8_t psp_type);

#endif

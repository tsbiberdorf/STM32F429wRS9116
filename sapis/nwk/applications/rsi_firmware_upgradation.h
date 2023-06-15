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

#ifndef RSI_FWUP_H
#define RSI_FWUP_H
/******************************************************
 * *                      Macros
 * ******************************************************/
//! Firmware upgrade packet types
#define RSI_FWUP_RPS_HEADER           1          
#define RSI_FWUP_RPS_CONTENT          0 


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
 * *               Function Declarations
 * ******************************************************/
int32_t rsi_fwup_start(uint8_t *rps_header);
int32_t rsi_fwup_load(uint8_t *content, uint16_t length);
int32_t rsi_ota_firmware_upgradation(uint8_t flags, uint8_t *server_ip, uint32_t server_port, uint16_t chunk_number, uint16_t timeout, uint16_t tcp_retry_count, void(*ota_fw_up_response_handler)(uint16_t status, uint16_t chunk_number));
#endif

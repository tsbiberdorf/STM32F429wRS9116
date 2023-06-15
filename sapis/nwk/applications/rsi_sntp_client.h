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

#ifndef RSI_SNTP_CLIENT_H
#define RSI_SNTP_CLIENT_H
/******************************************************
 * *                      Macros
 * ******************************************************/
#ifndef RSI_IPV6
#define   RSI_IPV6                         1
#endif


//!SNTP Method to USe 
#define RSI_SNTP_BROADCAST_MODE            1

//!SNTP Method to USe 
#define RSI_SNTP_UNICAST_MODE              2


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
int32_t rsi_sntp_client_create_async(uint8_t flags, uint8_t *server_ip, uint8_t sntp_method, uint16_t sntp_timeout,  
           void (*rsi_sntp_client_create_response_handler)(uint16_t status,const uint8_t cmd_type, const uint8_t *buffer ));
int32_t rsi_sntp_client_gettime(uint16_t length, uint8_t *sntp_time_rsp);
int32_t rsi_sntp_client_gettime_date(uint16_t length, uint8_t *sntp_time_date_rsp);
int32_t rsi_sntp_client_server_info(uint16_t length, uint8_t *sntp_server_response);
int32_t rsi_sntp_client_delete_async(void);
#endif

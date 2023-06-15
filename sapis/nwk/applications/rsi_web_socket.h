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

#ifndef RSI_WEB_SOCKET_H
#define RSI_WEB_SOCKET_H

/******************************************************
 * *                      Macros
 * ******************************************************/


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
int32_t rsi_web_socket_create(int8_t flags, uint8_t *server_ip_addr, uint16_t server_port, uint16_t device_port,
                                    uint8_t *webs_resource_name, uint8_t *webs_host_name, int32_t *socket_id, 
                                    void (*web_socket_data_receive_notify_callback)(uint32_t sock_no, uint8_t *buffer, uint32_t length));
int32_t rsi_web_socket_send_async(int32_t sockID, uint8_t opcode, int8_t *msg, int32_t msg_length);
int32_t rsi_web_socket_close(int32_t sockID);
#endif


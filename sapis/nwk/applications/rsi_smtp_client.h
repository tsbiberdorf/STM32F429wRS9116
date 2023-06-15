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

#ifndef RSI_SMTP_CLIENT_H
#define RSI_SMTP_CLIENT_H
/******************************************************
 * *                      Macros
 * ******************************************************/
#ifndef RSI_IPV6
#define   RSI_IPV6                       1
#endif

//! Low Priority
#define RSI_SMTP_MAIL_PRIORITY_LOW       1

//! Normal Priority
#define RSI_SMTP_MAIL_PRIORITY_NORMAL    2        

//! High Priority
#define RSI_SMTP_MAIL_PRIORITY_HIGH      4        

//! Auth type
#define RSI_SMTP_CLIENT_AUTH_LOGIN       1

//! Auth type
#define RSI_SMTP_CLIENT_AUTH_PLAIN       3

//SMTP OVER SSL
#define SMTP_SSL_ENABLED      BIT(2)
#define SMTP_SSL_TLSv_1         BIT(3)
#define SMTP_SSL_TLSv_1_2     BIT(4)
#define SMTP_SSL_TLSv_1_1     BIT(5)


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
int32_t rsi_smtp_client_mail_send_async(uint8_t *mail_recipient_address, uint8_t priority, uint8_t *mail_subject, uint8_t *mail_body, uint16_t mail_body_length,
                                                           void(*smtp_client_mail_response_handler)(uint16_t status, const uint8_t cmd_type));
int32_t rsi_smtp_client_create(uint8_t flags, uint8_t *username, uint8_t *password, uint8_t *from_address, uint8_t *client_domain, uint8_t auth_type, uint8_t *server_ip, uint32_t port);
int32_t rsi_smtp_client_delete_async(void(*smtp_client_delete_response_handler)(uint16_t status,const uint8_t cmd_type));
#endif

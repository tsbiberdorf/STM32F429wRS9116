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

#ifndef RSI_POP3_CLIENT_H
#define RSI_POP3_CLIENT_H

#include <stdint.h>
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


//! To create the POP3 session
int32_t rsi_pop3_session_create(uint8_t flags, uint8_t *server_ip_address, uint16_t server_port_number, 
                                uint8_t auth_type, uint8_t *username, uint8_t *password,
                                void(*rsi_pop3_response_handler)(uint16_t status, uint8_t type, uint8_t *buffer));
int32_t rsi_pop3_session_create_async(uint8_t flags, uint8_t *server_ip_address, uint16_t server_port_number, 
                                uint8_t auth_type, uint8_t *username, uint8_t *password,
                                void(*rsi_pop3_client_mail_response_handler)(uint16_t status, uint8_t type, const uint8_t *buffer)) ;


//! To delete the POP3 sssion
int32_t rsi_pop3_session_delete(void);

//! To get the Number of mails count and total size of the mails
int32_t rsi_pop3_get_mail_stats(void);

//! To get the mail size of the passed mail index
int32_t rsi_pop3_get_mail_list(uint16_t mail_index);

//! To get the mail header and mail body of the passed mail index
int32_t rsi_pop3_retrive_mail(uint16_t mail_index);

//! The mail will be marked as deleted on the POP3 server for the passed mail index
int32_t rsi_pop3_mark_mail(uint16_t mail_index);

//! Any messages/mails marked as deleted in the current session will unmarked
int32_t rsi_pop3_unmark_mail(void); /* or rsi_pop3_reset(void);*/

//! To get the connection status of the POP3 server
int32_t rsi_pop3_get_server_status(void);

#endif

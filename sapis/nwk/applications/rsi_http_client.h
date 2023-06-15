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

#ifndef RSI_HTTP_CLIENT_H
#define RSI_HTTP_CLIENT_H
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
#if RSI_HTTP_STATUS_INDICATION_EN
int32_t rsi_http_client_get_async(uint8_t flags, uint8_t *ip_address, uint16_t port, 
                            uint8_t *resource, uint8_t *host_name,
                            uint8_t *extended_header, uint8_t *user_name, uint8_t *password,
                            void(*http_client_get_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length,  const uint32_t moredata, uint16_t status_code));
int32_t rsi_http_client_post_async(uint8_t flags, uint8_t *ip_address, uint16_t port, 
                            uint8_t *resource, uint8_t *host_name, uint8_t *extended_header, 
                            uint8_t *user_name, uint8_t *password, uint8_t *post_data, uint32_t post_data_length,
                            void(*http_client_post_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata, uint16_t status_code));

int32_t rsi_http_client_post_data(uint8_t *file_content, uint16_t current_chunk_length, 
                            void(*http_client_post_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata, uint16_t status_code));
#else 
int32_t rsi_http_client_get_async(uint8_t flags, uint8_t *ip_address, uint16_t port, 
                            uint8_t *resource, uint8_t *host_name,
                            uint8_t *extended_header, uint8_t *user_name, uint8_t *password,
                            void(*http_client_get_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata));
int32_t rsi_http_client_post_async(uint8_t flags, uint8_t *ip_address, uint16_t port, 
                            uint8_t *resource, uint8_t *host_name, uint8_t *extended_header, 
                            uint8_t *user_name, uint8_t *password, uint8_t *post_data, uint32_t post_data_length,
                            void(*http_client_post_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata));

int32_t rsi_http_client_post_data(uint8_t *file_content, uint16_t current_chunk_length, 
                            void(*http_client_post_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata));
#endif
int32_t rsi_http_client_abort(void);

int32_t rsi_http_client_put_start(uint8_t flags, uint8_t *ip_address, uint32_t port_number, 
                            uint8_t *resource, uint8_t *host_name, uint8_t *extended_header, 
                            uint8_t *user_name, uint8_t *password, uint32_t content_length,
                            void(*callback)(uint16_t status, uint8_t type, const uint8_t *buffer, uint16_t length, const uint8_t moredata));

int32_t rsi_http_client_put_delete(void);

int32_t rsi_http_client_put_pkt(uint8_t *file_content, uint16_t current_chunk_length);
int32_t rsi_http_client_put_create(void);
#endif

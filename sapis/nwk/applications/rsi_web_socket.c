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

#include "rsi_driver.h"

#include "rsi_wlan_non_rom.h"
#include "rsi_web_socket.h"
extern rsi_socket_info_non_rom_t *rsi_socket_pool_non_rom;

/*==============================================*/
/**
 * @fn           int32_t rsi_web_socket_create(int8_t flags, uint8_t *server_ip_addr, uint16_t server_port, uint16_t device_port,
                                                     uint8_t *webs_resource_name, uint8_t *webs_host_name, int32_t *socket_id,
                                                      void (*web_socket_data_receive_notify_callback)(uint32_t sock_no, uint8_t *buffer, uint32_t length))
 * @brief        This API is used to create web socket client
 * @param[in]  flags,  to select version and security , BIT(0) : 0 - IPv4 , 1 - IPv6, BIT(1): 0 - WEBSOCKET , 1 - WEBSOCKET_SSL
 * @param[in]    server_ip_addr, web socket server ip address 
 * @param[in]    server_port, web socket server port no
 * @param[in]    device_port, module port number 
 * @param[in]    webs_resource_name, web resource name ,string of 50 characters maximum 
 * @param[in]    webs_host_name, web host name ,string of 50 characters maximum
 * @param[out]   sockect_id, socket id  
 * @param[in]    web_socket_create_response_handler, web_socket_response_handler 
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description  This API is used to create web socket client
 */

int32_t rsi_web_socket_create(int8_t flags, uint8_t *server_ip_addr, uint16_t server_port, uint16_t device_port,
                                    uint8_t *webs_resource_name, uint8_t *webs_host_name, int32_t *socket_id,                                    
                                    void (*web_socket_data_receive_notify_callback)(uint32_t sock_no, uint8_t *buffer, uint32_t length))
{

  int32_t     status = RSI_SUCCESS;
  int32_t     sockID, protocolFamily = 0 ,protocol = 0;
  rsi_pkt_t *pkt = NULL;
  rsi_req_socket_t *socket_create;
  struct      rsi_sockaddr_in server_addr, client_addr;
  struct      rsi_sockaddr_in6 server_addr_v6, client_addr_v6;

  //! check for invalid parameters
  if((web_socket_data_receive_notify_callback == NULL) ||
      (server_ip_addr == NULL))
  {
#ifdef RSI_WITH_OS
    rsi_set_os_errno(RSI_ERROR_EINVAL);
#endif    
    return  RSI_SOCK_ERROR;
  }

  //! check for ipv6 bit enable
  if(flags & RSI_IPV6)
  {
    protocolFamily = AF_INET6;
  }
  else
  {
    protocolFamily = AF_INET;
  }
  
  // check if ssl is enabled
  if(flags & RSI_SSL_ENABLE)
  {
    protocol |= RSI_SOCKET_FEAT_SSL;
  }

  //! Create socket
  sockID = rsi_socket_async(protocolFamily, SOCK_STREAM, protocol, web_socket_data_receive_notify_callback);
  if(sockID < 0)
  {
    return RSI_SOCK_ERROR;
  }

  *(int32_t *)socket_id = sockID;

  if(protocolFamily == AF_INET)
  {
    //! Memset client structure
    memset(&client_addr, 0, sizeof(client_addr));  

    //! Set family type
    client_addr.sin_family = protocolFamily;

    //! Set local port number
    client_addr.sin_port = htons(device_port);

    //! Bind socket
    status = rsi_bind(sockID, (struct rsi_sockaddr *) &client_addr, sizeof(client_addr));
  }
  else if(protocolFamily == AF_INET6)
  {

    //! Memset client structure
    memset(&client_addr_v6, 0, sizeof(client_addr_v6));  

    //! Set family type
    client_addr_v6.sin6_family = protocolFamily;

    //! Set local port number
    client_addr_v6.sin6_port = htons(device_port);

    //! Bind socket
    status = rsi_bind(sockID, (struct rsi_sockaddr *) &client_addr_v6, sizeof(client_addr_v6));
  }

  if(status != RSI_SUCCESS)
  {
    rsi_shutdown(sockID, 0);
#ifdef RSI_WITH_OS
    status = rsi_get_error(sockID);
    rsi_set_os_errno(status);
#endif    
    return status;
  }

  if(protocolFamily == AF_INET)
  {
    //! Set server structure
    memset(&server_addr, 0, sizeof(server_addr));  

    //! Set server address family
    server_addr.sin_family = protocolFamily;

    //! Set server port number, using htons function to use proper byte order
    server_addr.sin_port = htons(server_port);

    //! Set IP address to localhost
    memcpy((uint8_t *)&server_addr.sin_addr.s_addr, server_ip_addr,RSI_IPV4_ADDRESS_LENGTH);
  }
  else if(protocolFamily == AF_INET6)
  {
    //! Set server structure
    memset(&server_addr_v6, 0, sizeof(server_addr_v6));  

    //! Set server address family
    server_addr_v6.sin6_family = protocolFamily;

    //! Set server port number, using htons function to use proper byte order
    server_addr_v6.sin6_port = htons(server_port);

    //! Set IP address to localhost
    memcpy(server_addr_v6.sin6_addr.s6_addr, server_ip_addr, RSI_IPV6_ADDRESS_LENGTH);
  }
  if(webs_resource_name != NULL)
  {
    //! check valid lengths
    if(rsi_strlen(webs_resource_name) > RSI_WEBS_MAX_URL_LENGTH)
    {
#ifdef RSI_WITH_OS
    rsi_set_os_errno(RSI_ERROR_EINVAL);
#endif          
      return  RSI_SOCK_ERROR;
    }
  }
  if(webs_host_name != NULL)
  {
    //! check valid lengths
    if(rsi_strlen(webs_host_name) > RSI_WEBS_MAX_HOST_LENGTH)
    {
#ifdef RSI_WITH_OS
    rsi_set_os_errno(RSI_ERROR_EINVAL);
#endif          
      return  RSI_SOCK_ERROR;
    }
  }

  pkt = rsi_pkt_alloc(&rsi_driver_cb->wlan_cb->wlan_tx_pool);
  if(pkt == NULL)
  {
    //! Set error
    rsi_wlan_socket_set_status(RSI_ERROR_PKT_ALLOCATION_FAILURE,sockID);
#ifdef RSI_WITH_OS
    status = rsi_get_error(sockID);
    rsi_set_os_errno(status);
#endif
    return RSI_SOCK_ERROR;
  }
  
  if(protocolFamily == AF_INET)  
  {
    //! Save server IPv4 address
    memcpy(rsi_socket_pool[sockID].destination_ip_addr.ipv4, (uint8_t *)&server_addr.sin_addr.s_addr, RSI_IPV4_ADDRESS_LENGTH);
  }
  else
  {
    memcpy(rsi_socket_pool[sockID].destination_ip_addr.ipv6, server_addr_v6.sin6_addr.s6_addr, RSI_IPV6_ADDRESS_LENGTH);    
  }

  rsi_socket_pool[sockID].destination_port =  htons(server_port);  

  //! Get data pointer
  socket_create = (rsi_req_socket_t *)pkt->data;

  //! Memset before filling
  memset(socket_create, 0, sizeof(rsi_req_socket_t));

  //! Fill IP verison and destination IP address
  if((rsi_socket_pool[sockID].sock_type >> 4) == AF_INET)
  {
    rsi_uint16_to_2bytes(socket_create->ip_version, 4);
    memcpy(socket_create->dest_ip_addr.ipv4_address, rsi_socket_pool[sockID].destination_ip_addr.ipv4, RSI_IPV4_ADDRESS_LENGTH);
  }
  else
  {
    rsi_uint16_to_2bytes(socket_create->ip_version, 6);
    memcpy(socket_create->dest_ip_addr.ipv6_address, rsi_socket_pool[sockID].destination_ip_addr.ipv6, RSI_IPV6_ADDRESS_LENGTH);
  }

  //! Fill local port
  rsi_uint16_to_2bytes(socket_create->module_socket, rsi_socket_pool[sockID].source_port);

  //! Fill destination port
  rsi_uint16_to_2bytes(socket_create->dest_socket, rsi_socket_pool[sockID].destination_port); 

  //! Fill socket type
  rsi_uint16_to_2bytes(socket_create->socket_type, RSI_SOCKET_TCP_CLIENT); 

  //! set max count to 0
  rsi_uint16_to_2bytes(socket_create->max_count, 0);   

  //! set websocket support in socket information pool
  rsi_socket_pool[sockID].sock_bitmap |= RSI_SOCKET_FEAT_WEBS_SUPPORT;

  //! set websocket support for the socket
  socket_create->ssl_bitmap |= RSI_SOCKET_FEAT_WEBS_SUPPORT;

  if(webs_resource_name != NULL)
  {
    //! copy webs resource name
    rsi_strcpy(socket_create->webs_resource_name, webs_resource_name);
  } 
  if(webs_host_name != NULL)
  {
    //!copy webs host name
    rsi_strcpy(socket_create->webs_host_name, webs_host_name);
  }
  //! Check for SSL feature and fill it in ssl bitmap
  if(rsi_socket_pool[sockID].sock_bitmap & RSI_SOCKET_FEAT_SSL)
  {
    socket_create->ssl_bitmap |= RSI_SOCKET_FEAT_SSL;
  }

  //! Check for SYNCHRONOUS feature and fill it in socket bitmap
  if(rsi_socket_pool[sockID].sock_bitmap & RSI_SOCKET_FEAT_SYNCHRONOUS)
  {
    socket_create->socket_bitmap |= BIT(0);
  }

  //! Set waiting socket ID
  rsi_driver_cb->wlan_cb->waiting_socket_id = sockID;
  socket_create->vap_id  = rsi_socket_pool_non_rom[sockID].vap_id;
  rsi_uint32_to_4bytes(socket_create->tos, RSI_TOS);
  socket_create->ssl_ciphers  = RSI_SSL_CIPHERS;
  if(rsi_socket_pool_non_rom[sockID].max_tcp_retries)
  {
    socket_create->max_tcp_retries_count  = rsi_socket_pool_non_rom[sockID].max_tcp_retries;
  }
  else
  {
    socket_create->max_tcp_retries_count  = 10;
  }
  if(rsi_socket_pool_non_rom[sockID].tcp_keepalive_initial_time)
  {
    memcpy(socket_create->tcp_keepalive_initial_time,rsi_socket_pool_non_rom[sockID].tcp_keepalive_initial_time,2);
  }
  else
  {
    rsi_uint16_to_2bytes(socket_create->tcp_keepalive_initial_time,RSI_SOCKET_KEEPALIVE_TIMEOUT);
  }
  if(rsi_socket_pool_non_rom[sockID].ssl_bitmap)
  {
    socket_create->ssl_bitmap  = rsi_socket_pool_non_rom[sockID].ssl_bitmap;
  }
  else if(socket_create->socket_bitmap & RSI_SOCKET_FEAT_SSL)
  {
    socket_create->ssl_bitmap   =  rsi_wlan_cb_non_rom->tls_version;
  }
  if(rsi_socket_pool_non_rom[sockID].high_performance_socket)
  {
    socket_create->ssl_bitmap  |= RSI_HIGH_PERFORMANCE_SOCKET;
  }

#ifndef RSI_SOCK_SEM_BITMAP
  rsi_socket_pool_non_rom[sockID].socket_wait_bitmap |= BIT(0);
#endif
  //! Send socket create command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SOCKET_CREATE, pkt);

  //! wait on socket semaphore
  status = rsi_wait_on_socket_semaphore(&rsi_socket_pool_non_rom[sockID].socket_sem, RSI_SOCKET_CREATE_RESPONSE_WAIT_TIME);
  if(status != RSI_ERROR_NONE)
  {
    //! get wlan/network command response status
    rsi_wlan_socket_set_status(status,sockID);
#ifdef RSI_WITH_OS
    status = rsi_get_error(sockID);
    rsi_set_os_errno(status);
#endif    
    return RSI_SOCK_ERROR;
  }
  //! get wlan/network command response status
  status =  rsi_wlan_socket_get_status(sockID);
  if(status == RSI_SUCCESS)
  {
    //! Change state to connected
    rsi_socket_pool[sockID].sock_state = RSI_SOCKET_STATE_CONNECTED;
    
    return RSI_SUCCESS;
  }
  else
  {
    rsi_shutdown(sockID, 0);
#ifdef RSI_WITH_OS
    status = rsi_get_error(sockID);
    rsi_set_os_errno(status);
#endif    
    return status;
  }
}



/*==============================================*/
/**
 * @fn          int32_t rsi_web_socket_send_async(int32_t sockID, uint8_t opcode, int8_t *msg, int32_t msg_length, int32_t flags);
 * @brief       This function is used to send data on a websocket client
 * @param[in]   sockID, Application socket id 
 * @param[in]   opcode, opcode
 * @param[in]   msg, message
 * @param[in]   msg_length, message length
 * @param[in]   flags, reserved
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This function is used to send data on a websocket client
 */

int32_t rsi_web_socket_send_async(int32_t sockID, uint8_t opcode, int8_t *msg, int32_t msg_length)
{
  int32_t  status          = RSI_SUCCESS;

  //! set as final packet
  rsi_socket_pool[sockID].opcode = opcode; 

  //! call send API to send web socket data
  status = rsi_send(sockID, msg, msg_length, 0);

  return status;

}

/*==============================================*/
/**
 * @fn          int32_t rsi_web_socket_close(int32_t sockID);
 * @brief       This API is used to close web socket  
 * @param[in]   sockID, socket id of particular socket 
 * @param[out]  None
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This        This API is used to close web socket 
 *
 *
 */
int32_t rsi_web_socket_close(int32_t sockID)
{
  int32_t     status = RSI_SUCCESS;
  
  //! web socket client socket close only sock id based
 status =  rsi_shutdown(sockID, 0);

 return status;

}


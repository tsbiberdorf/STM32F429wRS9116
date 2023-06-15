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

#ifndef ROM_WIRELESS
#include "rsi_driver.h"

/*==============================================*/
/**
 * @fn          int32_t  ROM_WL_rsi_get_application_socket_descriptor(global_cb_t *global_cb_p, int32_t sock_id)
 * @brief      Gets application socket descriptor from module socket descriptor
 * @param[in]  global_cb_p,pointer to the global control block
 * @param[in]   sock_id, module's socket descriptor
 * @param[out]  None
 * @return
 *             <0 - If index is not found
 *             >0 - application index
 *
 * @section description
 * This function is used to get the application socket descriptor from module socket descriptor
 *
 */
int32_t ROM_WL_rsi_get_application_socket_descriptor(global_cb_t *global_cb_p, int32_t sock_id)
{
  int i;
  
  rsi_socket_info_t *rsi_socket_pool = global_cb_p->rsi_socket_pool;

  for(i = 0; i < NUMBER_OF_SOCKETS; i++)
  {
    if(rsi_socket_pool[i].sock_id == sock_id)
    {
      break;
    }
  }

  if(i >= NUMBER_OF_SOCKETS)
  {
    return -1;
  }

  return i;
}


/*==============================================*/
/**
 * @fn          int32_t ROM_WL_rsi_get_primary_socket_id(global_cb_t *global_cb_p, uint16_t port_number)
 * @brief       function to get the primary socket ID from port number
 * @param[in]  global_cb_p,pointer to the global control block
 * @param[in]   port_number, port number
 * @param[out]  None
 * @return 
 *              <0  - If socket is not found
 *              >=0 - socket descriptor
 *
 * @section description
 * This function is used to clear socket information
 *
 */
int32_t ROM_WL_rsi_get_primary_socket_id(global_cb_t *global_cb_p, uint16_t port_number)
{
  int i;
  
  rsi_socket_info_t *rsi_socket_pool = global_cb_p->rsi_socket_pool;

  for(i = 0; i < NUMBER_OF_SOCKETS; i++)
  {
    if((rsi_socket_pool[i].source_port == port_number) && (rsi_socket_pool[i].ltcp_socket_type == RSI_LTCP_PRIMARY_SOCKET))
    {
      break;
    }
  }

  if(i >= NUMBER_OF_SOCKETS)
  {
    return -1;
  }
  return i;
}

/*==============================================*/
/**
 * @fn          uint8_t ROM_WL_calculate_buffers_required(global_cb_t *global_cb_p, uint8_t type, uint16_t length)
 * @brief       This api caluculates the buffers required for the data packet to transmit
 * @param[in]   global_cb_p,pointer to the global control block
 * @param[in]   type, type of socket to create
 * @param[in]   length,length of the message

 * @return The number of buffers required for a data packet
 *
 *
 * @section description
 * This function is used to calculate number of buffers required for the 
 * data packet
 *
 */
uint8_t ROM_WL_calculate_buffers_required(global_cb_t *global_cb_p, uint8_t type, uint16_t length)
{

  uint8_t header_size, buffers_required = 1;
  uint16_t first_buffer_available_size, remaining_length;

  //! Calculate header size including extra 2bytes based on the proto type
  if((type & 0xF) == SOCK_STREAM)
  {
    header_size = 56;
  }
  else
  {
    header_size = 44;
  }

  //! Increase header size by 20 for IPv6 case
  if((type >> 4) == AF_INET6)
  {
    header_size += 20;
  }

  remaining_length = length;

  first_buffer_available_size = (512 - header_size - 252);
  
  if(length <= first_buffer_available_size)
  {
    return 1;
  }

  remaining_length -= first_buffer_available_size;

  do
  {
    buffers_required++;
    if(remaining_length > 512)
    {
      remaining_length -= 512;
    }
    else
    {
      remaining_length = 0;
    }

  }while(remaining_length);

  return buffers_required;
}
/*==============================================*/
/**
 * @fn         uint16_t ROM_WL_calculate_length_to_send(global_cb_t *global_cb_p, uint8_t type, uint8_t buffers)
 * @brief      This API caluculates the msg length sent using available buffers      
 * @param[in]   global_cb_p,pointer to the global control block
 * @param[in]  type,type of the socket stream
 * @param[in]  buffers,available buffers
 * @param[out]
 * @return
 *
 *
 * @section description
 * This API caluculates the msg length sent using available buffers       
 *
 *
 */

uint16_t ROM_WL_calculate_length_to_send(global_cb_t *global_cb_p, uint8_t type, uint8_t buffers)
{
  uint8_t header_size;
  uint16_t length;

  //! Calculate header size including extra 2bytes based on the proto type
  if((type & 0xF) == SOCK_STREAM)
  {
    header_size = 56;
  }
  else
  {
    header_size = 44;
  }

  //! Increase header size by 20 for IPv6 case
  if((type >> 4) == AF_INET6)
  {
    header_size += 20;
  }
 
  length = (512 - header_size - 252);

  if(buffers == 1)
  {
    return length;
  }

  buffers--;

  while(buffers)
  {
    length += 512;
	buffers--;
  }

  return length;

}
#endif




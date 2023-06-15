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
#include "rsi_json_handlers.h"


/*==============================================*/
/**
 * @fn                int32_t rsi_webpage_load(uint8_t flags, uint8_t *file_name, uint8_t *webpage, uint32_t length)
 * @brief             This function is used to load webpage to the HTTP server 
 * @param[in]   ,     flags,  to enable json object set BIT(3) 
 * @param[in]   ,     file_name, file name or URL name of the webpage
 * @param[in]   ,     webpage, webpage to load
 * @param[in]   ,     length, length of the webpage
 * @param[out]  ,     None
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This function is used to load the HTML webpages to the HTTP server
 *
 *
 */
int32_t rsi_webpage_load(uint8_t flags, uint8_t *file_name, uint8_t *webpage, uint32_t length)
{
  rsi_webpage_load_t        *webpage_load;
  rsi_pkt_t                 *pkt;
  int32_t                    status    = RSI_SUCCESS;
  uint16_t                   send_size = 0;
  uint8_t                   *host_desc = NULL;
  uint16_t                   current_len  =  RSI_MAX_WEBPAGE_SEND_SIZE;
  uint16_t                   offset = 0;
  uint32_t                   file_length = length;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

 if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
 {
  while(file_length)
  {
  //! allocate command buffer  from wlan pool
  pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(NWK_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }
  
  webpage_load = (rsi_webpage_load_t *) pkt->data;

    //! Fill total length of the webpage
    rsi_uint16_to_2bytes(webpage_load->total_len, length);

    //! Fill file name of the webpage
    rsi_strcpy(&webpage_load->filename, file_name);

    if(flags & RSI_WEB_PAGE_JSON_ENABLE)
    {
      //! Webpage associated json object flag
      webpage_load->has_json_data = RSI_WEB_PAGE_HAS_JSON;  
    }
    else
    {
      webpage_load->has_json_data = 0;  
    }

    //! Get the current length that you want to send
    current_len = ((length - offset) > RSI_MAX_WEBPAGE_SEND_SIZE) 
      ? RSI_MAX_WEBPAGE_SEND_SIZE : (length - offset);


    //! Update the current length to be sent
    rsi_uint16_to_2bytes(webpage_load->current_len, current_len);

    //! Copy the webpage file contents into the buffer
    memcpy((uint8_t*)(webpage_load->webpage), (uint8_t*)(webpage + offset), current_len);

    //! Calculate send size
    send_size = sizeof(rsi_webpage_load_t) - RSI_MAX_WEBPAGE_SEND_SIZE + current_len;

    //! get the host descriptor
    host_desc = (pkt->desc);

    //! Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_NWK_SEM_BITMAP
  rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    //! send webpage load request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WEBPAGE_LOAD, pkt);

    //! wait on nwk semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_WP_LOAD_RESPONSE_WAIT_TIME); 
    
    //! get wlan/network command response status
    status =  rsi_wlan_get_nwk_status();
    //! If  fails ,donot send other chunks
    if(status != RSI_SUCCESS)
    {
      break;
    }

    //! Increase webpage content offset
    offset  += current_len;

    //! Decrese webpage remaining size
    if(file_length >= current_len)
    {
      file_length -= current_len;
    }
    else
    {
      file_length = 0;
    }

  }

  //!Changing the nwk state to allow 
  rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 

 }
 else
 {
   //! return nwk command error
   return RSI_ERROR_NWK_CMD_IN_PROGRESS;
 }
  
 //! Return the status
  return status;

}



/*==============================================*/
/**
 * @fn                int32_t rsi_json_object_create(uint8_t flags, uint8_t *file_name, uint8_t *json_object, uint32_t length)
 * @brief             This function is used to load json object to the perticular webpage
 * @param[in]   ,     flags,  to enable IPv6, set BIT(0), Bydefault it selects IPv4 
 * @param[in]   ,     file_name, file name of the json object
 * @param[in]   ,     json_object, json object to load
 * @param[in]   ,     length, length of the json object
 * @param[out]  ,     None
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This function is used to load the json object to the HTML webpage
 *
 *
 */

int32_t rsi_json_object_create(uint8_t flags, const char *file_name, uint8_t *json_object, uint32_t length)
{
  rsi_json_object_load_t     *json_object_create;
  rsi_pkt_t                  *pkt;
  int32_t                     status       = RSI_SUCCESS;
  uint16_t                    send_size    = 0;
  uint8_t                    *host_desc    = NULL;
  uint16_t                    current_len  = RSI_MAX_WEBPAGE_SEND_SIZE;
  uint16_t                    offset       = 0;
  uint32_t                    file_length = 0;


  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

 if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
 { 
  file_length =  length;

  while(file_length)
  {
  //! allocate command buffer  from wlan pool
  pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(NWK_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  json_object_create = (rsi_json_object_load_t *) pkt->data;



    //! Fill total length of the json object
    rsi_uint16_to_2bytes(json_object_create->total_length, length);

    //! Fill file name of the json object
    rsi_strcpy(&json_object_create->filename, file_name);

    //! Get the current length that you want to send
    current_len = ((length - offset) > RSI_JSON_MAX_CHUNK_LENGTH) 
      ? RSI_JSON_MAX_CHUNK_LENGTH : (length - offset);

    //! Update the current length to be sent
    rsi_uint16_to_2bytes(json_object_create->current_length, current_len);

    //! Copy the json object file contents into the buffer
    memcpy((uint8_t*)(json_object_create->json_object), (uint8_t *)(json_object + offset), current_len);

    //! Calculate send size
    send_size = sizeof(rsi_json_object_load_t) - RSI_JSON_MAX_CHUNK_LENGTH + current_len;

    //! get the host descriptor
    host_desc = (pkt->desc);

    //! Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_NWK_SEM_BITMAP
  rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    //! send json object create request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_JSON_LOAD, pkt);

    //! wait on nwk semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_JSON_LOAD_RESPONSE_WAIT_TIME); 
    
    //! get wlan/network command response status
    status =  rsi_wlan_get_nwk_status();
    
    //! If  fails ,donot send other chunks
    if(status != RSI_SUCCESS)
    {
      break;
    }

    //! Increase json object content offset
    offset  += current_len;

    //! Decrese json object remaining size
    if(file_length >= current_len)
    {
      file_length -= current_len;
    }
    else
    {
      file_length = 0;
    }

  }

  //!Changing the nwk state to allow 
  rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 

 }
 else
 {
   //! return nwk command error
   return RSI_ERROR_NWK_CMD_IN_PROGRESS;
 }
  

  //! Return the status
  return status;

}


/*==============================================*/
/**
 * @fn                int32_t rsi_webpage_erase(uint8_t *file_name)
 * @brief             This function is used to erase the perticular webpage 
 * @param[in]   ,     file_name, file name or URL name of the webpage
 * @param[out]  ,     None
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This function is used to erase the HTML webpage 
 *
 */

int32_t rsi_webpage_erase(uint8_t *file_name)
{

  rsi_wbpage_erase_t         *web_page_erase;
  rsi_webpage_erase_all_t    *web_page_erase_all;
  rsi_pkt_t                  *pkt;
  int32_t                     status       = RSI_SUCCESS;
  uint16_t                    send_size    = 0;
  uint8_t                    *host_desc    = NULL;
  uint8_t                     cmd_type     = 0x00;
  int32_t                     rsi_response_wait_time     = 0;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

 if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
 { 
  //! allocate command buffer  from wlan pool
  pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(NWK_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  web_page_erase     = (rsi_wbpage_erase_t *) pkt->data;
  web_page_erase_all = (rsi_webpage_erase_all_t *)pkt->data;

  if(file_name)
  {

    //! memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_wbpage_erase_t));

    //! Fill file name of the json object
    rsi_strcpy(&web_page_erase->filename, file_name);

    //! Using host descriptor to set payload length
    send_size = sizeof(rsi_wbpage_erase_t);

    //! Set command type 
    cmd_type  = RSI_WLAN_REQ_WEBPAGE_ERASE; 
        
    rsi_response_wait_time = RSI_WP_ERASE_RESPONSE_WAIT_TIME;
  }
  else
  {

    //! memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_webpage_erase_all_t));

    //! Clear all webpage files
    web_page_erase_all->clear = RSI_WEB_PAGE_CLEAR_ALL_FILES;

    //! Using host descriptor to set payload length
    send_size = sizeof(rsi_webpage_erase_all_t);

    //! Set command type 
    cmd_type  = RSI_WLAN_REQ_WEBPAGE_CLEAR_ALL;
        
    rsi_response_wait_time = RSI_WP_CLEAR_ALL_RESPONSE_WAIT_TIME;
  }

  //! get the host descriptor
  host_desc = (pkt->desc);

  //! Fill data length in the packet host descriptor
  rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_NWK_SEM_BITMAP
  rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
  //! send webpage erase request command
  status = rsi_driver_wlan_send_cmd((rsi_wlan_cmd_request_t)cmd_type, pkt);

  //! wait on nwk semaphore
  rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, rsi_response_wait_time); 
  
  //! get wlan/network command response status
  status =  rsi_wlan_get_nwk_status();

  //!Changing the nwk state to allow 
  rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 

 }
 else
 {
   //! return nwk command error
   return RSI_ERROR_NWK_CMD_IN_PROGRESS;
 }

  //! Return the status
  return status;

}


/*==============================================*/
/**
 * @fn                int32_t rsi_json_object_erase(uint8_t *file_name)
 * @brief             This function is used to erase the perticular json object 
 * @param[in]   ,     file_name, file name of the json object
 * @param[out]  ,     None
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This function is used to erase the json object 
 *
 */

int32_t rsi_json_object_delete(uint8_t *file_name)
{

  rsi_json_object_erase_t    *json_object_delete;
  rsi_pkt_t                  *pkt;
  int32_t                     status       = RSI_SUCCESS;
  uint16_t                    send_size    = 0;
  uint8_t                    *host_desc    = NULL;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

 if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
 { 
 
  //! allocate command buffer  from wlan pool
  pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(NWK_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  json_object_delete     = (rsi_json_object_erase_t *) pkt->data;

  //! memset the packet data
  memset(&pkt->data, 0, sizeof(rsi_json_object_erase_t));

  //! Fill file name of the json object
  rsi_strcpy(&json_object_delete->filename, file_name);

  //! Using host descriptor to set payload length
  send_size = sizeof(rsi_json_object_erase_t);

  //! get the host descriptor
  host_desc = (pkt->desc);

  //! Fill data length in the packet host descriptor
  rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_NWK_SEM_BITMAP
  rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
  //! send json object delete request command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_JSON_OBJECT_ERASE, pkt);

  //! wait on nwk semaphore
  rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_JSON_ERASE_RESPONSE_WAIT_TIME); 
  
  //! get wlan/network command response status
  status =  rsi_wlan_get_nwk_status();

  //!Changing the nwk state to allow 
  rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 

 }
 else
 {
   //! return nwk command error
   return RSI_ERROR_NWK_CMD_IN_PROGRESS;
 }

  //! Return the status
  return status;
}

/*==============================================*/
/**
 * @fn                int32_t rsi_webpage_send(uint8_t flags, uint8_t *webpage, uint32_t length)
 * @brief             This function is used to load json object to the perticular webpage
 * @param[in]   ,     flags,  to enable IPv6, set BIT(0), Bydefault it selects IPv4 
 * @param[in]   ,     webpage, webpage to send
 * @param[in]   ,     length, length of the webpage
 * @param[out]  ,     None
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This function is used to send the HTML webpage
 *
 *
 */
int32_t rsi_webpage_send(uint8_t flags, uint8_t *webpage, uint32_t length)
{

  rsi_webpage_send_t         *webpage_send;
  rsi_pkt_t                  *pkt;
  int32_t                     status       = RSI_SUCCESS;
  uint16_t                    send_size    = 0;
  uint8_t                    *host_desc    = NULL;
  uint16_t                    current_len  = RSI_MAX_WEBPAGE_SEND_SIZE;
  uint32_t                    offset       = 0;
  uint32_t                    file_size    = length;


  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! if state is not in card ready received state
  if(wlan_cb->state < RSI_WLAN_STATE_CONNECTED)
  {
    //! Command given in wrong state 
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

 if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
 { 
  while(file_size)
  {
  //! allocate command buffer  from wlan pool
  pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(NWK_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  webpage_send = (rsi_webpage_send_t *) pkt->data;


    //! memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_webpage_send_t));

    //! Fill total host web page size
    rsi_uint32_to_4bytes(webpage_send->total_len, length);

    //! Fill more chunks
    if((length - offset) <= RSI_MAX_HOST_WEBPAGE_SEND_SIZE)
    {
      webpage_send->more_chunks = 0;
    }
    else
    {
      webpage_send->more_chunks = 1;
    }

    //! Get the current length that you want to send
    current_len = ((length - offset) > RSI_MAX_HOST_WEBPAGE_SEND_SIZE) 
      ? RSI_MAX_HOST_WEBPAGE_SEND_SIZE : (length - offset);

    //! Fill web page content
    memcpy((uint8_t*)(webpage_send->webpage), (uint8_t*)(webpage + offset), current_len);

    //! calculate length of frame to send
    send_size = current_len + (sizeof(rsi_webpage_send_t) - sizeof(webpage_send->webpage));

    //! get the host descriptor
    host_desc = (pkt->desc);

    //! Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_NWK_SEM_BITMAP
  rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    //! send webpage load request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HOST_WEBPAGE_SEND, pkt);

    //! wait on nwk semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_HOST_WP_SEND_RESPONSE_WAIT_TIME); 
    
    //! get wlan/network command response status
    status =  rsi_wlan_get_nwk_status();
    
    //! If  fails ,donot send other chunks
    if(status != RSI_SUCCESS)
    {
      break;
    }

    //! Increase webpage content offset
    offset  += current_len;

    //! Decrese webpage remaining size
    if(file_size >= current_len)
    {
      file_size -= current_len;
    }
    else
    {
      file_size = 0;
    }

  }

  //!Changing the nwk state to allow 
  rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 

 }
 else
 {
   //! return nwk command error
   return RSI_ERROR_NWK_CMD_IN_PROGRESS;
 }
  

  //! Return the status
  return status;
}



/*==============================================*/
/**
 * @fn                int32_t rsi_req_wireless_fwup(void)
 * @brief             This function is used to send the wireless firmware upgrade request 
 * @param[in]   ,     None
 * @param[out]  ,     None
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This function is used to send the wireless firmware upgrade request 
 *
 */

int32_t rsi_req_wireless_fwup(void)
{
  rsi_pkt_t                  *pkt;
  int32_t                     status       = RSI_SUCCESS;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

 if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
 { 

  //! allocate command buffer  from wlan pool
  pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(NWK_CMD,ALLOW);
    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  if(rsi_wlan_cb_non_rom->nwk_callbacks.rsi_wireless_fw_upgrade_handler == NULL)
  {
#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
  }
  //! send wireless firmware upgrade request command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WIRELESS_FWUP, pkt);

  if(rsi_wlan_cb_non_rom->nwk_callbacks.rsi_wireless_fw_upgrade_handler == NULL)
  {
    //! wait on nwk semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_FWUP_RESPONSE_WAIT_TIME); 

    //! get wlan/network command response status
    status =  rsi_wlan_get_nwk_status();
    //!Changing the nwk state to allow 
    rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 
  }
 }
 else
 {
   //! return nwk command error
   return RSI_ERROR_NWK_CMD_IN_PROGRESS;
 }
  

  //! Return the status
  return status;
}
/*==============================================================================
  @fn                int32_t rsi_http_credentials(int8_t *username , int8_t *password)
 * @brief             This function is used to send the http server credentials request
 * @param[in]   ,     username , password
 * @param[out]  ,      
 * @return           int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This function is used to send the http sever credentials request 
 *
 * ============================================================================*/


int32_t rsi_http_credentials(int8_t *username , int8_t *password)
{
	rsi_pkt_t                  *pkt;
	int32_t                     status       = RSI_SUCCESS;
	rsi_req_http_credentials_t *http_ptr;
	//! Get wlan cb structure pointer
	rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;
	if(wlan_cb->state > RSI_WLAN_STATE_BAND_DONE)
	{
		//! Command given in wrong state 
	   return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
	}
	
 if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
 { 
  //! allocate command buffer  from wlan pool
	pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
	//! If allocation of packet fails
	if(pkt == NULL)
	{
    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(NWK_CMD,ALLOW);
		//! return packet allocation failure error
		return RSI_ERROR_PKT_ALLOCATION_FAILURE;
	}

  memset(&pkt->data, 0, sizeof(rsi_req_http_credentials_t));

	http_ptr = (rsi_req_http_credentials_t *) pkt->data;

	rsi_strcpy(http_ptr->username, username);
	rsi_strcpy(http_ptr->password, password);

#ifndef RSI_NWK_SEM_BITMAP
  rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
	//! send http_credentials 
	status = rsi_driver_wlan_send_cmd( RSI_WLAN_REQ_HTTP_CREDENTIALS , pkt);

  //! wait on nwk semaphore
  rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_HTTP_RESPONSE_WAIT_TIME); 
  //! get wlan/network command response status
  status =  rsi_wlan_get_nwk_status();

  //!Changing the nwk state to allow 
  rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 

 }
 else
 {
   //! return nwk command error
   return RSI_ERROR_NWK_CMD_IN_PROGRESS;
 }
  

	//! Return the status
	return status;
}

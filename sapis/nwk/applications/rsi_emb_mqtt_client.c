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
#include "rsi_emb_mqtt_client.h"


#define RSI_LENGTH_ADJ 2
/*==============================================*/
/**
 * @fn         rsi_emb_mqtt_client_info_t * rsi_mqtt_client_init( int8_t *buffer, uint32_t length, int8_t *server_ip, uint32_t server_port, uint32_t client_port, uint16_t flags, uint16_t keep_alive_interval)
 * @brief      Allocates memory for MQTT client information 
 * @param[in]  buffer , buffer pointer to  allocate  memory for MQTT Client information 
 * @param[in]  length , buffer length
 * @param[in]  server_ip , IPv4 address of the MQTT broker.  
 * @param[in]  server_port ,MQTT broker port number
 * @param[in]  client_port ,MQTT client port number
 * @param[in]  flags,Each bit has its own significance
 *              BIT(0) - Clean Session 
 *                   0 - Disable Clean Session
 *                   1 - Enable Clean Session
 *              BIT(1) - SSL Enable Bit
 *                 0 - SSL Disable
 *                 1 - SSL Enable
 *              Bit(2) - IP version of the Server IP 
 *                 0 - IPv4 
 *                 1 - IPv6
 * @param[in]  keep_alive_interval , MQTT client keep alive interval
 *             If there are no transactions between MQTT client and broker with in 
 *             this time period, MQTT Broker disconnects the MQTT client
 *             If 0 -> Server Doesnot disconnects
 *
 * @param[in]  clientid ,MQTT client port number
 * @param[in]  username ,MQTT server login credentials,username (string)
 * @param[in]  password ,MQTT server login credentials,password (string)
 *             
 * @section description
 * This API is used to allocate memory for the MQTT for a single client.
 *
 *
 */

int32_t rsi_emb_mqtt_client_init(int8_t *server_ip, uint32_t server_port, uint32_t client_port, uint16_t flags, uint16_t keep_alive_interval,int8_t *clientid,int8_t *username, int8_t *password)
{
  int32_t  status = RSI_SUCCESS;

  rsi_pkt_t       *pkt;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_emb_mqtt_client_init_t *mqtt_ops = NULL;


  if(wlan_cb->opermode != RSI_WLAN_CLIENT_MODE)
  {
      //!command not supported 
      return RSI_ERROR_COMMAND_NOT_SUPPORTED;
  }
  else
  {
    //! if state is not in ipconfig done state
    if((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE))
    {
      //! Command given in wrong state 
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  if((clientid == NULL)||(server_ip == NULL)||(!client_port)||(!server_port))
  {
    return RSI_ERROR_INVALID_PARAM;

  }
  //! If any of the parameter is valid and exceeds max allowed length then return error
  if((rsi_strlen(clientid) > (RSI_EMB_MQTT_CLIENTID_MAX_LEN - RSI_LENGTH_ADJ))
      ||((username != NULL) && (rsi_strlen(username) > (RSI_EMB_MQTT_USERNAME_MAX_LEN - RSI_LENGTH_ADJ)))
      ||((password != NULL) && (rsi_strlen(password) > (RSI_EMB_MQTT_PASSWORD_MAX_LEN - RSI_LENGTH_ADJ)))
      )
  {
    return RSI_ERROR_PARAMTER_LENGTH_EXCEEDS_MAX_VAL;
  }
 
  if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
  { 

    //! allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    //! If allocation of packet fails
    if(pkt == NULL)
    {
      //!Changing the nwk state to allow 
      rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 
      //! return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;

    }

    mqtt_ops =  (rsi_emb_mqtt_client_init_t *)pkt->data;
    
    //!memset
    memset(mqtt_ops, 0, sizeof(rsi_emb_mqtt_client_init_t));

    rsi_uint32_to_4bytes(mqtt_ops->command_type, RSI_EMB_MQTT_CLIENT_INIT);

    //! Copy MQTT Server port
    rsi_uint32_to_4bytes(mqtt_ops->server_port,server_port);
    
    //! Copy MQTT Client port
    rsi_uint32_to_4bytes(mqtt_ops->client_port,client_port);
   
    //! Copy MQTT Keep Alive period
    rsi_uint16_to_2bytes(mqtt_ops->keep_alive_interval,keep_alive_interval);
   

    
  if(!(flags & RSI_EMB_MQTT_IPV6_ENABLE))
  {
    //! fill IP version
    rsi_uint32_to_4bytes(mqtt_ops->server_ip.ip_version, RSI_IP_VERSION_4);
    //! Fill IP address
    memcpy(mqtt_ops->server_ip.server_ip_address.ipv4_address, server_ip, RSI_IPV4_ADDRESS_LENGTH);
  }
  else
  {
    //! fill IP version
    rsi_uint32_to_4bytes(mqtt_ops->server_ip.ip_version, RSI_IP_VERSION_6);
    //! Fill IPv6 address
    memcpy(mqtt_ops->server_ip.server_ip_address.ipv6_address, server_ip, RSI_IPV6_ADDRESS_LENGTH);
  }

    //! Copy Client id length
    mqtt_ops->clientID_len = rsi_strlen(clientid);
    
    //! Copy Client ID 
    rsi_strcpy(&mqtt_ops->client_id,clientid);
    
    //! Copy useranme length
    mqtt_ops->username_len = rsi_strlen(username);
    
    if(username)
    {
      //! copy Username if username is not NULL
      rsi_strcpy(&mqtt_ops->user_name,username);
    }
    //! Copy password length
    mqtt_ops->password_len = rsi_strlen(password);
    if(password)
    {
      //! copy Password if password is not NULL
      rsi_strcpy(&mqtt_ops->password,password);
    }


    if(flags & RSI_EMB_MQTT_CLEAN_SESSION)
    {
      mqtt_ops->clean =  1;
    }
    if(flags & RSI_EMB_MQTT_SSL_ENABLE)
    {
      rsi_wlan_cb_non_rom->emb_mqtt_ssl_enable = 1;
      
      mqtt_ops->encrypt = 1;
    }

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    //! send set FTP Create command 
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EMB_MQTT_CLIENT, pkt);

    //! wait on nwk semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_EMB_MQTT_RESPONSE_WAIT_TIME); 

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


  //! Return the status if error in sending command occurs
  return status;







}


/*==============================================*/
/**
 * @fn          int32_t rsi_emb_mqtt_connect( uint8_t mqtt_flags,int8_t *will_topic, int8_t *will_message)
 * @brief       connects to MQTT broker  
 * @param[in]   mqtt_flags, Network flags,Each bit has its own significance
 *              Bit(0) -IP version 
 *                 0 - IPv4 
 *                 1 - IPv6 
 *              Bit(1) - Enable SSL
 *                 0 - Disable SSL
 *                 1 - Enable SSL
 * @param[in]   client_id , clientID of the MQTT Client and should be unique for each device   
 * @param[in]   username , Username for the login credentials of MQTT server   
 * @param[in]   password , password for the login credentials of MQTT server   
 * @return      int32_t 
 *              0  =  success
 *              <0 = failure 
 * @section description
 * This API establishes TCP connection with the given MQTT client port and establishes
 * MQTT protocol level connection
 *
 */

int32_t rsi_emb_mqtt_connect( uint8_t mqtt_flags,int8_t *will_topic, uint16_t will_message_len, int8_t *will_message)
{

  int32_t  status = RSI_SUCCESS;

  rsi_pkt_t       *pkt;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_emb_mqtt_connect_t *mqtt_ops = NULL;

  if(wlan_cb->opermode != RSI_WLAN_CLIENT_MODE )
  {
      //!command not supported 
      return RSI_ERROR_COMMAND_NOT_SUPPORTED;
  }
  else
  {
    //! if state is not in ipconfig done state
    if((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE))
    {
      //! Command given in wrong state 
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
  { 

    //! allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    //! If allocation of packet fails
    if(pkt == NULL)
    {
      //!Changing the nwk state to allow 
      rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 
      //! return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;

    }

    mqtt_ops =  (rsi_emb_mqtt_connect_t *)pkt->data;
    
    //!memset
    memset(mqtt_ops, 0, sizeof(rsi_emb_mqtt_connect_t));
    
    rsi_uint32_to_4bytes(mqtt_ops->command_type, RSI_EMB_MQTT_CONNECT);

    if(mqtt_flags & RSI_EMB_MQTT_USER_FLAG)
    {
      mqtt_ops->usrFlag = 1;
    }
    if(mqtt_flags & RSI_EMB_MQTT_PWD_FLAG)
    {
      mqtt_ops->pwdFlag = 1;
    }
    //! Will Messages are not supported


#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    //! send set mqtt connect command 
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EMB_MQTT_CLIENT, pkt);

    //! wait on nwk semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_EMB_MQTT_RESPONSE_WAIT_TIME); 

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


  //! Return the status if error in sending command occurs
  return status;






}






/*==============================================*/
/**
 * @fn         int32_t rsi_emb_mqtt_publish(int8_t *topic, rsi_mqtt_pubmsg_t *publish_msg)
 * @brief      publishes the message given on the topic specified 
 * @param[in]  topic, string of topic 
 * @param[in]  message strcuture, message to publish 
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * The API is used to publish the given message on the given topic
 *
 *
 */
int32_t rsi_emb_mqtt_publish(int8_t *topic, rsi_mqtt_pubmsg_t *publish_msg)
{
  int32_t  status = RSI_SUCCESS;

  rsi_pkt_t       *pkt;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_emb_mqtt_snd_pub_t *mqtt_ops = NULL;

  uint16_t max_payload_size;

  if(wlan_cb->opermode != RSI_WLAN_CLIENT_MODE)
  {
      //!command not supported 
      return RSI_ERROR_COMMAND_NOT_SUPPORTED;
  }
  else
  {
    //! if state is not in ipconfig done state
    if((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE))
    {
      //! Command given in wrong state 
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }


  if((topic == NULL)||(publish_msg == NULL)||(publish_msg->qos > 2) )
  {
    return RSI_ERROR_INVALID_PARAM;

  }

  if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
  { 

    //! allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    //! If allocation of packet fails
    if(pkt == NULL)
    {
      //!Changing the nwk state to allow 
      rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 
      //! return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;

    }

    mqtt_ops =  (rsi_emb_mqtt_snd_pub_t *)pkt->data;

    //!memset
    memset(mqtt_ops, 0, sizeof(rsi_emb_mqtt_snd_pub_t));

    //! length of TOPIC
    mqtt_ops->topic_len = rsi_strlen(topic);

    //! Strlen 
    if(mqtt_ops->topic_len > (RSI_EMB_MQTT_TOPIC_MAX_LEN - RSI_LENGTH_ADJ))
    {
      return RSI_ERROR_PARAMTER_LENGTH_EXCEEDS_MAX_VAL;
    }

    if(rsi_wlan_cb_non_rom->emb_mqtt_ssl_enable)
    {
      max_payload_size  = RSI_EMB_MQTT_SSL_PUB_MAX_LEN; 
    }
    else
    {
      max_payload_size  = RSI_EMB_MQTT_PUB_MAX_LEN; 
    }

    if(max_payload_size < rsi_cal_mqtt_packet_len((2 + mqtt_ops->topic_len + publish_msg->payloadlen)))
    {
      return RSI_ERROR_PARAMTER_LENGTH_EXCEEDS_MAX_VAL;
    }

    rsi_uint32_to_4bytes(mqtt_ops->command_type, RSI_EMB_MQTT_SND_PUB_PKT);

    //! copying TOPIC
    rsi_strcpy(&mqtt_ops->topic,topic);

    mqtt_ops->qos = publish_msg->qos;

    mqtt_ops->retained = publish_msg->retained;

    mqtt_ops->dup = publish_msg->dup;

    rsi_uint16_to_2bytes(mqtt_ops->msg_len,publish_msg->payloadlen);


     mqtt_ops->msg = (int8_t *)(pkt->data + sizeof(rsi_emb_mqtt_snd_pub_t));
    if(publish_msg->payloadlen)
    {
  
      memcpy(mqtt_ops->msg,publish_msg->payload,publish_msg->payloadlen);
    }
#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    //! send set FTP Create command 
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EMB_MQTT_CLIENT, pkt);

    //! wait on nwk semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_EMB_MQTT_RESPONSE_WAIT_TIME); 

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


  //! Return the status if error in sending command occurs
  return status;
}

/*==============================================*/
/**
 * @fn         int32_t rsi_emb_mqtt_subscribe(uint8_t qos, int8_t *topic)
 * @brief      Subscribes to the topic given 
 * @param[in]  rsi_mqtt_client, MQTT client structure info pointer which was returned in rsi_mqtt_client_init() API
 * @param[in]  qos, Quality of service of the message 
 * @param[in]  topic, topic string 
 * @return     int32_t
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This API is used to subscribe on the specified topic given.
 * If any other client posts any message on the same topic,
 * That message is received if MQTT client is subscribed to that
 * topic
 */
int32_t rsi_emb_mqtt_subscribe(uint8_t qos, int8_t *topic)
{
  int32_t  status = RSI_SUCCESS;

  rsi_pkt_t       *pkt;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_emb_mqtt_sub_t *mqtt_ops = NULL;

  if(wlan_cb->opermode != RSI_WLAN_CLIENT_MODE)
  {
      //!command not supported
      return RSI_ERROR_COMMAND_NOT_SUPPORTED;
  }
  else
  {
    //! if state is not in ipconfig done state
    if((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE))
    {
      //! Command given in wrong state 
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }


  if(topic == NULL)
  {
    return RSI_ERROR_INVALID_PARAM;

  }
  //! Strle`n 
  if(rsi_strlen(topic) > (RSI_EMB_MQTT_TOPIC_MAX_LEN - RSI_LENGTH_ADJ))
  {
    return RSI_ERROR_PARAMTER_LENGTH_EXCEEDS_MAX_VAL;
  }


  if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
  { 

    //! allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    //! If allocation of packet fails
    if(pkt == NULL)
    {
      //!Changing the nwk state to allow 
      rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 
      //! return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;

    }


    mqtt_ops =  (rsi_emb_mqtt_sub_t *)pkt->data;
    
    //!memset
    memset(mqtt_ops, 0, sizeof(rsi_emb_mqtt_sub_t));

    rsi_uint32_to_4bytes(mqtt_ops->command_type, RSI_EMB_MQTT_SUBSCRIBE);
    
    //! length of topic
    mqtt_ops->topic_len = rsi_strlen(topic);
    
    rsi_strcpy(&mqtt_ops->topic,topic);

    mqtt_ops->qos = qos; 

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    //! send set subscribe command 
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EMB_MQTT_CLIENT, pkt);

    //! wait on nwk semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_EMB_MQTT_RESPONSE_WAIT_TIME); 

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


  //! Return the status if error in sending command occurs
  return status;
}






/*==============================================*/
/**
 * @fn         int32_t rsi_emb_mqtt_unsubscribe(int8_t *topic)
 * @brief      Unsubscribes to the topic given 
 * @param[in]  topic, topic string 
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This API is used to unsubscribe on the specified topic given.
 * if unsubscribed, any messages on the topic is not received further
 */

int32_t rsi_emb_mqtt_unsubscribe(int8_t *topic)
{
  int32_t  status = RSI_SUCCESS;

  rsi_pkt_t       *pkt;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_emb_mqtt_unsub_t *mqtt_ops = NULL;

  if(wlan_cb->opermode != RSI_WLAN_CLIENT_MODE )
  {
      //!command not supported
      return RSI_ERROR_COMMAND_NOT_SUPPORTED;
  }
  else
  {
    //! if state is not in ipconfig done state
    if((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE))
    {
      //! Command given in wrong state 
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }
 
  
  if(topic == NULL)
  {
    return RSI_ERROR_INVALID_PARAM;

  }
 
  //! Strlen 
  if(rsi_strlen(topic) > (RSI_EMB_MQTT_TOPIC_MAX_LEN - RSI_LENGTH_ADJ))
  {
    return RSI_ERROR_PARAMTER_LENGTH_EXCEEDS_MAX_VAL;
  }


  if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
  { 

    //! allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    //! If allocation of packet fails
    if(pkt == NULL)
    {
      //!Changing the nwk state to allow 
      rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 
      //! return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;

    }

    
    mqtt_ops =  (rsi_emb_mqtt_unsub_t *)pkt->data;

    //!memset
    memset(mqtt_ops, 0, sizeof(rsi_emb_mqtt_unsub_t));

    rsi_uint32_to_4bytes(mqtt_ops->command_type, RSI_EMB_MQTT_UNSUBSCRIBE);

    //! length of TOPIC
    mqtt_ops->topic_len = rsi_strlen(topic);

    //!copying topic
    rsi_strcpy(&mqtt_ops->topic,topic);
#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    //! send mqtt cmd
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EMB_MQTT_CLIENT, pkt);

    //! wait on nwk semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_EMB_MQTT_RESPONSE_WAIT_TIME); 

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


  //! Return the status if error in sending command occurs
  return status;



}


/*==============================================*/
/**
 * @fn         int32_t rsi_emb_mqtt_disconnect()
 * @brief      Disconnect from MQTT cloud.Socket and MQTT level disconnection will be happening
 * @param[in]  void 
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This API is used to unsubscribe on the specified topic given.
 * if unsubscribed, any messages on the topic is not received further
 */

int32_t rsi_emb_mqtt_disconnect()
{
  int32_t  status = RSI_SUCCESS;

  rsi_pkt_t       *pkt;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_req_emb_mqtt_command_t *mqtt_ops = NULL;

  if(wlan_cb->opermode != RSI_WLAN_CLIENT_MODE)
  {
      //!command not supported
      return RSI_ERROR_COMMAND_NOT_SUPPORTED;
  }
  else
  {
    //! if state is not in ipconfig done state
    if((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE))
    {
      //! Command given in wrong state 
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
  { 

    //! allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    //! If allocation of packet fails
    if(pkt == NULL)
    {
      //!Changing the nwk state to allow 
      rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 
      //! return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;

    }

    mqtt_ops =  (rsi_req_emb_mqtt_command_t *)pkt->data;

    rsi_uint32_to_4bytes(mqtt_ops->command_type, RSI_EMB_MQTT_DISCONNECT);

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    //! send set FTP Create command 
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EMB_MQTT_CLIENT, pkt);

    //! wait on nwk semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_EMB_MQTT_RESPONSE_WAIT_TIME); 

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


  //! Return the status if error in sending command occurs
  return status;


}


/*==============================================*/
/**
 * @fn         int32_t rsi_emb_mqtt_destroy()
 * @brief      Destroy MQTT resources in firmware
 * @param[in]  void 
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This API is used to unsubscribe on the specified topic given.
 * if unsubscribed, any messages on the topic is not received further
 */

int32_t rsi_emb_mqtt_destroy()
{
  int32_t  status = RSI_SUCCESS;

  rsi_pkt_t       *pkt;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_req_emb_mqtt_command_t *mqtt_ops = NULL;

  if(wlan_cb->opermode != RSI_WLAN_CLIENT_MODE)
  {
      //!command Not supported
      return RSI_ERROR_COMMAND_NOT_SUPPORTED;
  }
  else
  {
    //! if state is not in ipconfig done state
    if((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE))
    {
      //! Command given in wrong state 
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS) 
  { 

    //! allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    //! If allocation of packet fails
    if(pkt == NULL)
    {
      //!Changing the nwk state to allow 
      rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 
      //! return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;

    }

    mqtt_ops =  (rsi_req_emb_mqtt_command_t *)pkt->data;

    rsi_uint32_to_4bytes(mqtt_ops->command_type, RSI_EMB_MQTT_COMMAND_DESTROY);

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    //! send set FTP Create command 
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EMB_MQTT_CLIENT, pkt);

    //! wait on nwk semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_EMB_MQTT_RESPONSE_WAIT_TIME); 

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


  //! Return the status if error in sending command occurs
  return status;


}

 /*==============================================*/
/**
 * @fn         int32_t rsi_emb_mqtt_register_call_back( uint32_t callback_id, void (*call_back_handler_ptr)(uint16_t status, uint8_t *buffer, const uint32_t length))
 * @brief      Register call backs for MQTT 
 * @param[in]  callback_id, Call back id for MQTT Responses
 *             
 * @param[in]  call_back_handler_ptr, call back function pointer
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This API is used to register call backs for MQTT Asynchronous messages
 */

int32_t rsi_emb_mqtt_register_call_back( uint32_t callback_id, void (*call_back_handler_ptr)(uint16_t status, uint8_t *buffer, const uint32_t length))
 {
  if(callback_id > RSI_MAX_NUM_CALLBACKS)
  {
    /*
     *Return , if the callback number exceeds the RSI_MAX_NUM_CALLBACKS ,or
     * the callback is already registered
     */
    return RSI_ERROR_EXCEEDS_MAX_CALLBACKS;
  }

  if(callback_id == RSI_WLAN_NWK_EMB_MQTT_REMOTE_TERMINATE_CB)
  {
    //! Remote terminate of Embedded mqtt socket call back handler
    rsi_wlan_cb_non_rom->nwk_callbacks.rsi_emb_mqtt_remote_terminate_handler = call_back_handler_ptr;
  }
  else if(callback_id == RSI_WLAN_NWK_EMB_MQTT_PUB_MSG_CB)
  {
    //!  MQTT Call back for publish message
    rsi_wlan_cb_non_rom->nwk_callbacks.rsi_emb_mqtt_publish_message_callback = call_back_handler_ptr;
  }
  else if(callback_id == RSI_WLAN_NWK_EMB_MQTT_KEEPALIVE_TIMEOUT_CB)
  {
    //! MQTT  keep alive timeout callback handler  
    rsi_wlan_cb_non_rom->nwk_callbacks.rsi_emb_mqtt_keep_alive_timeout_callback = call_back_handler_ptr;
  }
  else
  {
    return RSI_ERROR_INVALID_PARAM;
  }


  return RSI_SUCCESS;



 }




int32_t rsi_cal_mqtt_packet_len(int32_t rem_len)
{
	rem_len += 1; /* header byte */

	/* now remaining_length field */
	if (rem_len < 128)
		rem_len += 1;
	else if (rem_len < 16384)
		rem_len += 2;
	else if (rem_len < 2097151)
		rem_len += 3;
	else
		rem_len += 4;
	return rem_len;
}





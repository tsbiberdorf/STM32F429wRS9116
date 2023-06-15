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
#include "rsi_dhcp_user_class.h"

/*==============================================*/
/**
 * @fn         rsi_dhcp_user_class(uint8_t mode, uint8_t count, uint8_t *(usr_cls_arr)[count], void(*usr_cls_update_rsp_handler)(uint16_t status));
 * @brief      dhcp user class (option-77)
 * @param[in]  mode, DHCP User Class mode 
 * @param[in]  count, DHCP User Class count 
 * @param[in]  usr_cls_arr, Containing User class data
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This function is used to enable DHCP user class 
 *
 */

int32_t rsi_dhcp_user_class(uint8_t mode, uint8_t count, user_class_data_t *user_class_data, void(*dhcp_usr_cls_rsp_handler)(uint16_t status))
{
	rsi_dhcp_user_class_t       *user_class; 
	rsi_pkt_t                   *pkt;
	int32_t status   = RSI_SUCCESS;
	uint8_t i=0;

	//! Get wlan cb structure pointer
	rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

	//! if state is not in card ready received state
	if((wlan_cb->state < RSI_WLAN_STATE_CONNECTED))
	{
		//! Command given in wrong state 
		return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
	} 


  if(rsi_check_and_update_cmd_state(NWK_CMD,IN_USE) == RSI_SUCCESS)
  {
    if(dhcp_usr_cls_rsp_handler != NULL)
    {
      //! Register dhcp client user class response notify call back handler
      rsi_wlan_cb_non_rom->nwk_callbacks.rsi_dhcp_usr_cls_rsp_handler = dhcp_usr_cls_rsp_handler;
    }
    else
    {
      //!Changing the nwk state to allow 
      rsi_check_and_update_cmd_state(NWK_CMD,ALLOW); 
      //! return invalid command error
      return RSI_ERROR_INVALID_PARAM;
    }

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
  
  user_class = (rsi_dhcp_user_class_t *)pkt->data;

	//! memset the packet data
	memset(&pkt->data, 0, sizeof(rsi_dhcp_user_class_t));

  if(count > RSI_DHCP_USER_CLASS_MAX_COUNT)
  {
    count = RSI_DHCP_USER_CLASS_MAX_COUNT; 
  }
  //! Fill user class mode
	user_class->mode = mode;

	//! Fill user class count
	user_class->count = count;

	//! Fill user class data
	for(i=0; i < count; i++)
	{
    //! Check for Maximum user class data count

    //! Copy user class data
    user_class->user_class_data[i].length =  user_class_data[i].length;
    memcpy(&user_class->user_class_data[i].data[0], user_class_data[i].data, user_class_data[i].length);
	}

	//! send DHCP User Class command
	status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_DHCP_USER_CLASS, pkt);

  }
  else
  {
    //! return nwk command error
    return RSI_ERROR_NWK_CMD_IN_PROGRESS;
  }
	
  //! Return the status
	return status;

}

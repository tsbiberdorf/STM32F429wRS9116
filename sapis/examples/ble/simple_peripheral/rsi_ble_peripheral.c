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

/*================================================================================
 * @brief : This file contains example application for BLE Simple Peripheral
 * @section Description :
 * This application demonstrates how to configure the device in simple peripheral
 * mode and how to get connected from the remote Central device.
 =================================================================================*/

/**
 * Include files
 * */

//! BLE include file to refer BLE APIs
#include<rsi_ble_apis.h>
#include<rsi_ble_config.h>
#include<rsi_bt_common_apis.h>
#include<rsi_bt_common.h>

//! Common include file 
#include<rsi_common_apis.h>
#include "rsi_driver.h"
#include <string.h>

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif

#define RSI_BLE_LOCAL_NAME             "WYZBEE_PERIPHERAL"
#define RSI_SEL_ANTENNA                RSI_SEL_INTERNAL_ANTENNA//RSI_SEL_EXTERNAL_ANTENNA
#define RSI_SCAN_RESP_DATA					"REDPINE"

//! application events list
#define RSI_APP_EVENT_CONNECTED        1
#define RSI_APP_EVENT_DISCONNECTED     2

//! Memory length for driver
#define BT_GLOBAL_BUFF_LEN    15000

#ifdef RSI_WITH_OS
//! BLE task stack size
#define RSI_BT_TASK_STACK_SIZE  1000

//! BT task priority
#define RSI_BT_TASK_PRIORITY   1

//! Number of packet to send or receive
#define NUMBER_OF_PACKETS 1000

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY   2

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE  3000

void rsi_wireless_driver_task(void);

#endif

//! Memory to initialize driver
uint8_t global_buf[BT_GLOBAL_BUFF_LEN] = {0};

//! Application global parameters.
static rsi_bt_resp_get_local_name_t  rsi_app_resp_get_local_name = {0};
static uint8_t rsi_app_resp_get_dev_addr[RSI_DEV_ADDR_LEN] = {0};
static uint8_t rsi_app_resp_device_state = 0;
static int8_t rsi_app_resp_rssi = 0;
static uint8_t rsi_app_async_event_map = 0;
static rsi_ble_event_conn_status_t rsi_app_connected_device = {0};
static rsi_ble_event_disconnect_t rsi_app_disconnected_device = {0};

/*==============================================*/
/**
 * @fn         rsi_ble_app_init_events
 * @brief      initializes the event parameter.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used during BLE initialization.
 */
static void rsi_ble_app_init_events()
{
	rsi_app_async_event_map = 0;
	return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_set_event
 * @brief      sets the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
static void rsi_ble_app_set_event(uint32_t event_num)
{
	rsi_app_async_event_map |= BIT(event_num);
	return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_clear_event
 * @brief      clears the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
static void rsi_ble_app_clear_event(uint32_t event_num)
{
	rsi_app_async_event_map &= ~BIT(event_num);
	return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
static int32_t rsi_ble_app_get_event(void)
{
	uint32_t  ix;

	for (ix = 0; ix < 32; ix++)
	{
		if (rsi_app_async_event_map & (1 << ix))
		{
			return ix;
		}
	}

	return (RSI_FAILURE);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_peripheral_on_conn_status_event
 * @brief      invoked when connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_simple_peripheral_on_conn_status_event(rsi_ble_event_conn_status_t *resp_conn)
{
	memcpy(&rsi_app_connected_device, resp_conn, sizeof(rsi_ble_event_conn_status_t));
	rsi_ble_app_set_event (RSI_APP_EVENT_CONNECTED);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_peripheral_on_disconnect_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This callback function indicates disconnected device information and status
 */
void rsi_ble_simple_peripheral_on_disconnect_event(rsi_ble_event_disconnect_t *resp_disconnect, uint16_t reason)
{
	memcpy(&rsi_app_disconnected_device, resp_disconnect, sizeof(rsi_ble_event_disconnect_t));
	rsi_ble_app_set_event (RSI_APP_EVENT_DISCONNECTED);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_peripheral_on_enhance_conn_status_event
 * @brief      invoked when enhanced connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_simple_peripheral_on_enhance_conn_status_event(rsi_ble_event_enhance_conn_status_t *resp_enh_conn)
{
	rsi_app_connected_device.dev_addr_type = resp_enh_conn->dev_addr_type;
	memcpy (rsi_app_connected_device.dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
	rsi_app_connected_device.status = resp_enh_conn->status;
	rsi_ble_app_set_event (RSI_APP_EVENT_CONNECTED);
}

/*==============================================*/
/**
 * @fn         rsi_ble_peripheral
 * @brief      Tests the BLE GAP peripheral role.
 * @param[in]  none
  * @return    none.
 * @section description
 * This function is used to test the BLE peripheral role and simple GAP API's.
 */
int32_t rsi_ble_peripheral(void)
{
	int32_t status = 0;
	int32_t temp_event_map = 0;
	uint8_t remote_dev_addr[18] = {0};
	uint8_t adv[31] = {2,1,6};

#ifndef RSI_WITH_OS
	//! Driver initialization
	status = rsi_driver_init(global_buf, BT_GLOBAL_BUFF_LEN);
	if((status < 0) || (status > BT_GLOBAL_BUFF_LEN))
	{
		return status;
	}

	//! Redpine module intialisation
	status = rsi_device_init(LOAD_NWP_FW);
	if(status != RSI_SUCCESS)
	{
		LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%X\r\n",status);
		return status;
	}
	else
	{
		LOG_PRINT("\r\nDevice Initialization Success\r\n");
	}
#endif

	//! WC initialization
	status = rsi_wireless_init(0, RSI_OPERMODE_WLAN_BLE);
	if(status != RSI_SUCCESS)
	{
		LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%X\r\n",status);
		return status;
	}
	else
	{
		LOG_PRINT("\r\nWireless Initialization Success\r\n");
	}

	//! BLE register GAP callbacks
	rsi_ble_gap_register_callbacks(
			NULL,
			rsi_ble_simple_peripheral_on_conn_status_event,
			rsi_ble_simple_peripheral_on_disconnect_event,
			NULL,
			NULL,
			NULL,
			rsi_ble_simple_peripheral_on_enhance_conn_status_event,
			NULL,
			NULL,
			NULL);

	//! initialize the event map
	rsi_ble_app_init_events ();

	//! get the local device address(MAC address).
	status = rsi_bt_get_local_device_address(rsi_app_resp_get_dev_addr);
	if(status != RSI_SUCCESS)
	{
		LOG_PRINT("\r\nGet MAC Addr Failed, Error Code : 0x%X\r\n",status);
		return status;
	}
	else
	{
		LOG_PRINT("\r\nGet MAC Addr Success\r\n");
	}

	//! set the local device name
	status = rsi_bt_set_local_name(RSI_BLE_LOCAL_NAME);
	if(status != RSI_SUCCESS)
	{
		LOG_PRINT("\r\nSet Device Name Failed, Error Code : 0x%X\r\n",status);
		return status;
	}
	else
	{
		LOG_PRINT("\r\nSet Device Name Success\r\n");
	}

	//! get the local device name
	status = rsi_bt_get_local_name(&rsi_app_resp_get_local_name);
	if(status != RSI_SUCCESS)
	{
		LOG_PRINT("\r\nGet Device Name Failed, Error Code : 0x%X\r\n",status);
		return status;
	}
	else
	{
		LOG_PRINT("\r\nGet Device Name Success\r\n");
	}

	//! select/set the antenna type(internal/external)
	/*	status = rsi_bt_set_antenna(RSI_SEL_ANTENNA);
	if(status != RSI_SUCCESS)
	{
		return status;
	}
	 */
	//!preparing scan response data
	adv[3] = strlen (RSI_SCAN_RESP_DATA) + 1;
	adv[4] = 9;
	strcpy ((char *)&adv[5], RSI_SCAN_RESP_DATA);


	//!set scan response data
	rsi_ble_set_scan_response_data (adv, strlen (RSI_SCAN_RESP_DATA	) + 5);



	//! prepare advertise data //local/device name
	adv[3] = strlen (RSI_BLE_LOCAL_NAME) + 1;
	adv[4] = 9;
	strcpy ((char *)&adv[5], RSI_BLE_LOCAL_NAME);

	//! set advertise data
	rsi_ble_set_advertise_data (adv, strlen (RSI_BLE_LOCAL_NAME) + 5);


	//! start the advertising
	status = rsi_ble_start_advertising();
	if(status != RSI_SUCCESS)
	{
		LOG_PRINT("\r\nBLE Start Advertising Failed, Error Code : 0x%X\r\n",status);
		return status;
	}
	else
	{
		LOG_PRINT("\r\nBLE Start Advertising Success\r\n");
	}

	while(1)
	{
		//! Application main loop
#ifndef RSI_WITH_OS
		//! run the non OS scheduler
		rsi_wireless_driver_task();
#endif
		//! checking for received events
		temp_event_map = rsi_ble_app_get_event ();
		if (temp_event_map == RSI_FAILURE) {
			//! if events are not received loop will be continued.
			continue;
		}

		//! if any event is received, it will be served.
		switch(temp_event_map)
		{
		case RSI_APP_EVENT_CONNECTED:
		{
			//! remote device connected event

			//! clear the connected event.
			rsi_ble_app_clear_event (RSI_APP_EVENT_CONNECTED);

			//! converts the 6 byte MAC address to string format
			//rsi_6byte_dev_address_to_ascii ((int8_t *)remote_dev_addr, rsi_app_connected_device.dev_addr);

			//! get the RSSI value with connected remote device
			status = rsi_bt_get_rssi((uint8_t *)rsi_app_connected_device.dev_addr, &rsi_app_resp_rssi);
			if(status != RSI_SUCCESS)
			{
				LOG_PRINT("\r\nGet RSSI Failed, Error Code : 0x%X\r\n",status);
				return status;
			}

			//! get the local device state.
			status = rsi_ble_get_device_state(&rsi_app_resp_device_state);
			if(status != RSI_SUCCESS)
			{
				LOG_PRINT("\r\nGet Device State Failed, Error Code : 0x%X\r\n",status);
				return status;
			}
		}
		break;

		case RSI_APP_EVENT_DISCONNECTED:
		{
			//! remote device connected event

			//! clear the disconnected event.
			rsi_ble_app_clear_event (RSI_APP_EVENT_DISCONNECTED);

			//! get the local device state.
			status = rsi_ble_get_device_state(&rsi_app_resp_device_state);
			if(status != RSI_SUCCESS)
			{
				LOG_PRINT("\r\nGet Device State Failed, Error Code : 0x%X\r\n",status);
				return status;
			}

			//! set device in advertising mode.
			adv:
			status = rsi_ble_start_advertising ();
			if (status != RSI_SUCCESS)
			{
				LOG_PRINT("\r\nStart Advertising Failed, Error Code : 0x%X\r\n",status);
				goto adv;
			}
		}
		break;
		}
	}

}

/*==============================================*/
/**
 * @fn         main_loop
 * @brief      Schedules the Driver task.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function schedules the Driver task.
 */
void main_loop (void)
{
	while (1) {
		rsi_wireless_driver_task ();
	}
}

/*==============================================*/
/**
 * @fn         main
 * @brief      main function
 * @param[in]  none.
 * @return     none.
 * @section description
 * This is the main function to call Application
 */
int main (void)
{
	int32_t status;
#ifdef RSI_WITH_OS
	rsi_task_handle_t bt_task_handle = NULL;
	rsi_task_handle_t driver_task_handle = NULL;
#endif



#ifndef RSI_WITH_OS

	//Start BT Stack
	intialize_bt_stack(STACK_BTLE_MODE);

	//! Call BLE Peripheral application
	status = rsi_ble_peripheral();

	//! Application main loop
	main_loop();

	return status;
#endif
#ifdef RSI_WITH_OS
	//! Driver initialization
	status = rsi_driver_init(global_buf, BT_GLOBAL_BUFF_LEN);
	if((status < 0) || (status > BT_GLOBAL_BUFF_LEN))
	{
		return status;
	}
	//! Redpine module intialisation
	status = rsi_device_init(LOAD_NWP_FW);
	if(status != RSI_SUCCESS)
	{
		LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%X\r\n",status);
		return status;
	}
	else
	{
		LOG_PRINT("\r\nDevice Initialization Success\r\n");
	}

	//Start BT Stack
	intialize_bt_stack(STACK_BTLE_MODE);

	//! OS case
	//! Task created for BLE task
	rsi_task_create(rsi_ble_peripheral, "ble_task", RSI_BT_TASK_STACK_SIZE, NULL, RSI_BT_TASK_PRIORITY, &bt_task_handle);

	//! Task created for Driver task
	rsi_task_create(rsi_wireless_driver_task, "driver_task",RSI_DRIVER_TASK_STACK_SIZE, NULL, RSI_DRIVER_TASK_PRIORITY, &driver_task_handle);

	//! OS TAsk Start the scheduler
	rsi_start_os_scheduler();

	return status;
#endif

}

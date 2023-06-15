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
#if  (defined(RSI_BT_ENABLE)	|| defined(RSI_BLE_ENABLE))		

#ifndef RSI_BT_COMMON_H
#define RSI_BT_COMMON_H

#include<rsi_data_types.h>
#include<rsi_error.h>
#include<rsi_os.h>
#include<rsi_pkt_mgmt.h>
//#include"rsi_ant.h"
//#include<rsi_driver.h>

//#define  RSI_DEBUG_PRINTS
/******************************************************
 * *                      Macros
 * ******************************************************/
#define  RSI_BT_DUAL_MODE                 0x8
#define  RSI_BT_BLE_MODE_BITS             (BIT(2) | BIT(3))

#define RSI_BT_STATUS_OFFSET	            0x0C
#define RSI_BT_RSP_TYPE_OFFSET            0x02
#define RSI_BT_RSP_LEN_OFFSET             0x00
#define RSI_BT_RSP_LEN_MASK             0x0FFF

#define RSI_PROTO_BT_COMMON               0x01
#define RSI_PROTO_BT_CLASSIC              0x02
#define RSI_PROTO_BLE                     0x03
#define RSI_PROTO_ANT                     0x04

#define RSI_DEV_NAME_LEN                    50
#define RSI_DEV_ADDR_LEN                     6
#ifdef RSI_SDIO_INTERFACE
//! Adusting length of SDIO interface
#define RSI_DEV_ATT_LEN                    208
#else
#define RSI_DEV_ATT_LEN                    240
#endif

#define RSI_BT_CLASSIC_DEVICE             0x00
#define RSI_BT_LE_DEVICE                  0x01
#define RSI_BT_UNKWON_DEVICE              0xFF

#define STACK_BT_MODE                     0x01
#define STACK_BTLE_MODE                   0x02

//!RF types
#define RSI_EXTERNAL_RF                   0x00
#define RSI_INTERNAL_RF                   0x01
#define RSI_RF_TYPE                       RSI_INTERNAL_RF


#define  RSI_MAX_ATT                      5

#define  RSI_OPERMODE_WLAN_BT_DUAL_MODE   9

#define RSI_BT_STATE_NONE              0
#define RSI_BT_STATE_OPERMODE_DONE     1
/******************************************************
 * *                    Constants
 * ******************************************************/

/******************************************************
 * *                 Type Definitions
 * ******************************************************/

typedef struct rsi_ble_cb_s rsi_ble_cb_t;
typedef struct rsi_bt_classic_cb_s rsi_bt_classic_cb_t;
typedef struct rsi_bt_common_specific_cb_s rsi_bt_common_specific_cb_t;
typedef int32_t (*rsi_bt_get_ber_pkt_t)(uint8_t *pkt, uint16_t pkt_len);
typedef struct rsi_ant_cb_s rsi_ant_cb_t;

/******************************************************
 * *                   Enumerations
 * ******************************************************/

typedef enum rsi_bt_common_cmd_request_e
{
  RSI_BT_SET_LOCAL_NAME                      = 0x0001, 
  RSI_BT_GET_LOCAL_NAME                      = 0x0002, 
  RSI_BT_GET_RSSI                            = 0x0005,
  RSI_BT_GET_LOCAL_DEV_ADDR                  = 0x0007,
  RSI_BT_REQ_INIT			 			                 = 0x008D,
  RSI_BT_REQ_DEINIT			  			             = 0x008E,
  RSI_BT_SET_ANTENNA_SELECT                  = 0x008F,
  RSI_BT_REQ_PER_CMD                         = 0x009A,
  RSI_BT_SET_FEATURES_BITMAP                 = 0x00A6,
  RSI_BT_VENDOR_SPECIFIC                    = 0x00BE,
  RSI_BT_SET_ANTENNA_TX_POWER_LEVEL          = 0x00A7,
  RSI_BT_SET_BD_ADDR_REQ                     = 0x012E,
  RSI_BT_GET_BT_STACK_VERSION                = 0x012F,
	
	RSI_BLE_ONLY_OPER_MODE                     = 0x8010,
  RSI_BLE_REQ_PWRMODE                        = 0x8015,
	RSI_BLE_REQ_SOFTRESET                      = 0x801C
}rsi_bt_common_cmd_request_t;

typedef enum rsi_bt_common_event_e
{
  RSI_BT_EVENT_CARD_READY                    = 0x0505,
}rsi_bt_common_event_t;

typedef enum rsi_bt_common_cmd_resp_e
{
  RSI_BT_RSP_SET_LOCAL_NAME                      = 0x0001,
  RSI_BT_RSP_QUERY_LOCAL_NAME                    = 0x0002, 
  RSI_BT_RSP_QUERY_RSSI                          = 0x0005,
  RSI_BT_RSP_QUERY_LOCAL_BD_ADDRESS              = 0x0007,
  RSI_BT_RSP_INIT				 			                   = 0x008D,
  RSI_BT_RSP_DEINIT				  			               = 0x008E,
  RSI_BT_RSP_ANTENNA_SELECT                      = 0x008F,
  RSI_BT_RSP_SET_FEATURES_BITMAP                 = 0x00A6,
  RSI_BT_RSP_ANTENNA_TX_POWER_LEVEL              = 0x00A7,
  RSI_BT_RSP_SET_BD_ADDR                         = 0x012E,
	
	RSI_BLE_RSP_ONLY_OPER_MODE                     = 0x8010,
  RSI_BLE_RSP_PWRMODE                            = 0x8015,
	RSI_BLE_RSP_SOFTRESET                          = 0x801C
}rsi_bt_common_cmd_resp_t;

/******************************************************
 * *                    Structures
 * ******************************************************/
//! Driver control block
struct rsi_driver_cb_s;

//! Driver BT Common control block
struct rsi_bt_common_specific_cb_s
{
  //! PER BER Call Backs
  rsi_bt_get_ber_pkt_t rsi_bt_get_ber_pkt;
};

//! Specific BT, BLE blocks
typedef struct rsi_bt_global_cb_s
{
  //! BT Common specific cb
  rsi_bt_common_specific_cb_t *bt_common_specific_cb;
  //! BLE specific cb
  rsi_ble_cb_t *ble_specific_cb;
  //! BT Classic specific cb
  rsi_bt_classic_cb_t *bt_specific_cb;
  //! ANT specific cb
  rsi_ant_cb_t *ant_specific_cb;
}rsi_bt_global_cb_t;

//! Remote LE Device info structure
typedef struct rsi_remote_ble_info_s
{
   //! BD Address of the remote LE device
   uint8_t    remote_dev_bd_addr[RSI_DEV_ADDR_LEN];

   //! Address type of the remote LE device
   uint8_t    remote_dev_addr_type; 
   
   //! Available Buffer Count
   uint8_t    avail_buf_cnt;
 
   //! Max Buffer Count
   uint8_t    max_buf_cnt;
 
   //! Max Buffer Length
   uint16_t   max_buf_len;

   //! Flag for dev info used or not
   uint8_t    used;

   //! Flag for checking command in use
   uint8_t    cmd_in_use;

   //! Flag for checking expected remote response for each procedure
   uint16_t   expected_resp;

   //! Buffer config mode
   uint8_t   mode;
}rsi_remote_ble_info_t;

//! Driver BT/BLE/ANT control block
typedef struct rsi_bt_cb_s
{
  //! driver BT control block status
  volatile int32_t status;

  //! driver BT control block state
  uint16_t state;

  //! driver BT control block mutex
  rsi_mutex_handle_t bt_mutex;

  //! driver BT control block expected command response
  void *expected_response_buffer;

  //! expected command response type
  uint16_t expected_response_type;

  //! sync command flag to identify that the command is blocking / sync type
  uint8_t sync_rsp;

  //! BT device type at disconnect.
  uint8_t dev_type;

  //! driver BT control block semaphore
  rsi_semaphore_handle_t bt_cmd_sem;

  //! driver BT control block semaphore
  rsi_semaphore_handle_t bt_sem;

  //! driver BT control block tx packet pool
  rsi_pkt_pool_t  bt_tx_pool;

  //! buffer pointer given by application to driver
  uint8_t *app_buffer;

  //! buffer length given by application to driver
  uint32_t app_buffer_length;

  rsi_bt_global_cb_t *bt_global_cb;
	
  //!uint8, address of the device to which directed advertising has to be done in ll privacy mode
	uint8_t  directed_addr[RSI_DEV_ADDR_LEN];

  //! Structure Holding Remote LE Dev info (BD address & Controller Buffer availability)
  #define MAX_REMOTE_BLE_DEVICES        8
  rsi_remote_ble_info_t  remote_ble_info[MAX_REMOTE_BLE_DEVICES];

  //! Variable indicating buffer full/empty status --> 0 -> Empty, 1 -> Full
  uint8_t  buf_status;

  //! Variable indicating command in use status --> 0 -> Not In Use, 1 -> In Use
  uint8_t  cmd_status;

  //! Variable to save Remote info index
  uint8_t  remote_ble_index;
}rsi_bt_cb_t;

//! Set local name command structure
typedef struct rsi_bt_req_set_local_name_s
{
  //! uint8, length of the required name to be set
  uint8_t name_len;
  //! int8[50], required name
  int8_t name[RSI_DEV_NAME_LEN];
}rsi_bt_req_set_local_name_t;

//! Get RSSI command structure
typedef struct rsi_bt_get_rssi_s
{
  //! uint8, device address of the device whose RSSI has to be known
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
}rsi_bt_get_rssi_t;

//! Response structures

//! Get local name response structures
typedef struct rsi_bt_resp_get_local_name_s
{
  //! uint8, length of the required name to be set
  uint8_t name_len;
  //! int8[50], required name
  int8_t name[RSI_DEV_NAME_LEN];
}rsi_bt_resp_get_local_name_t;

//! Get Stack Version
typedef struct rsi_bt_resp_get_bt_stack_version_s
{
  //! stack version
  int8_t stack_version[10];
}rsi_bt_resp_get_bt_stack_version_t;

//! Set antenna structure
typedef struct rsi_ble_set_antenna_s
{
	//!uint8, antenna value - internal/external
	uint8_t value;
}rsi_ble_set_antenna_t;

//! Set feature bitmap structure
typedef struct rsi_bt_set_feature_bitmap_s
{
	//!uint8[4], features bits (bit map)
	uint32_t bit_map;
}rsi_bt_set_feature_bitmap_t;


typedef struct rsi_ble_oper_mode_s
{
	//!uint8, antenna value - internal/external
	uint32_t bt_features;
	uint8_t module_type;
}rsi_ble_oper_mode_t;

typedef struct rsi_ble_power_mode_s
{
	  //! power mode to set
  uint8_t   power_mode;                         

  //! set LP/ULP/ULP-without RAM retention
  uint8_t   ulp_mode_enable;              

}rsi_ble_power_mode_t;

//! Set antenna tx power level structure
typedef struct rsi_bt_set_antenna_tx_power_level_s
{
	//!uint8, protocol_mode : 1-BT, 2-LE
	uint8_t protocol_mode;
  //!int8_t, transmit power
  int8_t  tx_power;
}rsi_bt_set_antenna_tx_power_level_t;

//! BT PER Stats
typedef struct rsi_bt_per_stats_s
{
  uint16_t crc_fail_cnt;
  uint16_t crc_pass_cnt;
  uint16_t tx_abort_cnt;
  uint16_t rx_drop_cnt;
  uint16_t rx_cca_idle_cnt;
  uint16_t rx_start_idle_cnt;
  uint16_t rx_abrt_cnt;
  uint16_t tx_dones;
  int8_t rssi;
  uint16_t id_pkts_rcvd;
  uint16_t dummy[5];
}rsi_bt_per_stats_t;

typedef struct rsi_bt_per_cw_mode_s
{
        uint8_t cmd_id;
        //!uint8, cw_mode_enable : 1-Enable, 0-Disable
        uint8_t cw_mode_enable;
}rsi_bt_per_cw_mode_t;

typedef struct rsi_bt_set_local_bd_addr_s {
  uint8_t     dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_set_local_bd_addr_t;


//! BT BER CMD
typedef struct rsi_bt_ber_cmd_s
{
	uint8_t cmd_id;
	//!uint8, cm_mode_enable : 1-Enable, 0-Disable
	uint8_t enable;
}rsi_bt_ber_cmd_t;

/******************************************************
 * * BT/BLE common function declarations
 * ******************************************************/
void rsi_bt_set_status(rsi_bt_cb_t *bt_cb, int32_t status);
void rsi_bt_common_init(void);
void rsi_bt_common_tx_done(rsi_pkt_t *pkt);
int8_t rsi_bt_cb_init(rsi_bt_cb_t *bt_cb);
int32_t rsi_bt_driver_send_cmd(uint16_t cmd, void *cmd_struct, void *resp);
uint16_t rsi_bt_global_cb_init(struct rsi_driver_cb_s *driver_cb, uint8_t *buffer);
uint16_t rsi_driver_process_bt_resp_handler(rsi_pkt_t *pkt);
uint16_t rsi_bt_get_proto_type(uint16_t rsp_type, rsi_bt_cb_t **bt_cb);

int32_t rsi_bt_get_local_name(rsi_bt_resp_get_local_name_t *bt_resp_get_local_name);
int32_t rsi_bt_per_cw_mode(struct rsi_bt_per_cw_mode_s *bt_cw_mode);
#ifndef SAPIS_BT_STACK_ON_HOST
int32_t intialize_bt_stack(uint8_t mode);
#endif
#endif

#define HCI_BT_PER_STATS_CMD_ID           0x08
#define HCI_BT_CW_MODE_CMD_ID             0x0D
#define HCI_BLE_TRANSMIT_CMD_ID           0x13
#define HCI_BLE_RECEIVE_CMD_ID            0x14
#define HCI_BT_TRANSMIT_CMD_ID            0x15
#define HCI_BT_RECEIVE_CMD_ID             0x16

#endif

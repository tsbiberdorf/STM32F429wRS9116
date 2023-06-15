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

#ifndef RSI_ANT_BLE_H
#define RSI_ANT_BLE_H

#include<rsi_data_types.h>
#include<rsi_bt_common.h>
#include <rsi_ble_apis.h>
//! enumeration for ANT command request codes
typedef enum rsi_ant_cmd_request_e
{
  ANT_ATM_CMD                           = 0x0001,
  ANT_GET_VERSION                       = 0x0002,
  ANT_GET_STATS                         = 0x0003,
  ANT_GET_ACTIVITY_STATS                = 0x0004,
  ANT_PER_MODE_REQ										  = 0X0020,
	ANT_DATA_CONFIG_REQ										= 0x0023,
	ANT_ENCRYPTION_REQ										= 0x0024,
}rsi_ant_cmd_request_t;


//! enumeration for ANT command response codes
typedef enum rsi_ant_cmd_resp_e
{
  ANT_ATM_CMD_RESP                     = 0x0001,
  ANT_GET_VERSION_RESP                 = 0x0002,
  ANT_GET_STATS_RESP                   = 0x0003,
  ANT_GET_ACTIVITY_STATS_RESP          = 0x0004,
  ANT_PER_MODE_RESP										 = 0X0020,
	ANT_DATA_CONFIG_RESP 								 = 0x0023,
	ANT_ENCRYPTION_RESP									 = 0x0024,
}rsi_ant_cmd_resp_t;


#if 1 /* Used in ANT PER Commands */

//ANT Per Trasmit Data  cmd_ix = 0xE000
typedef struct  __attribute__((__packed__)) rsi_ant_tx_data_s {
uint8_t cmd_ix;
uint8_t ant_data_len;
uint8_t ant_data_payload[20];
}rsi_ant_tx_data_t;
//ANT Per ENCYPTION Data, cmd_ix = 0xE000
typedef struct  __attribute__((__packed__)) rsi_ant_encryption_data_s {
uint8_t cmd_ix;
uint8_t ant_enc_offset;
uint8_t ant_enc_key[16];
uint8_t ant_enc_ctr[16];
}rsi_ant_encryption_data_t;

//ANT Per mode Command, cmd_ix = 0xE000
typedef struct  __attribute__((__packed__)) rsi_ant_start_atm_s {
uint8_t cmd_ix;
	uint8_t enable;
	uint8_t dtm_mode;
	uint32_t mode_flags;
	uint8_t rf_channel;
	uint8_t tx_power;
	uint8_t  sync_word[4];
	uint16_t  pkt_length;
	uint32_t 	ant_mod_index;
}rsi_ant_start_atm_t;

//ANT Per mode Command, cmd_ix = 0xE000
typedef struct  __attribute__((__packed__))  rsi_ant_per_stats_s
{
	uint8_t cmd_ix;
	uint16_t  crc_fail;
	uint16_t  crc_pass;
	uint16_t  tx_aborts;
	uint16_t  cca_stk;
	uint16_t  cca_not_stk;
	uint16_t  fls_rx_start;
	uint16_t  cca_idle;
	uint16_t  tx_dones;
	uint16_t  rssi;
	uint16_t  id_pkts_rcvd;
}rsi_ant_per_stats_t;



#endif



//! enumeration for ANT Asyncronous Events
typedef enum rsi_ant_event_e
{
  RSI_ANT_SCHED_STATS                     = 0x1530,
  RSI_ANT_ACTIVITY_STATS                  = 0x1531,
}rsi_ant_event_t;

typedef struct rsi_ant_stack_cmd_s
{
    uint8_t data[256];
} rsi_ant_stack_cmd_t;

int32_t rsi_ant_send_cmd (void *ant_cmd, void *ant_cmd_resp);

typedef struct rsi_ant_atm_mode_s{
	uint8_t cmd_ix;
	uint8_t flags;
}rsi_ant_atm_mode_t;

//Ant get versions cmd structure
typedef struct rsi_ant_get_ver_s {
  uint8_t cmd_ix;
}rsi_ant_get_ver_t;

//Ant scheduling stats
typedef struct rsi_ant_schedule_stats_s {
  uint16_t reserved;
  uint16_t activities_blocked;
  uint16_t activities_aborted;
  uint16_t schedules_blocked;
  uint16_t radio_ops_complete;
  uint16_t schedules_attempted; 
}rsi_ant_schedule_stats_t;

//Ant activity stats
typedef struct rsi_ant_activity_stats_s {
  uint16_t reserved;
  uint16_t activity_status_ok;
  uint16_t activities_aborted; 
  uint16_t activity_status_too_late;
  uint16_t activity_status_no_sync;
  uint16_t activity_status_incomplete;
  uint16_t activity_status_bad_crc;
  uint16_t activity_status_vnd_error_start;
}rsi_ant_activity_stats_t;


/*==============================================*/
/**
 * @fn         rsi_ant_resp_handler_register_callbacks
 * @brief      post the ant rx async events to the application 
 * @param[in]  pointer to the data packet start address
 *  * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function registers the function pointers for GATT responses
 */
//Ant get stats cmd structure
typedef struct rsi_ant_get_stats_s {
  uint8_t cmd_ix;
}rsi_ant_get_stats_t;

typedef void (*rsi_ant_resp_handler_t) (uint8_t *data);
typedef void (*rsi_ant_data_request_callback_t) (void);

void rsi_ant_register_callbacks (
    rsi_ant_resp_handler_t   ant_async_resp_handler,
		rsi_ant_data_request_callback_t rsi_ant_data_request_callback);


//! Driver ANT control block
typedef struct rsi_ant_cb_s
{
  //ANT CBFC callbacks
  rsi_ant_resp_handler_t   ant_async_resp_handler;
	rsi_ant_data_request_callback_t rsi_ant_data_request_callback;
}rsi_ant_cb_t;

int32_t rsi_ant_driver_send_cmd(uint16_t cmd, void *cmd_struct, void *resp);
void rsi_ant_common_tx_done(rsi_pkt_t *pkt);
uint32_t rsi_ant_app(void);
void rsi_ant_app_task(void);


//! ANT protocol more data request threshold.
#define ANT_MORE_DATA_REQUEST_THRESHOLD				0x02
#define RSI_OPERMODE_ANT_GARDEN       				0x109

#endif

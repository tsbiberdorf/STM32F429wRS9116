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

#ifndef RSI_USER_H
#define RSI_USER_H
/******************************************************
 * *                      Macros
 * ******************************************************/

//! Si Ver 1.3
#define CHIP_VER_1P3                  0

//! Si Ver 1.4 and above
#define CHIP_VER_1P4_AND_ABOVE        1

//! To selcet Si ver, select either of one from among two macros
#define RS9116_SILICON_CHIP_VER       CHIP_VER_1P4_AND_ABOVE //! Set 1 for Silicon Chip Version 1p4 and Above, 0 for Silicon Chip Version 1p3

//! No of ltcp socktes
#define RSI_NUMBER_OF_LTCP_SOCKETS 2

//! Default number of sockets supported,max 10 sockets are supported
#define RSI_NUMBER_OF_SOCKETS   (2 + RSI_NUMBER_OF_LTCP_SOCKETS)

//! Default number of sockets supported,max 10 selects are supported
#define RSI_NUMBER_OF_SELECTS  4

//! Max packets in wlan control tx pool
#define RSI_WLAN_TX_POOL_PKT_COUNT 1

//! Max packets in bt common control tx pool
#define RSI_BT_COMMON_TX_POOL_PKT_COUNT 1

//! Max packets in bt classic control tx pool
#define RSI_BT_CLASSIC_TX_POOL_PKT_COUNT 1

//! Max packets in ble control tx pool
#define RSI_BLE_TX_POOL_PKT_COUNT 1

//! User configured MAX number of ANT TX PKTS pool COUNT.
#define ANT_TX_POOL_CONFIG_VAL			5

//! Max packets in ANT control tx pool
#define RSI_ANT_TX_POOL_PKT_COUNT ANT_TX_POOL_CONFIG_VAL

//! Max packets in wlan control tx pool
#define RSI_ZIGB_TX_POOL_PKT_COUNT 1

//! Max packets in common control tx pool
#define RSI_COMMON_TX_POOL_PKT_COUNT 1

//! Max packets 

#define RSI_DRIVER_RX_POOL_PKT_COUNT 1


//! Little Endian is enabled
#define RSI_LITTLE_ENDIAN      1 

//! Maximum scan results to be stored
#define MAX_SCAN_COUNT  48

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
#endif

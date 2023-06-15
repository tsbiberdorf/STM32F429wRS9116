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

#ifndef RSI_IAP_H
#define RSI_IAP_H
/******************************************************
 * *                      Macros
 * ******************************************************/
#define RSI_MAX_DIGEST_LENGTH   100
/******************************************************/

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/

/******************************************************
 * *                 Type Definitions
 * ******************************************************/


//! Structure of MFI Authentication create request
typedef struct rsi_mfi_auth_create_request_s
{
  //! Input Digest length from the host 
  uint8_t digest_length[4];

  //! Digest to give to the IAP chip
  uint8_t digest[RSI_MAX_DIGEST_LENGTH];

}rsi_mfi_auth_create_request_t;


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

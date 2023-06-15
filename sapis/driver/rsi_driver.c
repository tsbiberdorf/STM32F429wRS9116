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

/**
 * Includes
 * */

#include "rsi_driver.h"
/*===================================================*/
/**
 * @fn			void rsi_interrupt_handler(void)
 * @brief		interrupt handler to handle interrupt coming from module
 * @param[in]	none
 * @param[out]  none
 * @return	    none
 * @description This API is the call back for handling packet pedning interrupt from module
 */

void  rsi_interrupt_handler(void)
{
#ifndef RSI_M4_INTERFACE
  //! Disable the interrupt 
  rsi_hal_intr_mask();
#ifdef RSI_WITH_OS
  //! Set event RX pending from device
  rsi_set_event_from_isr(RSI_RX_EVENT);
#else
  //! Set event RX pending from device
  rsi_set_event(RSI_RX_EVENT);
#endif
#endif
  return;
}

void rsi_mask_ta_interrupt(void)
{
#ifdef RSI_M4_INTERFACE
	//! mask P2P interrupt
	mask_ta_interrupt(TX_PKT_TRANSFER_DONE_INTERRUPT  | RX_PKT_TRANSFER_DONE_INTERRUPT);
#else
	rsi_hal_intr_mask();
#endif
}

void rsi_unmask_ta_interrupt(void)
{
#ifdef RSI_M4_INTERFACE
	//! unmask P2P interrupt
	unmask_ta_interrupt(TX_PKT_TRANSFER_DONE_INTERRUPT  | RX_PKT_TRANSFER_DONE_INTERRUPT);
#else
	rsi_hal_intr_unmask();
#endif
}



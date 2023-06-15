/**
 * @file       rsi_hal_mcu_interrupt.c
 * @version    0.1
 * @date       18 sept 2015
 *
 * Copyright(C) Redpine Signals 2015
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief HAL INTERRUPT: Functions related to HAL Interrupts
 * 
 * @section Description
 * This file contains the list of functions for configuring the microcontroller interrupts.
 * Following are list of API's which need to be defined in this file.
 *
 */


/**
 * Includes
 */
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "rsi_driver.h"

typedef void (* UserIntCallBack_t)(void);

UserIntCallBack_t call_back;

extern uint8_t send_ping_for_keep_alive;
//extern TIM_HandleTypeDef htim2;
#if 1
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */
	if(call_back!=NULL)
	(*call_back)();
  /* USER CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */

  /* USER CODE END EXTI9_5_IRQn 1 */
}
#endif



/*===================================================*/
/**
 * @fn           void rsi_hal_intr_config(void (* rsi_interrupt_handler)())
 * @brief        Starts and enables the SPI interrupt
 * @param[in]    rsi_interrupt_handler() ,call back function to handle interrupt
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to initialize the register/pins
 *               related to interrupts and enable the interrupts.
 */
void rsi_hal_intr_config(void (* rsi_interrupt_handler)())
{
#ifdef RSI_SPI_INTERFACE
  	call_back = rsi_interrupt_handler;

  //! Configure interrupt pin/register in input mode and register the interrupt handler
  
  return;
#endif

}


/*===================================================*/
/** 
 * @fn           void rsi_hal_intr_mask(void)
 * @brief        Disables the SPI Interrupt
 * @param[in]    none
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to mask/disable interrupts.
 */
void rsi_hal_intr_mask(void)
{
#ifdef RSI_SPI_INTERFACE
  //! Mask/Disable the interrupt 
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
		return;
#endif

}


/*===================================================*/
/**
 * @fn           void rsi_hal_intr_unmask(void)
 * @brief        Enables the SPI interrupt
 * @param[in]    none  
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to enable interrupts.
 */
void rsi_hal_intr_unmask(void)
{
  
#ifdef RSI_SPI_INTERFACE
  //! Unmask/Enable the interrupt
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  return;
#endif
}



/*===================================================*/
/**
 * @fn           void rsi_hal_intr_clear(void)
 * @brief        Clears the pending interrupt
 * @param[in]    none
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to clear the handled interrupts.
 */
void rsi_hal_intr_clear(void)
{
   //! Clear the interrupt
   
   return;

 
}


/*===================================================*/
/**
 * @fn          uint8_t rsi_hal_intr_pin_status(void)
 * @brief       Checks the SPI interrupt at pin level
 * @param[in]   none  
 * @param[out]  uint8_t, interrupt status 
 * @return      none
 * @description This API is used to check interrupt pin status(pin level whether it is high/low).
 */	
uint8_t rsi_hal_intr_pin_status(void)
{
#ifdef RSI_SPI_INTERFACE
  volatile uint8_t status = 0;
 //! Gets the status of external interrupt GPIO pin
  status = rsi_hal_get_gpio(RSI_HAL_MODULE_INTERRUPT_PIN);

  //! Return interrupt pin  status(high(1) /low (0))
  return status;
#endif
}


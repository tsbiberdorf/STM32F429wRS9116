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
 * Include files
 * */
#include "rsi_driver.h"
#include "rsi_timer.h"
#include "rsi_hal.h"





/*==============================================*/
/**
 * @fn          void rsi_timer_expiry_interrupt_handler()
 * @brief       timer interrupt handler which is called for every milli second
 * @param[in]   none
 * @param[out]  none
 * @return      void
 *
 *
 * @section description
 * This function is called for every milli second and increments the timer counter
 *
 *
 */
void rsi_timer_expiry_interrupt_handler(void)
{
	rsi_driver_cb_non_rom->timer_counter++;
#ifdef RSI_SPI_INTERFACE
#ifdef RSI_HWTIMER
	rsi_driver_cb_non_rom->rsi_spi_hal->rsi_spiTimer2++;
	rsi_driver_cb_non_rom->rsi_spi_hal->rsi_spiTimer1++;
	rsi_driver_cb_non_rom->rsi_spi_hal->rsi_spiTimer3++;
#endif
#endif

}


/*==============================================*/
/**
 * @fn           uint32_t rsi_timer_read_counter()
 * @brief        This function returns the timer counter value
 * @param[in]    none
 * @param[out]   none
 * @return       uint32_t
 *               timer counter value
 *
 *
 * @section description
 * This function returns the timer counter value
 *
 *
 */
uint32_t rsi_timer_read_counter(void)
{
	return rsi_hal_gettickcount();
}




/*==============================================*/
/**
 * @fn            void rsi_init_timer(rsi_timer_instance_t *rsi_timer, uint32_t duration)
 * @brief         This function initialises the timer instance with the expiry time
 * @param[in]     rsi_timer, timer instance
 * @param[in]     duration, duration in milli seconds
 * @return        none
 *
 * @section description
 *  This function initialises the timer instance with the expiry time
 *
 *
 */
void rsi_init_timer(rsi_timer_instance_t *rsi_timer, uint32_t duration)
{
	*rsi_timer  =  rsi_timer_read_counter() + duration;
}






/*==============================================*/
/**
 * @fn           int32_t rsi_timer_expired(rsi_timer_instance_t *timer)
 * @brief        This function is used to check whether the timer instance is expired or not
 * @param[in]    rsi_timer, timer instance
 * @param[out]   none
 * @return       int32_t
 *               1 - if timer is expired
 *               0 - if timer is not expired
 *
 *
 * @section description
 * This function is used to check whether the timer instance is expired or not
 *
 *
 */

int32_t rsi_timer_expired(rsi_timer_instance_t *timer)
{
	long left = ((*timer )- rsi_timer_read_counter());

	if(left < 0)
		return 1;
	else
		return 0;

}









/*==============================================*/
/**
 * @fn           uint32_t rsi_timer_left(rsi_timer_instance_t *timer)
 * @brief        This function is used get the remaining time
 * @param[in]    rsi_timer, timer instance
 * @param[out]   none
 * @return       int32_t
 *               >0 - time left to expire
 *               0 - if timer is expired
 *
 *
 * @section description
 *  This function is used get the remaining time for timer expiry
 *
 *
 */
uint32_t rsi_timer_left(rsi_timer_instance_t *timer)
{
	long left = ((*timer )- rsi_timer_read_counter());
	return (left < 0) ? 0 : left;
}

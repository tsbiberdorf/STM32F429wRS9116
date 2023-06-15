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

#ifndef RSI_FREE_RTOS
#define RSI_FREE_RTOS
#ifdef RSI_WITH_OS
/**
 * Include files
 * */
#include <FreeRTOS.h>
#include <rsi_driver.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <projdefs.h>
#include <task.h>
#include "rsi_wlan_non_rom.h"
extern rsi_socket_info_non_rom_t *rsi_socket_pool_non_rom;


/*==============================================*/
/**
 * @fn          rsi_reg_flags_t rsi_critical_section_entry()
 * @brief       This API's to enter critical section
 * @param[in]   none,
 * @return      flags, interrupt status before entering critical section
 *
 *
 * @section description
 * This API disable interrupt to enter crtical section.
 *
 *
 */
rsi_reg_flags_t rsi_critical_section_entry()
{
	//! hold interrupt status before entering critical section
	//! disable interrupts
	rsi_reg_flags_t xflags;
	//! hold interrupt status before entering critical section
	//! disable interrupts
	xflags = 0;

  taskENTER_CRITICAL();

	//! return stored interrupt status
	return (xflags);
}



/*==============================================*/
/**
 * @fn         rsi_critical_section_exit(rsi_reg_flags_t xflags)
 * @brief      This API to exit critical section
 * @param[in]  xflags, interrupt status to restore interrupt on exit from critical section
 * @return     None
 *
 *
 * @section description
 * This API to exit/restore critical section.
 *
 *
 */

void  rsi_critical_section_exit(rsi_reg_flags_t xflags)
{
	//! restore interrupts while exiting critical section
  taskEXIT_CRITICAL();
  
}



/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_create(rsi_mutex_handle_t *mutex)
 * @brief        This API is creates the mutex 
 * @param[in]    mutex handle pointer  
 * @param[out]   Mutex created   
 * @return       0 = success 
 *              <0 = failure
 * @section description
 * This function is OS Abstraction layer API which creates the mutex 
 *
 *
 */

rsi_error_t rsi_mutex_create(rsi_mutex_handle_t *mutex)
{
	SemaphoreHandle_t *p_mutex = (SemaphoreHandle_t *)mutex;
  if(p_mutex == NULL) {
    return  RSI_ERROR_IN_OS_OPERATION;
  }

  *p_mutex = (SemaphoreHandle_t)xSemaphoreCreateMutex();

  if( *p_mutex == NULL) //!Note : FreeRTOS porting
  {
    return  RSI_ERROR_IN_OS_OPERATION;
  }

	return RSI_ERROR_NONE;
}



/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_lock(volatile rsi_mutex_handle_t *mutex)
 * @brief        This API is takes the mutex 
 * @param[in]    mutex handle pointer  
 * @return       0 = success 
 *              <0 = failure
 * @section description
 * This function is OS Abstraction layer API which takes the mutex
 *
 *
 */
rsi_error_t rsi_mutex_lock(volatile rsi_mutex_handle_t *mutex)
{
	uint32_t timeout_ms = 0;

	SemaphoreHandle_t *p_mutex = (SemaphoreHandle_t *)mutex;
	if(mutex == NULL || *p_mutex == NULL) //!Note : FreeRTOS porting
	{
		return RSI_ERROR_INVALID_PARAM;
	}
	if(!timeout_ms)
	{
		timeout_ms = portMAX_DELAY;
	}
	if(xSemaphoreTake(*p_mutex, timeout_ms) == pdPASS)
	{
		return RSI_ERROR_NONE;
	}
	return RSI_ERROR_IN_OS_OPERATION;


}


/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_unlock(volatile rsi_mutex_handle_t *mutex)
 * @brief        This API is gives the mutex 
 * @param[in]    mutex handle pointer  
 * @return       0 = success 
 *              <0 = failure
 * @section description
 * This function is OS Abstraction layer API which gives the mutex 
 *
 */
rsi_error_t rsi_mutex_unlock(volatile rsi_mutex_handle_t *mutex)
{
	SemaphoreHandle_t *p_mutex = (SemaphoreHandle_t *)mutex;
	if(mutex == NULL || *p_mutex == NULL) //!Note : FreeRTOS porting
	{
		return RSI_ERROR_INVALID_PARAM;
	}
	if(xSemaphoreGive(*p_mutex) == pdPASS)
	{
		return RSI_ERROR_NONE;
	}
	return RSI_ERROR_IN_OS_OPERATION;

}



/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_destroy(rsi_mutex_handle_t *mutex)
 * @brief        This API is destroyes the mutex 
 * @param[in]    mutex handle pointer  
 * @return       0 = success 
 *              <0 = failure
 * @section description
 * This function is OS Abstraction layer API which destroy/delete the mutex 
 *
 */
rsi_error_t rsi_mutex_destroy(rsi_mutex_handle_t *mutex)
{
	SemaphoreHandle_t *p_mutex = (SemaphoreHandle_t *)mutex;
	if(mutex == NULL || *p_mutex == NULL) //!Note : FreeRTOS porting
	{
		return RSI_ERROR_INVALID_PARAM;
	}
	vSemaphoreDelete(*p_mutex);

	return RSI_ERROR_NONE;

}



/*==============================================*/
/**
 * @fn           rsi_error_t rsi_semaphore_create(rsi_semaphore_handle_t *semaphore,uint32_t count)
 * @brief        This API is creates the semaphore instance 
 * @param[in]    Semaphore handle pointer  
 * @param[in]    resource count
 * @param[out]   Semaphore handler created   
 * @return       0 = success 
 *              <0 = failure
 * @section description
 * This function is OS Abstraction layer API which creates the semaphore 
 *
 */
rsi_error_t rsi_semaphore_create(rsi_semaphore_handle_t *semaphore,uint32_t count)
{
	SemaphoreHandle_t *p_semaphore = NULL;
	p_semaphore = (SemaphoreHandle_t *)semaphore;

	if(semaphore == NULL) {
		return  RSI_ERROR_IN_OS_OPERATION;
  }
	*p_semaphore = xSemaphoreCreateBinary();

	if(*p_semaphore == NULL)
	{
		return  RSI_ERROR_IN_OS_OPERATION;
	}
	return RSI_ERROR_NONE;

}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_semaphore_destroy(rsi_semaphore_handle_t *semaphore)
 * @brief        This API is destroys the semaphore instance
 * @param[in]    Semaphore handle pointer  
 * @return       0 = success 
 *              <0 = failure
 * @section description
 * This function is OS Abstraction layer API which destroys the semaphore 
 *
 */
rsi_error_t rsi_semaphore_destroy(rsi_semaphore_handle_t *semaphore)
{
	SemaphoreHandle_t *p_semaphore = NULL;
	p_semaphore = (SemaphoreHandle_t *)semaphore;
	
  if(semaphore == NULL || *p_semaphore == NULL) //!Note : FreeRTOS porting
	{
		return RSI_ERROR_INVALID_PARAM;
	}
	vSemaphoreDelete(*p_semaphore);

	return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_semaphore_check_and_destroy(rsi_semaphore_handle_t *semaphore)
 * @brief        This API should check whether semaphore is created or not, It
 *               destroys the semaphore instance,if its created, otherwise should return success
 * @param[in]    Semaphore handle pointer  
 * @return       0 = success 
 *              <0 = failure
 * @section description
 * This function is OS Abstraction layer API which destroys the semaphore 
 *
 */

rsi_error_t rsi_semaphore_check_and_destroy(rsi_semaphore_handle_t *semaphore)
{
	SemaphoreHandle_t *p_semaphore = NULL;
	p_semaphore = (SemaphoreHandle_t *)semaphore;
	
  if(semaphore == NULL || *p_semaphore == NULL) //!Note : FreeRTOS porting
	{
		return RSI_ERROR_NONE; //! returning success since semaphore does not exists
	}
	vSemaphoreDelete(*p_semaphore);

	return RSI_ERROR_NONE;
}



/*==============================================*/
/**
 * @fn          rsi_error_t rsi_semaphore_wait(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms ) 
 * @brief       This API is used by wireless library to acquire or wait for semaphore.
 * @param[in]   Semaphore handle pointer  
 * @param[in]   Maximum time to wait to acquire semaphore. If timeout_ms is 0 then wait
                till acquire semaphore.
 * @return      0 = Success
 *              <0 = failure
 * @section description
 * This API is used by Wireless Library to acquire or wait for semaphore.
 *
 */

rsi_error_t rsi_semaphore_wait(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms )
{
	SemaphoreHandle_t *p_semaphore = NULL;
	p_semaphore = (SemaphoreHandle_t *)semaphore;
	
  if(semaphore == NULL || *p_semaphore == NULL) //!Note : FreeRTOS porting
	{
		return RSI_ERROR_INVALID_PARAM;
	}
	if(!timeout_ms)
	{
		timeout_ms = portMAX_DELAY;
	}
	if(xSemaphoreTake(*p_semaphore, timeout_ms) == pdPASS)
	{
		return RSI_ERROR_NONE;
	}
	return RSI_ERROR_IN_OS_OPERATION;

}


/*==============================================*/
/**
 * @fn          rsi_error_t rsi_semaphore_post(rsi_semaphore_handle_t *semaphore) 
 * @brief       This API is used by wireless library to release semaphore, which was acquired.
 * @param[in]   Semaphore handle pointer  
 * @param[in]   Maximum time to wait to acquire semaphore. If timeout_ms is 0 then wait
                till acquire semaphore.
 * @return      0 = Success
 *              <0 = failure
 * @section description
 * This API is used by Wireless Library to acquire or wait for semaphore.
 *
 */


rsi_error_t rsi_semaphore_post(rsi_semaphore_handle_t *semaphore)
{
	SemaphoreHandle_t *p_semaphore = NULL;
	p_semaphore = (SemaphoreHandle_t *)semaphore;
	
  if(semaphore == NULL || *p_semaphore == NULL) //!Note : FreeRTOS porting
	{
		return RSI_ERROR_INVALID_PARAM;
	}
	if(xSemaphoreGive(*p_semaphore) == pdPASS)
	{
		return RSI_ERROR_NONE;
	}
	return RSI_ERROR_IN_OS_OPERATION;

}

/*====================================================*/
/**
 * @fn          rsi_error_t rsi_semaphore_post_from_isr(rsi_semaphore_handle_t *semaphore)
 * @brief       This API is used by wireless library to release semaphore, which was acquired.
 * @param[in]   Semaphore handle pointer 
 * @return      
 *                0 = Success
 *              <0 = failure
 * @section description 
 * This API is used by Wireless Library to acquire or wait for semaphore.
 */

rsi_error_t rsi_semaphore_post_from_isr(rsi_semaphore_handle_t *semaphore)
{
	SemaphoreHandle_t *p_semaphore = NULL;
	p_semaphore = (SemaphoreHandle_t *)semaphore;
	
  if(semaphore == NULL || *p_semaphore == NULL) //!Note : FreeRTOS porting
	{
		return RSI_ERROR_INVALID_PARAM;
	}

	if(xSemaphoreGiveFromISR(*p_semaphore,NULL) == pdPASS)
	{
		return RSI_ERROR_NONE;
	}
	return RSI_ERROR_IN_OS_OPERATION;

}

/*==============================================*/
/**
 * @fn          rsi_error_t rsi_semaphore_reset(rsi_semaphore_handle_t *semaphore) 
 * @brief       This API is used by wireless library Wireless Library to the semaphore to initial state
 * @param[in]   Semaphore handle pointer  
 * @return      0 = Success
 *              <0 = failure
 * @section description
 * This API is used by Wireless Library to reset the semaphore.
 *
 */
rsi_error_t rsi_semaphore_reset(rsi_semaphore_handle_t *semaphore)
{
	SemaphoreHandle_t *p_semaphore = NULL;
	p_semaphore = (SemaphoreHandle_t *)semaphore;
	
  if(semaphore == NULL || *p_semaphore == NULL) //!Note : FreeRTOS porting
	{
		return RSI_ERROR_INVALID_PARAM;
	}
	vSemaphoreDelete(*p_semaphore);

	return RSI_ERROR_NONE;
}


/*==============================================*/
/**
 * @fn           rsi_error_t rsi_task_create( rsi_task_function_t task_function,uint8_t *task_name,
                 uint32_t stack_size, void *parameters,
                 uint32_t task_priority,rsi_task_handle_t  *task_handle)
 * @brief        This API is used to create different tasks in OS supported platforms 
 * @param[in]    Pointer to function to be executed by created thread. Prototype of the function
 * @param[in]    Name of the created task  
 * @param[in]    Stack size given to the created task  
 * @param[in]    Pointer to the parameters to be passed to task function
 * @param[in]    task priority 
 * @param[in]    task handle/instance created 
 * @return       0 = Success
 *              <0 = Failure
 * @section description
 * This API is used by Wireless Library to create platform specific OS
 * task/thread. 
 *
 *
 */


rsi_error_t rsi_task_create( rsi_task_function_t task_function,uint8_t *task_name,
		uint32_t stack_size, void *parameters,
		uint32_t task_priority, rsi_task_handle_t  *task_handle)
{
#ifdef SAPIS_BT_STACK_ON_HOST
	task_priority = 7;
#endif
	if(pdPASS == xTaskCreate( task_function, (char const*)task_name,
			stack_size,
			parameters,
			task_priority,
			(TaskHandle_t*)task_handle))
	{
		return RSI_ERROR_NONE;
	}

	return RSI_ERROR_IN_OS_OPERATION;
}


/*==============================================*/
/**
 * @fn          void rsi_task_destroy(rsi_task_handle_t *task_handle)
 * @brief       This function deletes the task created
 * @param[in]   Task handle/instance to be deleted
 * @return      None
 * @section description
 * This API is used to delete/destroy the task created
 *
 */


void rsi_task_destroy(rsi_task_handle_t *task_handle)
{
	vTaskDelete((TaskHandle_t ) task_handle);
}



/*==============================================*/
/**
 * @fn          void rsi_os_task_delay(uint32_t timeout_ms)
 * @brief       This function induces required delay in milli seconds
 * @param[in]   Expected delay in milli seconds
 * @return      None
 * @section description
 * This API is used to induce OS delay
 *
 */
void rsi_os_task_delay(uint32_t timeout_ms)
{
	vTaskDelay(timeout_ms);
}




/*==============================================*/
/**
 * @fn           void rsi_start_os_scheduler()
 * @brief        This function schedules the tasks created
 * @param[in]    None 
 * @return       None
 * @section description
 * This API Schedules the tasks created
 *
 */
void rsi_start_os_scheduler()
{
	vTaskStartScheduler();
}
/*==============================================*/
/**
 * @fn          int32_t rsi_get_error(void)
 * @brief       This sets the os error .
 * @param[in]   Error
 * @param[out]  None
 * @return      None
 *
 * @section description
 * This sets the os error.
 *
 */
void rsi_set_os_errno(int32_t error)
{

}

/*==============================================*/
/**
 * @fn          void rsi_task_notify_wait(uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, uint32_t timeout)
 * @brief       This function allows a task to wait with optional timeout
 * @param[in]   ulBitsToClearOnEntry - bits set here will be cleared in the task’s notification value on entry to the function.
 *              pulNotificationValue - used to pass out the task notification value and an optional paramter
 *              timeout - maximum amount of time the calling task should remain in blocked state
 * @return      None
 * @section description
 * This API is used to wait the task with optional timeout
 *
 */
rsi_base_type_t rsi_task_notify_wait( uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, uint32_t timeout)
{
	rsi_base_type_t xResult;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS( timeout );

	/* Wait to be notified of an interrupt. */
	xResult = xTaskNotifyWait( ulBitsToClearOnEntry,    /* Don't clear bits on entry. */
			ulBitsToClearOnExit,        /* Clear all bits on exit. */
			pulNotificationValue, /* Stores the notified value. */
			xMaxBlockTime );

	return xResult;
}

/*==============================================*/
/**
 * @fn          void rsi_task_notify(rsi_task_handle_t xTaskToNotify, uint32_t ulValue)
 * @brief       This function notification directly to a task
 * @param[in]   xTaskToNotify - The handle of the task to which the notification is being sent
 *              ulValue - How ulValue is used is dependent on the eNotifyAction value
 * @return      None
 * @section description
 * This API is used to notify to a task
 *
 */
rsi_base_type_t rsi_task_notify(rsi_task_handle_t xTaskToNotify, uint32_t ulValue)
{
	//! eAction - an enumerated type that specifies how to update the receiving task’s notification value
	eNotifyAction eAction = eSetBits;
	if(xTaskToNotify != NULL)
	{
		return xTaskNotify( xTaskToNotify, ulValue, (eNotifyAction)eAction );
	}
	else
	{
		return -1;
	}
}


/*==============================================*/
/**
 * @fn          void rsi_task_notify_from_isr(rsi_task_handle_t xTaskToNotify, uint32_t ulValue,rsi_base_type_t *pxHigherPriorityTaskWoken )
 * @brief       This function notification directly to a task and should be used in ISR only
 * @param[in]   xTaskToNotify - The handle of the task to which the notification is being sent
 *              ulValue - How ulValue is used is dependent on the eNotifyAction value
 * @return      None
 * @section description
 * This API is used to notify to a task
 *
 */
rsi_base_type_t rsi_task_notify_from_isr(rsi_task_handle_t xTaskToNotify, uint32_t ulValue, rsi_base_type_t *pxHigherPriorityTaskWoken )
{
	//! eAction - an enumerated type that specifies how to update the receiving task’s notification value
	eNotifyAction eAction = eSetBits;

	if(xTaskToNotify != NULL)
	{
	return xTaskNotifyFromISR( xTaskToNotify,
			ulValue,
			(eNotifyAction)eAction,
			pxHigherPriorityTaskWoken );
	}
	else
	{
		return -1;
	}
		
}



/*==============================================*/
/**
 * @fn          uint32_t rsi_os_task_notify_take( BaseType_t xClearCountOnExit, TickType_t xTicksToWait )
 * @brief       This function allows a task in wait in blocked state for its notification value >0
 * @param[in]   xClearCountOnExit - Based on this value calling task notification value will be decremented/zero
 *              xTicksToWait - maximum amount of time calling task should remain in blocked state
 * @return      None
 * @section description
 * This API is used block the task
 *
 */
uint32_t rsi_os_task_notify_take( BaseType_t xClearCountOnExit, TickType_t xTicksToWait )
{
	return ulTaskNotifyTake( xClearCountOnExit, xTicksToWait );
}


/*==============================================*/
/**
 * @fn          void rsi_os_task_notify_give(rsi_task_handle_t xTaskToNotify)
 * @brief       This function notification directly to a task
 * @param[in]   xTaskToNotify - Task handle to notify
 * @return      None
 * @section description
 * This API is used to notify to a task
 *
 */
BaseType_t rsi_os_task_notify_give( rsi_task_handle_t xTaskToNotify )
{
	if(xTaskToNotify != NULL)
	{
	return xTaskNotifyGive( xTaskToNotify );
	}
	else
	{
		return -1;
	}
}

/*==============================================*/
/**
 * @fn          int32_t rsi_get_error(void)
 * @brief       Returns wlan status
 * @param[in]   None
 * @param[out]  None
 * @return      wlan status
 *
 * @section description
 * This function returns wlan status
 *
 */
int32_t rsi_get_error(int32_t sockID)
{
  if(rsi_socket_pool_non_rom[sockID].socket_status == 0xFF87
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB38)
  {
    return RSI_ERROR_ECONNREFUSED; 
  }
  else if(rsi_socket_pool_non_rom[sockID].socket_status == 0x00D2)
  {
    return RSI_ERROR_ECONNABORTED; 
  }
  else if(rsi_socket_pool_non_rom[sockID].socket_status == 0xFF6C
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xFF6A
      || rsi_socket_pool_non_rom[sockID].socket_status == RSI_ERROR_RESPONSE_TIMEOUT
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xBBD6)
  {
    return RSI_ERROR_ETIMEDOUT; 
  }
  else if(rsi_socket_pool_non_rom[sockID].socket_status == 0x0021
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xFF82
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xFF74
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB27
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xFF80
      || rsi_socket_pool_non_rom[sockID].socket_status == 0x00D1)
  {
    return RSI_ERROR_EACCES; 
  }
  else if(rsi_socket_pool_non_rom[sockID].socket_status == 0x00D3
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xFFFE)
  {
    return RSI_ERROR_EPERM; 
  }
  else if(rsi_socket_pool_non_rom[sockID].socket_status == 0xFF7E
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xBBED
      || rsi_socket_pool_non_rom[sockID].socket_status == 0x003E
      || rsi_socket_pool_non_rom[sockID].socket_status == 0x0030
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB3E)
  {
    return RSI_ERROR_EMSGSIZE; 
  }
  else if(rsi_socket_pool_non_rom[sockID].socket_status == 0xFF86)
  {
    return RSI_ERROR_EBADF; 
  }
  else if(rsi_socket_pool_non_rom[sockID].socket_status == 0x0015)
  {
    return RSI_ERROR_EAGAIN; 
  }
  else if(rsi_socket_pool_non_rom[sockID].socket_status == 0xBB22
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB42)
  {
    return RSI_ERROR_EISCONN; 
  }
  else if(rsi_socket_pool_non_rom[sockID].socket_status == 0xFFFF
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB50)
  {
    return RSI_ERROR_ENOTCONN; 
  }
  else if(rsi_socket_pool_non_rom[sockID].socket_status == 0xBB33
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB23
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB45
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB46
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB50)
  {
    return RSI_ERROR_EADDRINUSE; 
  }
  else if(rsi_socket_pool_non_rom[sockID].socket_status == 0xBB36)
  {
    return RSI_ERROR_ENOTSOCK; 
  }
  else if(rsi_socket_pool_non_rom[sockID].socket_status == RSI_ERROR_PKT_ALLOCATION_FAILURE)
  {
    return RSI_ERROR_ENOMEM; 
  }
}



/*==============================================*/
/**n
 * @fn          void *rsi_malloc (uint32_t size)
 * @brief       Returns allocated memory starting address
 * @param[in]   required bytes in size
 * @param[out]  None
 * @return      Returns starting address of allocated memory
 *
 * @section description
 * This function allocates memory from the buffer which is maintained by freertos
 *
 */

void *rsi_malloc (uint32_t size)
{
	if(size == 0)
	{
		return NULL;
	}
	else
	{
		return pvPortMalloc(size);
	}
}

/*==============================================*/
/**
 * @fn          void rsi_free (void *ptr)
 * @brief       frees the memory pointed by 'ptr'
 * @param[in]   starting address of the memory to be freed
 * @param[out]  None
 * @return      None
 *
 * @section description
 * This function frees the memory pointed by 'ptr'
 *
 */
void rsi_free (void *ptr)
{
	if(ptr == NULL)
	{
		//! do nothing
	}
	else
	{
	 vPortFree(ptr);
	}
}

#endif
#endif

/**
 *****************************************************************************************************
 *
 * @file
 *	my_task_exp1_main.c
 *
 * @description 
 *   This file contains the main function of task experiment 1.
 *   
 * 
 * @version 1.01
 * @author Zhang Daqing
 * @date   2015-01-09
 *****************************************************************************************************
 */

#include <string.h>
#include <misc.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <os_global.h>
#include <mtasks.h>
#include <dev_usart.h>
#include <dev_led.h>
#include <dev_key.h>

/**
 ****************************************************************************************
 * @brief Initial MCU hardware devices
 * @param[in] None
 * @description
 *  This function will initialize NVIC, SystemTick,USART1 and GPIO.
 ****************************************************************************************
 */

static void prvSetupHardware( void )
{
	/* Set the Vector Table base address at 0x08000000 */
	NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x0 );
    
	//All is preemptive priority
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* Configure HCLK clock as SysTick clock source. */
	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );
   
	serial_init();
	rprintf("Init serial done\r\n");
	
	init_led(LED1|LED2|LED3);
	rprintf("Init led done \r\n");

	//init_key(LED1|LED2|LED3);
	kdev_init();
	rprintf("Init key done \r\n");

	rprintf("Hardware initialization OK\r\n");
}

/**
 ****************************************************************************************
 * @brief The main function
 * @param[in] None
 * @description
 *  The main function will initial hardware,create a task and run FreeRTOS.
 ****************************************************************************************
 */

int main(void)
{
	//Set NVIC,USART1,GPIO
	prvSetupHardware(); 
#if 1
	{
		char str[64] = {0};
		rprintf("input something,input q to exit\r\n");
		while(1) {		
			serial_gets(str);
			serial_puts("\r\n");
			serial_puts(str);
			serial_puts("\r\n");
			if(strcmp(str,"exit") == 0)
				break;
		}
	}
#endif
	
#if 0
	{
		uint16_t key_val = 0;
		rprintf("start key debug\r\n");
		for(;;)
		{
			key_val = read_key();
			if(key_val > 0) {			
				if(key_val == read_key()) {
					handle_key(key_val);
					key_val = 0;
				}
			}
		}
	}
#endif

#if 0
    //Create task
	xTaskCreate(vManagerTask,"ManagerTask",256,NULL,5,&ManagerTaskHandle);
	xTaskCreate(vTask_Key,"KEY_TASK",256,NULL,3,&KeyTaskHandle);
	xTaskCreate(vTask_Slave,"SLAVE_TASK",256,NULL,4,&SlaveTaskHandle);
	xTaskCreate(vTask_Master,"MASTER_TASK",256,NULL,4,&MasterTaskHandle);

    //Start task scheduler
    vTaskStartScheduler();
#endif
	//Forever Loop  
	while(1){};
}

/**
 ****************************************************************************************
 * @brief Application tick hook 
 * @param[in] None
 * @description
 *  
 ****************************************************************************************
 */
#if 0
void vApplicationTickHook( void )
{
//    static uint32_t app_base_tick=0;
//    app_base_tick++;
}

/*-----------------------------------------------------------*/
void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

/*-----------------------------------------------------------*/
void vApplicationIdleHook( void )
{
    static uint32_t idle_tick=0;
    //static const char *p_idle_str = "Idle hook running.\n";
    idle_tick++;
    if(idle_tick >0xfffff)
    {
        //vPrintString(p_idle_str);
        idle_tick =0;
    }
    
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}

/*-----------------------------------------------------------*/
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
#endif

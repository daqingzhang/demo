/*
 * INCLUDE FILES
 *****************************************************************
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stm32f10x.h"
#include "my_task_exp.h"
#include "dev_usart.h"
#include "dev_led.h"
#include "dev_key.h"

/*
 * LOCAL VARIBLE DEFINITIONS
 *****************************************************************
 */
xTaskHandle ManagerTaskHandle;
xTaskHandle SubTaskHandle;
xTaskHandle BuilderTaskHandle;
xTaskHandle SlaveTaskHandle;
xTaskHandle MasterTaskHandle;
xTaskHandle KeyTaskHandle;

/*
 * FUNCTION DEFINITIONS
 *****************************************************************
 */
 
void vPrintMessage(char *p_str)
{	
    #if OS_EN
    vTaskSuspendAll(); 
    #endif 
	
	rprintf(p_str);
	
	#if OS_EN
    xTaskResumeAll(); 
    #endif
}

/**
 ****************************************************************************************
 * @brief A normal task 
 * @param[in] pvParameters  Pointer that will be used as the parameter for the task
 * being created.  
 * @description
 * This task is created by another task and this sub task will delete self
 * after print a string.
 ****************************************************************************************
 */
void vSubTask(void *pvParameters)
{  
    for(;;)
    {
        vPrintMessage("Delete myself ...\r\n");
        vTaskDelete(SubTaskHandle);
    }
}
/**
 ****************************************************************************************
 * @brief A normal task 
 * @param[in] pvParameters  Pointer that will be used as the parameter for the task
 * being created.  
 * @description
 * This task will periodically create a sub task and print string.This task is a periodic
 * task.
 ****************************************************************************************
 */
void vTask_Builder(void *pvParameters)
{
    TickType_t period = 100;
    //Get current tick value
    TickType_t last_tick = xTaskGetTickCount();
    BaseType_t build_flag;
    //vTaskDelay(50);
    for(;;)
    {
        //Print string
        vPrintMessage("Build a new task\r\n");
        //Build a new task
        build_flag = xTaskCreate(vSubTask,"SUB_TASK",512,NULL,6,&SubTaskHandle);
        if(build_flag != pdPASS)
        {
            //Print string
            vPrintMessage("Build task failed\r\n");
			while(1);
        }
        //A periodic task
        vTaskDelayUntil(&last_tick,period);
    }
}
/**
 ****************************************************************************************
 * @brief A normal task 
 * @param[in] pvParameters  Pointer that will be used as the parameter for the task
 * being created.  
 * @description
 * This task will run in suspended state after print a string.This task can suspend self.
 ****************************************************************************************
 */
unsigned long long g_task_count[5] = {0};

void vTask_Slave(void *pvParameters)
{
	g_task_count[0] = 0;
    for(;;)
    {	
		g_task_count[0] += 1;
		
		//vPrintMessage("Slave task\r\n");
        light_on(LED2);
		vTaskDelay(5);
		light_off(LED2);
		vTaskDelay(5);
		//vPrintMessage("Sleeping ...\r\n");
        vTaskSuspend(NULL);
    }
}
/**
 ****************************************************************************************
 * @brief A normal task 
 * @param[in] pvParameters  Pointer that will be used as the parameter for the task
 * being created.  
 * @description
 * This task is used to check another task's running state and resume a suspended task.
 ****************************************************************************************
 */
void vTask_Master(void *pvParameters)
{
    eTaskState statue;
    g_task_count[1] = 0;
    for(;;)
    {
		g_task_count[1] += 1;
	
		light_on(LED1);
        vTaskDelay(2);
        light_off(LED1);
		vTaskDelay(2);
		//vPrintMessage("Master task\r\n");
        //If slave task suspened ,then resume it.        
        statue = eTaskGetState(SlaveTaskHandle);
        if(statue ==  eSuspended)
        {
            //vPrintMessage("Master wake up slave\r\n");
            vTaskResume(SlaveTaskHandle);      
        }
    }
}

int key2_callback(void *arg)
{
		rprintf("key2 callback done\n");
		return 0;
}
int key3_callback(void *arg)
{
		rprintf("key3 callback done\n");
		return 0;
}
int key4_callback(void *arg)
{
		rprintf("key4 callback done\n");
		return 0;
}

void vTask_Key(void *pvParameters)
{
	key_dev_t* kdev[3];
	key_handler_t* handler[3];
	// init key device
	kdev_init();
	// register key device
	kdev[0] = kdev_register(KEY2_ID,"key2",key2_callback,0);
	kdev[1] = kdev_register(KEY3_ID,"key3",key3_callback,0);
	kdev[2] = kdev_register(KEY4_ID,"key4",key4_callback,0);
	// add key device
	handler[0] = kdev_add(kdev[0]);
	handler[1] = kdev_add(kdev[1]);
	handler[2] = kdev_add(kdev[2]);

	handler[0] = handler[0];
	handler[1] = handler[1];
	handler[2] = handler[2];

	for(;;) {
		// start to process key event
		kdev_process();
	}
	// unregister key device
// 	kdev_unregister(kdev[0]);
// 	kdev_unregister(kdev[1]);
// 	kdev_unregister(kdev[2]);
}
/**
 ****************************************************************************************
 * @brief A normal task 
 * @param[in] pvParameters  Pointer that will be used as the parameter for the task
 * being created.  
 * @description
 * This task can change task priority and created several tasks. For avoiding vManagerTask
 * is preempted by other tasks, it will set a higher priority before new tasks are created.
 * After task creation finished, it will suspend self.
 ****************************************************************************************
 */
void vManagerTask(void *pvParameters)
{
    //uint32_t    task_dly_val =100;
    UBaseType_t old_priority;
    UBaseType_t current_priority;
    UBaseType_t highest_priority=8;
    
    old_priority = uxTaskPriorityGet(NULL);
    //Display highest priority
    vTaskPrioritySet(NULL,highest_priority);
    current_priority = uxTaskPriorityGet(NULL);
    
	rprintf("Manager task's priority: %d\r\n",current_priority);
#if 0
    xTaskCreate(vTask_Slave,"SLAVE_TASK",512,NULL,1,&SlaveTaskHandle);
	xTaskCreate(vTask_Master,"MASTER_TASK",512,NULL,1,&MasterTaskHandle);
	xTaskCreate(vTask_Builder,"BUILDER_TASK",512,NULL,1,&BuilderTaskHandle);
	xTaskCreate(vTask_Key,"KEY_TASK",512,NULL,1,&KeyTaskHandle);
#endif
    vTaskPrioritySet(NULL,old_priority);
    current_priority = uxTaskPriorityGet(NULL); 
	rprintf("manager task's priority: %d\r\n",current_priority);
		
    for(;;)
    {
        //Master task suspend self
		vPrintMessage("manager task is suspended\r\n");
        vTaskSuspend(NULL);
    }
}


/**
 *****************************************************************************************************
 *
 * @file my_task_exp1_app.h
 *
 * @description 
 *   The header file of task experiment 1.
 *   
 * @version 1.0
 * @author Zhang Daqing
 * @date   2015-01-09
 *****************************************************************************************************
 */
 
 /*
 * INCLUDE FILES
 *****************************************************************
 */
#ifndef __MY_TASK_EXP_APP_H
#define __MY_TASK_EXP_APP_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/*
 * GLOBAL VARIBLE DECLARATIONS
 *****************************************************************
 */
 
extern xTaskHandle ManagerTaskHandle;
extern xTaskHandle SubTaskHandle;
extern xTaskHandle BuilderTaskHandle;
extern xTaskHandle SlaveTaskHandle;
extern xTaskHandle MasterTaskHandle;
extern xTaskHandle KeyTaskHandle;
/*
 * FUNCTION DECLARATIONS
 *****************************************************************
 */
void vManagerTask(void *pvParameters);
void vTask_Builder(void *pvParameters);
void vTask_Master(void *pvParameters);
void vTask_Slave(void *pvParameters);
void vTask_Key(void *pvParameters);

#endif


/**
 *****************************************************************************************************
 *
 * @file os_port_usart1.c
 *
 * @description 
 *   The source file of usart1 port configuration.
 *   
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
 
#include <stdarg.h>
#include "misc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "os_port_usart.h"
#include "os_global.h"

#ifndef OS_EN
   #define OS_EN 1
#endif

#if OS_EN
   #include "FreeRTOS.h"
   #include "task.h"
   #include "queue.h"
#endif

#include <string.h>
#include "dev_usart.h"

/*
 * MACRO VARIBLE DEFINITIONS
 *****************************************************************
 */
 // USART1 is used to a public port for printing string 
#define PRINT_PORT USART1
 /*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************
 */
 usart_tag_t usart1_tag;
 /*
 * LOCAL VARIBLE DEFINITIONS
 *****************************************************************
 */
 
 /*
 * FUNCTION DEFINITIONS
 *****************************************************************
 */
 
/**
 ****************************************************************************************
 * @brief USART1 port configuration
 * @param[in] None.
 * @description
 *  This function is used to configure USART1 port and relative GPIO port. 
 *  USART1: 115200,8,N,1
 *  GPIO  : Tx Pin: PA09; Rx Pin: PA10
 ****************************************************************************************
 */
void USART1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef  USART_ClockInitStructure;
	
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;			// 时钟低电平活动
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;				// 时钟低电平
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;				// 时钟第二个边沿进行数据捕获
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;		// 最后一位数据的时钟脉冲不从SCLK输出
	/* Configure the USART1 synchronous paramters */
	USART_ClockInit(USART1, &USART_ClockInitStructure);					// 时钟参数初始化设置


	USART_Init(USART1, &USART_InitStructure); 
	USART_Cmd(USART1, ENABLE);
}

/**
 ****************************************************************************************
 * @brief NVIC configuration about USART1
 * @param[in] None.  
 * @description
 *  This function is used to configur USART1 IRQ.
 ****************************************************************************************
 */
void NVIC_USART1_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable the USARTy Interrupt */

	NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    //GROUP 4
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

/**
 ****************************************************************************************
 * @brief Print a string to port.
 * @param[in] p_str  Pointer to a string which will be printed out.  
 * @description
 *  This function is used to print a string( or message) to USART port.
 ****************************************************************************************
 */

//USART1 Interrupt Handler
void USART1_IRQHandler(void)
{
    //TXE =1
    if(USART1->SR & 0x80)
    {
        //send data
        USART1->DR = usart1_tag.tx_dat;
        //handle tx callback
        if(usart1_tag.tx_callback !=NULL)
        {
            usart1_tag.tx_callback();
        }
    }
    //RXNE = 1
    if(USART1->SR & 0x20)
    {
        //receive data
        usart1_tag.rx_dat = USART1->DR;
        //handle rx callback
        if(usart1_tag.rx_callback !=NULL)
        {
            usart1_tag.rx_callback();
        }
    }
    //Error occur
    if(USART1->SR & 0x0F)
    {
        //clear error flag
        usart1_tag.rx_dat = USART1->DR;
        usart1_tag.temp = USART1->SR;
    }
}



















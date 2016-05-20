/**
 *****************************************************************************************************
 *
 * @file os_port_usart1.h
 *
 * @description 
 *   The header file of usart1 port.
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
#ifndef __OS_PORT_USART_H
#define	__OS_PORT_USART_H

#include <stdio.h>
#include "stm32f10x.h"

/*
 * GLOBAL VARIBLE DECLARATIONS
 *****************************************************************
 */
 
typedef struct
{
     void (*rx_callback)(void);
     void (*tx_callback)(void);
     void (*err_callback)(void);
    
     uint16_t rx_dat;
     uint16_t tx_dat;    
     uint32_t temp;
      
}usart_tag_t;
 
extern usart_tag_t usart1_tag;
 
/*
 * FUNCTION DECLARATIONS
 *****************************************************************
 */
extern void  USART1_Config(void);
extern void  NVIC_USART1_Configuration(void);
extern void  vPrintMessage(const char *p_str);

#endif 

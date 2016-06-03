/*************************************************************************************
* Test-program for Olimex “STM32-H103”, header board for “STM32F103RBT6”.
* After program start green LED (LED_E) will blink.
*
* Program has to be compiled with optimizer setting "-O0".
* Otherwise delay via while-loop will not work correctly.
*************************************************************************************/
#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include <dev_usart.h>
#include <dev_led.h>
typedef USART_TypeDef usart_dev_t;

#if 0
#ifndef NULL
#define NULL 0
#endif


static int config_usart_nvic(usart_dev_t *usart)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	if(usart == USART1) {
		/* Enable the USART1 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;	 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		//GROUP 4
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	} else if(usart == USART2) {
		/* Enable the USART2 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQn;	 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		//GROUP 4
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	} else {
		return -1;
	}
	return 0;
}

static int config_usart_gpio(usart_dev_t *usart)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	if(usart == USART1) {	
		/* Configure USART & GPIO Clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
		/* Configure GPIO Pins */
		/* Configure USART1 Tx (PA.09) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);    
		/* Configure USART1 Rx (PA.10) as input floating */
		GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);		
	} else if(usart == USART2) {
		/* Configure USART & GPIO clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
		/* Configure GPIO Pins */
		/* Configure USART2 Tx (PA.02) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);    
		/* Configure USART2 Rx (PA.03) as input floating */
		GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);		
	} else {
		return -1;
	}
	return 0;
}

static int config_usart(usart_dev_t *usart,int br,int data,char parity,int stop)
{
	uint16_t databit,paritybit,stopbit;
	USART_InitTypeDef USART_InitStructure;

	if(usart == NULL)
		return -1;

	if(data == 8)
		databit = USART_WordLength_8b;
	else if(data == 9)
		databit = USART_WordLength_9b;
	else
		return -1;

	if((parity == 'N') || (parity == 'n'))
		paritybit = USART_Parity_No;
	else if((parity == 'O') || (parity == 'o'))
		paritybit = USART_Parity_Odd;
	else if((parity == 'E') || (parity == 'e'))
		paritybit = USART_Parity_Even;
	else
		return -1;

	if(stop == 1)
		stopbit = USART_StopBits_1;
	else if(stop == 2)
		stopbit = USART_StopBits_2;
	else
		return -1;

	USART_InitStructure.USART_BaudRate    = (uint16_t)br;
	USART_InitStructure.USART_WordLength  = databit;
	USART_InitStructure.USART_Parity      = paritybit;
	USART_InitStructure.USART_StopBits    = stopbit;
	USART_InitStructure.USART_Mode        = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	if(usart == USART1) {
		// enable peripheral clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		// configure peripheral
		USART_Init(USART1, &USART_InitStructure);
		// enable usart
		USART_Cmd(USART1, ENABLE);
	} else if(usart == USART2) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);		
		USART_Init(USART2, &USART_InitStructure); 
		USART_Cmd(USART2, ENABLE);
	} else {
		return -1;
	}
	return 0;
}

static void reset_usart(usart_dev_t *usart)
{
	USART_DeInit(usart);
}

static int init_usart(usart_dev_t *usart)
{
	// reset usart
	reset_usart(usart);
	// config nvic
	if(config_usart_nvic(usart))
		return -1;
	// config gpio pins
	if(config_usart_gpio(usart))
		return -1;
	// config usart baudrate,data,parity,stop bit
	if(config_usart(usart,19200,8,'N',1))
		return -1;
	return 0;
}

#if 0
static uint8_t getc_usart(usart_dev_t *usart)
{
	while(USART_GetFlagStatus(usart,USART_FLAG_RXNE) == RESET);
	return ((uint8_t)(USART_ReceiveData(usart)));
}
#endif

static void putc_usart(usart_dev_t *usart,char ch)
{
	while(USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);	
	USART_SendData(usart,ch);
}

static void prvSetupHardware( void )
{
	/* Set the Vector Table base address at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
    
	//All is preemptive priority
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* Configure HCLK clock as SysTick clock source. */
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
   
	serial_init();
	rprintf("Init serial done\r\n");
	
	init_led(LED1|LED2|LED3);
	rprintf("Init led done \r\n");

	//init_key(LED1|LED2|LED3);
	kdev_init();
	rprintf("Init key done \r\n");

	rprintf("Hardware initialization OK\r\n");
}
#endif

/**
 ****************************************************************************************
 * @brief The main function
 * @param[in] None
 * @description
 *  The main function will initial hardware,create a task and run FreeRTOS.
 ****************************************************************************************
 */

void key_usage(void);

int init_usart(usart_dev_t *usart);
#define DBG_USART USART2
int main(int argc, char *argv[])
{
	int led = LED1 | LED2 | LED3;
	int tick = 0;
	init_usart(DBG_USART);
//	serial_init();
	init_led(led);
	while(1) {
		rprintf("tick = %d\n",tick++);
		light_flash(led,0x1fffff);
	}
}

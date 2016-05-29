#include <hal_config.h>
#include <string.h>
#include <misc.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <dev_usart.h>
#include <dev_led.h>

#define LED_GPIOX 	GPIOC
#define LED1_PIN 	GPIO_Pin_3
#define LED2_PIN 	GPIO_Pin_1
#define LED3_PIN 	GPIO_Pin_13
#define LED_PIN_ALL 	(LED1_PIN | LED2_PIN | LED3_PIN)

void init_led(int led)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	int pin = 0;
	if(led & LED1)
		pin |= LED1_PIN;
	if(led & LED2)
		pin |= LED2_PIN;
	if(led & LED3)
		pin |= LED3_PIN;
	// Reset GPIO register
	//GPIO_DeInit(LED_GPIOX);
	// Configure APB clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	// Configure GPIO Pins output
	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_GPIOX,&GPIO_InitStructure);
	// All LED light off
	GPIO_WriteBit(LED_GPIOX,LED_PIN_ALL,Bit_SET);
}

void light_on(int led)
{
	if(led & LED1)
		GPIO_WriteBit(LED_GPIOX,LED1_PIN,Bit_RESET);
	if(led & LED2)
		GPIO_WriteBit(LED_GPIOX,LED2_PIN,Bit_RESET);
	if(led & LED3)
		GPIO_WriteBit(LED_GPIOX,LED3_PIN,Bit_RESET);
}

void light_off(int led)
{
	if(led & LED1)
		GPIO_WriteBit(LED_GPIOX,LED1_PIN,Bit_SET);
	if(led & LED2)
		GPIO_WriteBit(LED_GPIOX,LED2_PIN,Bit_SET);
	if(led & LED3)
		GPIO_WriteBit(LED_GPIOX,LED3_PIN,Bit_SET);
}

void light_flash(int led, unsigned int cycle)
{
	unsigned int i;

	light_on(led);
	for(i = cycle; i != 0;i--);

	light_off(led);
	for(i = cycle; i != 0;i--);
}

static led_dev_t led_dev_list[MAX_LED_DEV_NUM];

int led_dev_init(led_dev_t *dev)
{
	if(dev == NULL)
		return -1;
	if(dev->id < 0)
		return -1;
	init_led(dev->id);
	if(dev->state)
		light_on(dev->id);
	else
		light_off(dev->id);
	return 0;
}

int led_dev_light_on(led_dev_t *dev)
{
	if(dev == NULL)
		return -1;
	if(dev->id < 0)
		return -1;
	light_on(dev->id);
	dev->state = 1;
	return 0;
}

int led_dev_light_off(led_dev_t *dev)
{
	if(dev == NULL)
		return -1;
	if(dev->id < 0)
		return -1;
	light_off(dev->id);
	dev->state = 0;
	return 0;
}

int led_dev_get_state(led_dev_t *dev,int *state)
{
	if(dev == NULL)
		return -1;
	if(dev->id < 0)
		return -1;
	*state = dev->state;
	return 0;
}

int led_dev_get_name(led_dev_t *dev,char *str)
{
	if(dev == NULL)
		return -1;
	if(dev->id < 0)
		return -1;
	if(str == NULL)
		return -1;
	memcpy(str,dev->name,strlen(dev->name));
	return 0;
}

int led_dev_flash(led_dev_t *dev, unsigned int cycle)
{
	if(dev == NULL)
		return -1;
	if(dev->id < 0)
		return -1;
	light_flash(dev->id,cycle);
	return 0;
}
int led_dev_list_init(void)
{
	int i;
	for(i = 0; i < MAX_LED_DEV_NUM;i++) {
		led_dev_list[i].id = -1;
		led_dev_list[i].state = 0;
		memset(led_dev_list[i].name,0x0,MAX_LED_DEV_NAME_SIZE);
	}
	return 0;
}

int led_dev_unregister(led_dev_t *dev)
{
	if(dev == NULL)
		return -1;

	dev->id = -1;
	dev->state = 0;
	memset(dev->name,0,MAX_LED_DEV_NAME_SIZE);
	return 0;
}

led_dev_t *led_dev_register(int id, const char *name, int state)
{
	int i = 0;
	led_dev_t *ptr_dev = NULL;

	for( i = 0; i < MAX_LED_DEV_NUM; i++) {
		if(led_dev_list[i].id != -1) {
			ptr_dev = &(led_dev_list[i]);
			break;
		}
	}
	if(ptr_dev != NULL) {
		ptr_dev->id = id;
		ptr_dev->state = 0;
		if(name != NULL)
			memcpy((void *)name,ptr_dev->name,strlen(name));
	}
	return ptr_dev;
}

int led_dev_print(led_dev_t *dev)
{
	return 0;
}
int led_handler_init(led_dev_t *dev, led_handler_t *handler)
{
	if(handler != NULL) {
		handler->priv = dev;
		handler->init = led_dev_init;
		handler->on = led_dev_light_on;
		handler->off = led_dev_light_off;
		handler->get_state = led_dev_get_state;
		handler->get_name = led_dev_get_name;
		handler->flash = led_dev_flash;

		handler->init(dev);
		return 0;
	}
	return -1;
}

void led_usage(void)
{
	led_handler_t handler;
	led_dev_t *dev;
	int state = 0,ret = 0;
	char name[20];
	ret = ret;

	// initial device list
	led_dev_list_init();
	
	// reigster device
	dev = led_dev_register(LED1,"led1",0);
	
	// print device
	led_dev_print(dev);
	
	if(dev != NULL) {
		// set device handler
		led_handler_init(dev,&handler);
	}

	ret = handler.on(dev);
	ret = handler.off(dev);
	ret = handler.get_state(dev,&state);
	ret = handler.get_name(dev,name);

	// unregister device
	ret = led_dev_unregister(dev);
}

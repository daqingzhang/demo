#include <hal_config.h>
#include <string.h>
#include <misc.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <dev_key.h>

#ifdef CONFIG_DEBUG_KEY
#include <dev_usart.h>
#endif

/*
 ************************************************************
 *
 *			KEY HAL
 *
 ************************************************************
 */
 
#define KEY_GPIOX GPIOB
#define KEY_GPIOX_CLK	RCC_APB2Periph_GPIOB
#define KEY2_PIN GPIO_Pin_9
#define KEY3_PIN GPIO_Pin_8
#define KEY4_PIN GPIO_Pin_7

#define KEY_PIN_MASK (KEY2_PIN | KEY3_PIN | KEY4_PIN)

int init_key(int key)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t pin = 0;

	if(key & KEY2_ID)
		pin |= KEY2_PIN;
	if(key & KEY3_ID)
		pin |= KEY3_PIN;
	if(key & KEY4_ID)
		pin |= KEY4_PIN;
	else
		return -1;

	// Reset GPIO register
	GPIO_DeInit(KEY_GPIOX);

	// Configure APB2 clock for GPIOB
	RCC_APB2PeriphClockCmd(KEY_GPIOX_CLK,ENABLE);

	// Configure GPIO Pins input pull-up
	GPIO_InitStructure.GPIO_Pin 	= pin;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(KEY_GPIOX,&GPIO_InitStructure);

#ifdef CONFIG_DEBUG_KEY
	rprintf("GPIO register value,crl: 0x%x,crh: 0x%x\r\n",
			KEY_GPIOX->CRL,KEY_GPIOX->CRH);
#endif
	return 0;
}

uint32_t read_key(void)
{
	uint32_t temp = 0;
	temp = GPIO_ReadInputData(KEY_GPIOX);
	return((~temp) & KEY_PIN_MASK);
}

#if 0
void handle_key(unsigned int key_val)
{
	int id = 0;

	switch(key_val) {
	case KEY2_PIN:
		id = 2;
		break;
	case KEY3_PIN:
		id = 3;
		break;
	case KEY4_PIN:
		id = 4;
		break;
	default:
		id = -1;
		break;
	}

#ifdef CONFIG_DEBUG_KEY
	rprintf("Key %d is handled\r\n",id);
#endif
}
#endif

/*
 ************************************************************
 *
 *							KEY DEVICE
 *
 ************************************************************
 */

/*
 *	PRIVATE FUNCTION
 ************************************************************
 */

static key_dev_t key_dev_list[MAX_KEY_DEV_NUM];
static key_handler_t key_handler_list[MAX_KEY_DEV_NUM];

static int key_init(key_dev_t *dev)
{
	if(init_key(dev->id))
		return -1;
	return 0;
}

static int key_process(key_dev_t *dev)
{
	return(dev->callback(dev->arg));
}

static int key_clicked(key_dev_t *dev)
{
	uint32_t mask;

	switch(dev->id) {
	case KEY2_ID:
		mask = KEY2_PIN;
		break;
	case KEY3_ID:
		mask = KEY3_PIN;
		break;
	case KEY4_ID:
		mask = KEY4_PIN;
		break;
	default:
		return 0;
	}
	return (mask & read_key()) ? 1 : 0;
}

static int key_valid(key_dev_t *dev)
{
	return dev->valid;
}

static int key_get_name(key_dev_t *dev,char *name)
{
	if(name)
		memcpy(name,dev->name,sizeof(dev->name));
	return 0;
}

static int key_get_id(key_dev_t *dev)
{
	return (dev->id);
}

static int key_get_count(key_dev_t *dev)
{
	if(dev == NULL)
		return -1;
	return (dev->count);
}

/*
 *	PUBLIC FUNCTION
 ************************************************************
 */

void kdev_init(void)
{
	int i;
	for(i = 0;i < MAX_KEY_DEV_NUM;i++) {
		memset((void *)&key_dev_list[i],0,sizeof(key_dev_list[i]));
		key_dev_list[i].valid = 0;
	}
}

key_dev_t* kdev_register(int id,const char *name,
						int (*callback)(void *arg),void *arg)
{
	int i = 0,len = 0;
	key_dev_t *dev = NULL;

	if(!IS_KEY_VALID(id))
		return NULL;

	for(i = 0; i < MAX_KEY_DEV_NUM; i++) {
		if(key_dev_list[i].valid == 0) {
			dev = &key_dev_list[i];
			dev->id = id;
			dev->callback = callback;
			dev->arg = arg;
			len = strlen(name);
			if(len > MAX_KEY_DEV_NAME_SIZE)
				len = MAX_KEY_DEV_NAME_SIZE;
			memcpy(dev->name,name,len);
			dev->count = 0;
			dev->clicked = 0;
			dev->valid = 1;
			break;
		}
	}
	return dev;
}

int kdev_unregister(key_dev_t *dev)
{
	if(dev) {
		dev->valid = 0;
		dev->count = 0;
	}
	return 0;
}

key_handler_t* kdev_add(key_dev_t *dev)
{
	key_handler_t *handler;
	int idx;

	if(dev == NULL)
		return NULL;
	if(!IS_KEY_VALID(dev->id))
		return NULL;

	idx = (dev->id) >> 2;
	handler = &key_handler_list[idx];

	handler->priv 		= dev;
	handler->init 		= key_init;
	handler->clicked 	= key_clicked;
	handler->valid		= key_valid;
	handler->process 	= key_process;
	handler->get_id 	= key_get_id;
	handler->get_count 	= key_get_count;
	handler->get_name 	= key_get_name;

	handler->init(dev);
	return handler;
}

int kdev_process(void)
{
	key_dev_t *dev;
	key_handler_t* handler;
	int r = 0,i;

	for(i = 0;i < MAX_KEY_DEV_NUM;i++) {
			handler = &key_handler_list[i];
			if(!handler->priv)
				continue;
			dev = handler->priv;
			if(!handler->valid(dev))
				continue;
			if(!handler->clicked(dev))
				continue;
			r += handler->process(dev);
	}
	return r;
}

/*
 ************************************************************
 * 					Usage for Key Device
 *
 * This key usage is used to descript how to work with a key
 * device.
 *
 ************************************************************
 */
#if 1
static int g_key_dat[] = {KEY2_ID,KEY3_ID,KEY4_ID};

int key_callback(void *arg)
{
	int *p = (int *)(arg);
	switch(*p) {
	case KEY2_ID:
		rprintf("%s, key 2 is pressed down\n",__func__);
		break;
	case KEY3_ID:
		rprintf("%s, key 3 is pressed down\n",__func__);
		break;
	case KEY4_ID:
		rprintf("%s, key 4 is pressed down\n",__func__);
		break;
	default:
		rprintf("%s, key error, %d\n",__func__,*p);
		break;
	}

	return 0;
}

void key_usage(void)
{
	key_dev_t* kdev[3];
//	key_handler_t* handler[3];

	// init key device
	kdev_init();
	// register key device, add it
	kdev[0] = kdev_register(KEY2_ID,"key2",key_callback,&g_key_dat);
	kdev[1] = kdev_register(KEY3_ID,"key3",key_callback,&g_key_dat);
	kdev[2] = kdev_register(KEY4_ID,"key4",key_callback,&g_key_dat);
	kdev_add(kdev[0]);
	kdev_add(kdev[1]);
	kdev_add(kdev[2]);
	while(1) {
		// start to process key event
		kdev_process();
	}
	// unregister key device
	kdev_unregister(kdev[0]);
	kdev_unregister(kdev[1]);
	kdev_unregister(kdev[2]);
}
#endif

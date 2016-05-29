#include <string.h>
#include "hal_config.h"
#include "hal_device.h"

#include "stm32f10x.h"

#ifdef HAL_DEV_USART_EN
	#include "dev_usart.h"
#endif

#ifdef HAL_DEV_SPI_EN
	#include "dev_spi.h"
#endif

#ifdef HAL_DEV_I2C_EN
	#include "dev_i2c.h"
#endif

void init_device_handler(struct hal_dev_handler *handler)
{
	handler->device = NULL;
}
int register_device_handler(struct hal_dev_handler *handler, enum HAL_DEVICE_ID dev_id)
{
	if(!IS_VALID_DEVICE_ID(dev_id)) {
		return DEV_RET_ERROR;
	}
	handler->device_id = dev_id;
	switch(dev_id) {
	case DEV_USART1:
		handler->device = (hal_device_t *)(USART1);
		break;
	case DEV_USART2:
		handler->device = (hal_device_t *)(USART2);
		break;
	case DEV_SPI1:
		handler->device = (hal_device_t *)(SPI1);
		break;
	case DEV_SPI2:
		handler->device = (hal_device_t *)(SPI2);
		break;
	case DEV_I2C1:
		handler->device = (hal_device_t *)(I2C1);
		break;
	case DEV_I2C2:
		handler->device = (hal_device_t *)(I2C2);
		break;
	default:
		return DEV_RET_ERROR;
		//break;
	}
	if ((dev_id == DEV_USART1) || (dev_id == DEV_USART2)) {
#if HAL_DEV_USART_EN
		handler->init = init_usart;
		handler->reset = reset_usart;
		handler->read = read_usart;
		handler->write = write_usart;
		handler->config = config_usart;
		handler->set_speed = set_usart_speed;
#else
		return DEV_RET_ERROR;
#endif
	} else if((dev_id == DEV_SPI1) || (dev_id == DEV_SPI2)) {
#if HAL_DEV_SPI1_EN
		handler->init = init_spi;
		handler->reset = reset_spi;
		handler->read = read_spi;
		handler->write = write_spi;
		handler->config = config_spi;
#else
		return DEV_RET_ERROR;
#endif
	} else if((dev_id == DEV_I2C1) || (dev_id == DEV_I2C2)) {
#if HAL_DEV_I2C_EN
		handler->init = init_i2c;
		handler->reset = reset_i2c;
		handler->read = read_i2c;
		handler->write = write_i2c;
		handler->config = config_i2c;
#else
		return DEV_RET_ERROR;
#endif
	} else {
		return DEV_RET_ERROR;
	}
	return DEV_RET_OK;
}
int unregister_device_handler(struct hal_dev_handler *handler)
{
	handler->device = NULL;
	return DEV_RET_OK;
}
int device_handler_valid(struct hal_dev_handler *handler)
{
	if(!IS_VALID_DEVICE_ID(handler->device_id)) {
		return DEV_RET_ERROR;
	} else if(handler->device == NULL) {
		return DEV_RET_ERROR;
	} else {
		return DEV_RET_OK;
	}	
}


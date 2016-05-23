#ifndef __HAL_DEVICE_H__
#define __HAL_DEVICE_H__

#include <stdint.h>
#include "stm32f10x.h"

/*
 *	HAL DEVICE DECLEARATION
 ****************************************************************
 */
typedef void hal_device_t;
typedef void hal_config_t;

//#ifdef HAL_DEV_USART_EN
typedef hal_device_t usart_dev_t;
typedef hal_config_t usart_config_t;
//#endif

//#ifdef HAL_DEV_SPI1_EN
typedef hal_device_t spi_dev_t;
typedef hal_config_t spi_config_t;
//#endif

//#ifdef //HAL_DEV_I2C_EN
typedef hal_device_t i2c_dev_t;
typedef hal_config_t i2c_config_t;
//#endif

enum HAL_DEVICE_ID {
	
	DEV_NULL,
	DEV_USART1,
	DEV_USART2,
	DEV_SPI1,
	DEV_SPI2,
	DEV_I2C1,
	DEV_I2C2,
};
#define IS_VALID_DEVICE_ID(x) ((x == DEV_USART1)	\
							|| (x == DEV_USART2)	\
							|| (x == DEV_SPI1)	\
							|| (x == DEV_SPI2)	\
							|| (x == DEV_I2C1)	\
							|| (x == DEV_I2C2))
enum HAL_DEVICE_RET {
	DEV_RET_ERROR = 0,
	DEV_RET_OK = 1	
};

struct hal_dev_handler {
	
	enum HAL_DEVICE_ID device_id;
	hal_device_t *device;
	
	void (*init)(hal_device_t *dev);
	void (*reset)(hal_device_t *dev);
	int (*read)(hal_device_t *dev, uint8_t *pbuf, int len);
	int (*write)(hal_device_t *dev, uint8_t *pdat, int len);
	int (*config)(hal_device_t *dev,hal_config_t *cfg);
	int (*set_speed)(int len);
	
	/* USART device will used */
	void (*puts)(hal_device_t *dev, char *s);
	char (*getc)(hal_device_t *dev);
	void (*print_u32)(hal_device_t *dev, uint32_t dat);
	void (*print_u8)(hal_device_t *dev,uint8_t dat);
	void (*print_u16)(hal_device_t *dev,uint16_t dat);
};
typedef struct hal_dev_handler hal_dev_handler_t;

void init_device_handler(struct hal_dev_handler *handler);
int  device_handler_valid(struct hal_dev_handler *handler);
int  register_device_handler(struct hal_dev_handler *handler, enum HAL_DEVICE_ID dev_id);
int  unregister_device_handler(struct hal_dev_handler *handler);

#endif /* __HAL_DEVICE_H__ */

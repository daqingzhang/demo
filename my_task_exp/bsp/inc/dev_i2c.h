#ifndef __DEV_I2C_H__
	#define __DEV_I2C_H__

#include "hal_device.h"

extern void init_i2c(i2c_dev_t *dev);
extern void reset_i2c(i2c_dev_t *dev);
extern int config_i2c(i2c_dev_t *dev,i2c_config_t *config);
extern int read_i2c(i2c_dev_t *dev, uint8_t *pbuf, int len);
extern int write_i2c(i2c_dev_t *dev, uint8_t *pdat, int len);


#endif

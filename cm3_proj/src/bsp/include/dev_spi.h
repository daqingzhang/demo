#ifndef __DEV_SPI_H__
	#define __DEV_SPI_H__
#include "hal_device.h"

extern void init_spi(spi_dev_t *dev);
extern void reset_spi(spi_dev_t *dev);
extern int config_spi(spi_dev_t *dev,spi_config_t *config);
extern int read_spi(spi_dev_t *dev, uint8_t *pbuf, int len);
extern int write_spi(spi_dev_t *dev, uint8_t *pdat, int len);

#endif

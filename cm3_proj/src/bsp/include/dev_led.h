#ifndef __DEV_LED_H__
#define __DEV_LED_H__

#include "hal_config.h"

#define LED1	(1<<0)
#define LED2	(1<<1)
#define LED3	(1<<2)

#define MAX_LED_DEV_NAME_SIZE 10
#define MAX_LED_DEV_NUM 3

struct stu_led_device
{
	int id;
	int state;
	char name[MAX_LED_DEV_NAME_SIZE];
};
typedef struct stu_led_device led_dev_t;

struct stu_led_handler
{
	led_dev_t *priv;
	int (*init)(led_dev_t *dev);
	int (*on)(led_dev_t *dev);
	int (*off)(led_dev_t *dev);
	int (*get_state)(led_dev_t *dev,int *state);
	int (*get_name)(led_dev_t *dev, char *str);
	int (*flash)(led_dev_t *dev, unsigned int cycle);
};
typedef struct stu_led_handler led_handler_t;

#ifdef CONFIG_DEV_LED

int led_dev_list_init(void);
int led_dev_unregister(led_dev_t *dev);
led_dev_t *led_dev_register(int id, const char *name, int state);
int led_dev_print(led_dev_t *dev);

int led_handler_init(led_dev_t *dev,led_handler_t *handler);

void init_led(int led);
void light_on(int led);
void light_off(int led);
void light_flash(int led, unsigned int cycle);

#else
static inline int led_dev_list_init(void){return 0;}
static inline int led_dev_unregister(led_dev_t *dev){return 0;}
static inline led_dev_t *led_dev_register(int id, const char *name, int state){return 0;}
static inline int led_dev_print(led_dev_t *dev){return 0;}
static inline int led_handler_init(led_dev_t *dev,led_handler_t *handler){return 0;}
static inline void init_led(int led){}
static inline void light_on(int led){}
static inline void light_off(int led){}
static inline void light_flash(int led, unsigned int cycle){}
#endif /* CONFIG_DEV_LED */
#endif /* __DEV_LED_H__ */

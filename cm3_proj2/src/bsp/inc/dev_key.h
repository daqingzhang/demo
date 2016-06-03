#ifndef __DEV_KEY_H__
#define __DEV_KEY_H__

#define KEY2_ID	(1<<0)
#define KEY3_ID	(1<<1)
#define KEY4_ID	(1<<2)

#define IS_KEY_VALID(id) (((id) == KEY2_ID)	\
						||((id) == KEY3_ID)	\
						||((id) == KEY3_ID))

#define MAX_KEY_DEV_NAME_SIZE	10
#define MAX_KEY_DEV_NUM	3

struct key_device
{
	int id;
	int clicked; //not used
	int valid;
	int count;
	char name[MAX_KEY_DEV_NAME_SIZE];
	int (*callback)(void *arg);
	void *arg;
};
typedef struct key_device key_dev_t;

struct key_handler
{
	key_dev_t *priv;
	int (*init)(key_dev_t *dev);
	int (*process)(key_dev_t *dev);
	int (*clicked)(key_dev_t *dev);
	int (*valid)(key_dev_t *dev);
	int (*get_name)(key_dev_t *dev,char *name);
	int (*get_id)(key_dev_t *dev);
	int (*get_count)(key_dev_t *dev);
};
typedef struct key_handler key_handler_t;

void kdev_init(void);
key_dev_t* kdev_register(int id,const char *name,
				int (*callback)(void *arg),void *arg);
int kdev_unregister(key_dev_t *dev);
key_handler_t* kdev_add(key_dev_t *dev);
int kdev_process(void);

#endif

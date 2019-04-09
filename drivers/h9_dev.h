#ifndef _H9_DEV_H_
#define _H9_DEV_H_

#include "h9msg.h"

#define DEV_ONSELECT_OK  0
#define DEV_ONSELECT_ERROR    1
#define DEV_ONSELECT_CRITICAL 2

typedef void (dev_onselect_callback_t)(const h9msg_t *msg, void *callback_data);

typedef void* (dev_create_func_t)(const char *connect_string);
typedef void (dev_destroy_func_t)(void *dev_data);
typedef int (dev_onselect_func_t)(void *dev_data, dev_onselect_callback_t *onrecv_callback,
                                  dev_onselect_callback_t *onsend_callback, void *callback_data);
typedef int (dev_send_func_t)(void *dev_data, const h9msg_t *msg);
typedef int (dev_getfd_func_t)(void *dev_data);

typedef struct {
    void *dev_data;

    dev_create_func_t *create_func;
    dev_destroy_func_t *destroy_func;
    dev_onselect_func_t *onselect_func;
    dev_send_func_t *send_func;
    dev_getfd_func_t *getfd_func;
} h9_dev_proxy_t;

h9_dev_proxy_t *h9_dev_factory(const char *name, const char *connect_string);
void h9_dev_proxy_destroy(h9_dev_proxy_t *dev);
int h9_dev_proxy_onselect(h9_dev_proxy_t *dev, dev_onselect_callback_t *onrecv_callback,
                          dev_onselect_callback_t *onsend_callback, void *callback_data);
int h9_dev_proxy_send(h9_dev_proxy_t *dev, const h9msg_t *msg);
int h9_dev_proxy_getfd(h9_dev_proxy_t *dev);

#endif //_H9_DEV_H_

#include "h9_devs/h9_dev.h"
#include "h9_dev.h"
#include "h9_log.h"
#include <stdlib.h>
#include <string.h>

#include "h9_devs/slcan.h"
#include "h9_devs/loop.h"

h9_dev_proxy_t *h9_dev_factory(const char *name, const char *connect_string) {
    if(strncmp(name, "slcan", 5) == 0) {
        void *tmp_dev_data = slcan_connect(connect_string);
        if (!tmp_dev_data) {
            return NULL;
        }
        h9_dev_proxy_t *tmp = malloc(sizeof(h9_dev_proxy_t));

        tmp->create_func = (dev_create_func_t*) slcan_connect;
        tmp->destroy_func = (dev_destroy_func_t*) slcan_free;
        tmp->onselect_func = (dev_onselect_func_t*) slcan_onselect_event;
        tmp->send_func = (dev_send_func_t*) slcan_send;
        tmp->getfd_func = (dev_getfd_func_t*) slcan_getfd;

        tmp->dev_data = tmp_dev_data;
        return tmp;
    }
    else if(strncmp(name, "loop", 4) == 0) {
        void *tmp_dev_data = loop_create(connect_string);
        if (!tmp_dev_data) {
            return NULL;
        }
        h9_dev_proxy_t *tmp = malloc(sizeof(h9_dev_proxy_t));

        tmp->create_func = (dev_create_func_t*) loop_create;
        tmp->destroy_func = (dev_destroy_func_t*) loop_free;
        tmp->onselect_func = (dev_onselect_func_t*) loop_onselect_event;
        tmp->send_func = (dev_send_func_t*) loop_send;
        tmp->getfd_func = (dev_getfd_func_t*) loop_getfd;

        tmp->dev_data = tmp_dev_data;
        return tmp;
    }
    else {
        h9_log_err("unknown dev: %s", name);
    }
    return NULL;
}

void h9_dev_proxy_destroy(h9_dev_proxy_t *dev) {
    dev->destroy_func(dev->dev_data);
    free(dev);
}

int h9_dev_proxy_onselect(h9_dev_proxy_t *dev, dev_onselect_callback_t *onrecv_callback,
                          dev_onselect_callback_t *onsend_callback, void *callback_data) {
    return dev->onselect_func(dev->dev_data, onrecv_callback, onsend_callback, callback_data);
}

int h9_dev_proxy_send(h9_dev_proxy_t *dev, const h9msg_t *msg) {
    return dev->send_func(dev->dev_data, msg);
}

int h9_dev_proxy_getfd(h9_dev_proxy_t *dev) {
    return dev->getfd_func(dev->dev_data);
}

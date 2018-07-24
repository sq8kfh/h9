#include "h9_endpoint/endpoint.h"
#include "endpoint.h"
#include "h9_log.h"
#include <stdlib.h>
#include <string.h>

#include "h9_endpoint/slcan.h"
#include "h9_endpoint/loop.h"

endpoint_t *endpoint_create(const char *name, const char *connect_string) {
    if(strncmp(name, "slcan", 5) == 0) {
        void *tmp_endpoint_data = slcan_connect(connect_string);
        if (!tmp_endpoint_data) {
            return NULL;
        }
        endpoint_t *tmp = malloc(sizeof(endpoint_t));

        tmp->create_func = (endpoint_create_func_t*) slcan_connect;
        tmp->destroy_func = (endpoint_destroy_func_t*) slcan_free;
        tmp->onselect_func = (endpoint_onselect_func_t*) slcan_onselect_event;
        tmp->send_func = (endpoint_send_func_t*) slcan_send;
        tmp->getfd_func = (endpoint_getfd_func_t*) slcan_getfd;

        tmp->endpoint_data = tmp_endpoint_data;
        return tmp;
    }
    else if(strncmp(name, "loop", 4) == 0) {
        void *tmp_endpoint_data = loop_create(connect_string);
        if (!tmp_endpoint_data) {
            return NULL;
        }
        endpoint_t *tmp = malloc(sizeof(endpoint_t));

        tmp->create_func = (endpoint_create_func_t*) loop_create;
        tmp->destroy_func = (endpoint_destroy_func_t*) loop_free;
        tmp->onselect_func = (endpoint_onselect_func_t*) loop_onselect_event;
        tmp->send_func = (endpoint_send_func_t*) loop_send;
        tmp->getfd_func = (endpoint_getfd_func_t*) loop_getfd;

        tmp->endpoint_data = tmp_endpoint_data;
        return tmp;
    }
    else {
        h9_log_err("unknown endpoint: %s", name);
    }
    return NULL;
}

void endpoint_destroy(endpoint_t *endpoint) {
    endpoint->destroy_func(endpoint->endpoint_data);
    free(endpoint);
}

int endpoint_onselect(endpoint_t *endpoint, endpoint_onselect_callback_t *onrecv_callback,
                       endpoint_onselect_callback_t *onsend_callback, void *callback_data) {
    return endpoint->onselect_func(endpoint->endpoint_data, onrecv_callback, onsend_callback, callback_data);
}

int endpoint_send(endpoint_t *endpoint, const h9msg_t *msg) {
    return endpoint->send_func(endpoint->endpoint_data, msg);
}

int endpoint_getfd(endpoint_t *endpoint) {
    return endpoint->getfd_func(endpoint->endpoint_data);
}

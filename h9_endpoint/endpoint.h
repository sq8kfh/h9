#ifndef _ENDPOINT_H_
#define _ENDPOINT_H_

#include "h9msg.h"

#define ENDPOINT_ONSELECT_OK  0
#define ENDPOINT_ONSELECT_ERROR    1
#define ENDPOINT_ONSELECT_CRITICAL 2

typedef void (endpoint_onselect_callback_t)(const h9msg_t *msg, void *callback_data);

typedef void* (endpoint_create_func_t)(const char *connect_string);
typedef void (endpoint_destroy_func_t)(void *endpoint_data);
typedef int (endpoint_onselect_func_t)(void *endpoint_data, endpoint_onselect_callback_t *onrecv_callback,
                                        endpoint_onselect_callback_t *onsend_callback, void *callback_data);
typedef int (endpoint_send_func_t)(void *endpoint_data, const h9msg_t *msg);
typedef int (endpoint_getfd_func_t)(void *endpoint_data);

typedef struct {
    void *endpoint_data;

    endpoint_create_func_t *create_func;
    endpoint_destroy_func_t *destroy_func;
    endpoint_onselect_func_t *onselect_func;
    endpoint_send_func_t *send_func;
    endpoint_getfd_func_t *getfd_func;
} endpoint_t;

endpoint_t *endpoint_create(const char *name, const char *connect_string);
void endpoint_destroy(endpoint_t *endpoint);
int endpoint_onselect(endpoint_t *endpoint, endpoint_onselect_callback_t *onrecv_callback,
                       endpoint_onselect_callback_t *onsend_callback, void *callback_data);
int endpoint_send(endpoint_t *endpoint, const h9msg_t *msg);
int endpoint_getfd(endpoint_t *endpoint);

#endif //_ENDPOINT_H_

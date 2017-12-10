#ifndef _H9D_CLIENT_MODULE_H_
#define _H9D_CLIENT_MODULE_H_

#include <time.h>

typedef struct {
    int socket_d;
} h9d_client_module_t;

h9d_client_module_t *h9d_client_module(int socket);
int h9d_client_module_process_events(h9d_client_module_t *ev_data, int event_type, time_t elapsed);

#endif //_H9D_CLIENT_MODULE_H_

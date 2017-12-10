#ifndef _H9D_SERVER_MODULE_H_
#define _H9D_SERVER_MODULE_H_

#include <stdlib.h>
#include <time.h>

typedef struct {
    int socket_d;
} h9d_server_module_t;

h9d_server_module_t *h9d_server_module_init(uint16_t port);
int h9d_server_module_process_events(h9d_server_module_t *ev_data, int event_type, time_t elapsed);

#endif //_H9D_SERVER_MODULE_H_

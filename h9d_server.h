#ifndef _H9D_SERVER_H_
#define _H9D_SERVER_H_

#include <stdlib.h>
#include <time.h>

typedef struct {
    int socket_d;
} h9d_server_t;

h9d_server_t *h9d_server_init(uint16_t port);
void h9d_server_free(h9d_server_t * server);
int h9d_server_process_events(h9d_server_t *ev_data, int event_type, time_t elapsed);

#endif //_H9D_SERVER_H_

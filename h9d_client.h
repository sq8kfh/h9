#ifndef _H9D_CLIENT_H_
#define _H9D_CLIENT_H_

#include "config.h"
#include <time.h>

#include "h9_xmlsocket.h"

typedef struct h9d_client_t{
    h9_xmlsocket_t *xmlsocket;

    struct h9d_client_t *next;
    struct h9d_client_t *prev;

    h9_counter_t recv_xmlmsg_counter;
    h9_counter_t last_readed_recv_xmlmsg_counter;
    h9_counter_t recv_invalid_xmlmsg_counter;
    h9_counter_t last_readed_recv_invalid_xmlmsg_counter;
    h9_counter_t send_xmlmsg_counter;
    h9_counter_t last_readed_send_xmlmsg_counter;
} h9d_client_t;

void h9d_client_init(size_t recv_buf_size, int xsd_validate);
h9d_client_t *h9d_client_addnew(int socket);
void h9d_client_del(h9d_client_t *cm);
int h9d_client_process_events(h9d_client_t *ev_data, int event_type);

h9d_client_t *h9d_client_first_endpoint(void);
h9d_client_t *h9d_client_getnext_endpoint(const h9d_client_t *c);

#endif //_H9D_CLIENT_H_

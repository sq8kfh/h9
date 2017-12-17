#ifndef _H9D_ENDPOINT_H_
#define _H9D_ENDPOINT_H_

#include "config.h"
#include <time.h>

#include "h9msg.h"
#include "h9_slcan.h"

typedef struct h9d_endpoint_t {
    h9_slcan_t *ep_imp;

    struct h9d_endpoint_t *next;
    struct h9d_endpoint_t *prev;

    h9_counter_t recv_msg_counter;
    h9_counter_t recv_invalid_msg_counter;
    h9_counter_t send_msg_counter;
} h9d_endpoint_t;

typedef unsigned int (h9d_endpoint_callback_t)(h9msg_t *msg, h9d_endpoint_t *ep);

void h9d_endpoint_init(void);
h9d_endpoint_t *h9d_endpoint_addnew(char *connect_string);
void h9d_endpoint_del(h9d_endpoint_t *endpoint_struct);

int h9d_endpoint_process_events(h9d_endpoint_t *endpoint_struct, int event_type, time_t elapsed);

#endif //_H9D_ENDPOINT_H_

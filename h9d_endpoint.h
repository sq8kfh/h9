#ifndef _H9D_ENDPOINT_H_
#define _H9D_ENDPOINT_H_

#include "config.h"
#include <time.h>

#include "h9msg.h"
#include "h9_endpoint/endpoint.h"

typedef struct {
    const char *connect_string;
    const char *name;
    unsigned throttle_level;
    int auto_respawn;
} h9d_endpoint_init_parameters_t;

typedef struct h9d_endpoint_t {
    h9d_endpoint_init_parameters_t *init_parameters;

    int auto_respawn;

    char *endpoint_name;
    endpoint_t *endpoint;

    struct h9d_endpoint_t *next;
    struct h9d_endpoint_t *prev;

    unsigned int msq_in_queue;
    unsigned int throttle_level;

    h9_counter_t throttled_counter;
    h9_counter_t last_readed_throttled_counter;
    h9_counter_t recv_msg_counter;
    h9_counter_t last_readed_recv_msg_counter;
    h9_counter_t recv_invalid_msg_counter;
    h9_counter_t last_readed_recv_invalid_msg_counter;
    h9_counter_t send_msg_counter;
    h9_counter_t last_readed_send_msg_counter;
    h9_counter_t recv_msg_by_type_counter[H9MSG_TYPE_COUNT];
} h9d_endpoint_t;


h9d_endpoint_t *h9d_endpoint_addnew(const char *connect_string, const char *name,
                                    unsigned throttle_level,
                                    int auto_respawn);
void h9d_endpoint_del(h9d_endpoint_t *endpoint_struct);

h9d_endpoint_init_parameters_t *h9d_endpoint_init_parameters_init(const char *connect_string,
                                                                  const char *name,
                                                                  unsigned throttle_level,
                                                                  int auto_respawn);
void h9d_endpoint_init_parameters_free(h9d_endpoint_init_parameters_t *ip);
h9d_endpoint_init_parameters_t *h9d_endpoint_init_parameters_cpy(const h9d_endpoint_init_parameters_t *ip);

int h9d_endpoint_process_events(h9d_endpoint_t *endpoint_struct, int event_type);
int h9d_endpoint_send_msg(h9msg_t *msg);

h9d_endpoint_t *h9d_endpoint_first_endpoint(void);
h9d_endpoint_t *h9d_endpoint_getnext_endpoint(const h9d_endpoint_t *e);

#endif //_H9D_ENDPOINT_H_

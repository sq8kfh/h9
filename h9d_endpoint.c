#include "h9d_endpoint.h"
#include "h9d_select_event.h"
#include "h9_log.h"
#include "h9d_trigger.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <termios.h>


static h9d_endpoint_t *endpoint_list_start = NULL;

static h9d_endpoint_t *h9d_endpoint_create(const h9d_endpoint_init_parameters_t *ip) {
    return h9d_endpoint_addnew(ip->connect_string,
                               ip->name,
                               ip->throttle_level,
                               ip->auto_respawn);
}

h9d_endpoint_t *h9d_endpoint_addnew(const char *connect_string,
                                    const char *name,
                                    unsigned int throttle_level,
                                    int auto_respawn) {

    endpoint_t *imp_tmp = endpoint_create(name, connect_string);

    if (!imp_tmp) {
        return NULL;
    }

    h9d_endpoint_t *ep = malloc(sizeof(h9d_endpoint_t));

    ep->endpoint = imp_tmp;

    ep->init_parameters = h9d_endpoint_init_parameters_init(connect_string,
                                                            name,
                                                            throttle_level,
                                                            auto_respawn);

    ep->auto_respawn = auto_respawn;

    ep->endpoint_name = strdup(name);

    ep->recv_invalid_msg_counter = 0;
    ep->last_readed_throttled_counter = 0;
    ep->recv_msg_counter = 0;
    ep->last_readed_recv_msg_counter = 0;
    ep->send_msg_counter = 0;
    ep->last_readed_send_msg_counter = 0;
    ep->throttled_counter = 0;
    ep->last_readed_throttled_counter = 0;

    for (int i = 0; i < H9MSG_TYPE_COUNT; ++i) {
        ep->recv_msg_by_type_counter[i] = 0;
    }

    ep->msq_in_queue = 0;
    ep->throttle_level = throttle_level;

    ep->next = NULL;
    ep->prev = NULL;

    if (endpoint_list_start) {
        ep->next = endpoint_list_start;
        endpoint_list_start->prev = ep;
        endpoint_list_start = ep;
    }
    else {
        endpoint_list_start = ep;
        ep->next = NULL;
    }

    return ep;
}

void h9d_endpoint_del(h9d_endpoint_t *endpoint_struct) {
    h9_log_debug("endpoint %p stats: send %u msg; recv %u msg; recv invalid %u msg",
                 endpoint_struct->endpoint,
                 endpoint_struct->send_msg_counter,
                 endpoint_struct->recv_msg_counter,
                 endpoint_struct->recv_invalid_msg_counter);

    for (h9d_endpoint_t *ep = endpoint_list_start; ep; ep = ep->next) {
        if (ep == endpoint_struct) {
            if (ep->prev) {
                ep->prev->next = ep->next;
            }
            else {
                endpoint_list_start = ep->next;
            }

            if (ep->next) {
                ep->next->prev = ep->prev;
            }

            break;
        }
    }

    endpoint_destroy(endpoint_struct->endpoint);
    h9d_endpoint_init_parameters_free(endpoint_struct->init_parameters);
    free(endpoint_struct->endpoint_name);
    free(endpoint_struct);
}

h9d_endpoint_init_parameters_t *h9d_endpoint_init_parameters_init(const char *connect_string,
                                                                  const char *name,
                                                                  unsigned throttle_level,
                                                                  int auto_respawn) {
    h9d_endpoint_init_parameters_t *ip = malloc(sizeof(h9d_endpoint_init_parameters_t));

    ip->connect_string = strdup(connect_string);
    ip->name = strdup(name);
    ip->throttle_level = throttle_level;
    ip->auto_respawn = auto_respawn;

    return ip;
}

void h9d_endpoint_init_parameters_free(h9d_endpoint_init_parameters_t *ip) {
    free((void *)ip->connect_string);
    free((void *)ip->name);
    free(ip);
}

h9d_endpoint_init_parameters_t *h9d_endpoint_init_parameters_cpy(const h9d_endpoint_init_parameters_t *ip) {
    return h9d_endpoint_init_parameters_init(ip->connect_string,
                                             ip->name,
                                             ip->throttle_level,
                                             ip->auto_respawn);
}

static void endpoint_respawn(h9d_endpoint_init_parameters_t *init_parameters, uint32_t mask, void *param) {
    h9d_endpoint_t *endpoint = h9d_endpoint_create(init_parameters);
    if (!endpoint) {
        h9_log_err("cannot open endpoint");
    }
    else {
        h9d_select_event_add(endpoint_getfd(endpoint->endpoint), H9D_SELECT_EVENT_READ | H9D_SELECT_EVENT_DISCONNECT,
                             (h9d_select_event_func_t *) h9d_endpoint_process_events, endpoint);
        h9d_trigger_del_listener(mask, init_parameters, (h9d_trigger_callback*)endpoint_respawn);
        h9d_endpoint_init_parameters_free(init_parameters);
    }
}

static void on_recv(const h9msg_t *msg, h9d_endpoint_t *endpoint_struct) {
    endpoint_struct->recv_msg_counter++;
    if (msg == NULL) {
        endpoint_struct->recv_invalid_msg_counter++;
        return;
    }
    endpoint_struct->recv_msg_by_type_counter[msg->type]++;

    h9msg_t *local_msg = h9msg_copy(msg);
    h9msg_replace_endpoint(local_msg, endpoint_struct->endpoint_name);

    h9_log_info("receive msg: %hu->%hu priority: %c; type: %hhu; dlc: %hhu; endpoint '%s'",
                local_msg->source_id, local_msg->destination_id,
                local_msg->priority == H9MSG_PRIORITY_HIGH ? 'H' : 'L',
                local_msg->type, local_msg->dlc,
                local_msg->endpoint);

    h9d_trigger_call(H9D_TRIGGER_RECV_MSG, local_msg);

    h9msg_free(local_msg);
}

static void on_send(const h9msg_t *msg, h9d_endpoint_t *endpoint_struct) {
    endpoint_struct->send_msg_counter++;
    endpoint_struct->msq_in_queue--;

    h9msg_t *local_msg = h9msg_copy(msg);

    h9d_trigger_call(H9D_TRIGGER_RECV_MSG, local_msg);

    h9msg_free(local_msg);
}

int h9d_endpoint_process_events(h9d_endpoint_t *endpoint_struct, int event_type) {
    if (event_type & H9D_SELECT_EVENT_READ) {
        int res = endpoint_onselect(endpoint_struct->endpoint,
                                          (endpoint_onselect_callback_t*)on_recv,
                                          (endpoint_onselect_callback_t*)on_send,
                                          endpoint_struct);
        if (res == ENDPOINT_ONSELECT_CRITICAL) {
            return H9D_SELECT_EVENT_RETURN_DISCONNECT;
        }
    }
    if (event_type & H9D_SELECT_EVENT_DISCONNECT) {
        if (endpoint_struct->auto_respawn) {
            h9d_endpoint_init_parameters_t *tmp = h9d_endpoint_init_parameters_cpy(endpoint_struct->init_parameters);
            h9d_endpoint_del(endpoint_struct);
            h9d_trigger_add_listener(H9D_TRIGGER_TIMMER, tmp, (h9d_trigger_callback*)endpoint_respawn);
        }
        else {
            h9d_endpoint_del(endpoint_struct);
        }

        return H9D_SELECT_EVENT_RETURN_DEL;
    }
    return H9D_SELECT_EVENT_RETURN_OK;
}

int h9d_endpoint_send_msg(const h9msg_t *msg) {
    h9msg_t *local_msg = h9msg_copy(msg);
    h9msg_replace_endpoint(local_msg, "h9d");

    for (h9d_endpoint_t *ep = endpoint_list_start; ep; ep = ep->next) {
        if (ep->throttle_level) {
            if (ep->msq_in_queue < ep->throttle_level) {
                ep->msq_in_queue++;
                endpoint_send(ep->endpoint, local_msg);
            }
            else {
                ep->throttled_counter++;
                h9msg_free(local_msg);
                return 0; //throttled
            }
        }
        else {
            ep->msq_in_queue++;
            endpoint_send(ep->endpoint, local_msg);
        }
    }
    h9msg_free(local_msg);
    return 1;
}

h9d_endpoint_t *h9d_endpoint_first_endpoint(void) {
    return endpoint_list_start;
}

h9d_endpoint_t *h9d_endpoint_getnext_endpoint(const h9d_endpoint_t *ep) {
    if (ep) {
        return ep->next;
    }
    return NULL;
}

#include "h9d_endpoint.h"
#include "h9d_select_event.h"
#include "h9_log.h"
#include "h9d_trigger.h"
#include "h9_slcan.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <termios.h>

#define INIT_BUF_SIZE 100

static h9d_endpoint_t *endpoint_list_start;

void h9d_endpoint_init(void) {
    endpoint_list_start = NULL;
}

h9d_endpoint_t *h9d_endpoint_addnew(const char *connect_string, const char *name) {
    h9d_endpoint_t *ep = malloc(sizeof(h9d_endpoint_t));
    ep->endpoint_name = strdup(name);

    ep->ep_imp = h9_slcan_connect(connect_string, INIT_BUF_SIZE);
    if (!ep->ep_imp) {
        free(ep);
        return NULL;
    }
    ep->recv_invalid_msg_counter = 0;
    ep->recv_msg_counter = 0;
    ep->send_msg_counter = 0;

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
                 endpoint_struct->ep_imp,
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

    h9_slcan_free(endpoint_struct->ep_imp);
    free(endpoint_struct->endpoint_name);
    free(endpoint_struct);
}

static unsigned int process_msg(h9msg_t *msg, h9d_endpoint_t *endpoint_struct) {
    endpoint_struct->recv_msg_counter++;
    if (msg->endpoint) {
        free(msg->endpoint);
    }
    msg->endpoint = strdup(endpoint_struct->endpoint_name);

    h9_log_info("rcv msg: priority: %u type: %u src: %u dest: %u",
                  (uint32_t)msg->priority, (uint32_t)msg->type,
                  (uint32_t)msg->source_id, (uint32_t)msg->destination_id);
    //endpoint_struct->recv_invalid_msg_counter++;

    h9d_trigger_call(H9D_TRIGGER_RECV_MSG, msg);

    return 1;
}

int h9d_endpoint_process_events(h9d_endpoint_t *endpoint_struct, int event_type, time_t elapsed){
    if (event_type & H9D_SELECT_EVENT_READ) {
        int res = h9_slcan_recv(endpoint_struct->ep_imp, (h9_slcan_recv_callback_t*)process_msg, endpoint_struct);
        if (res <= 0) {
            if (res < 0) {
                return H9D_SELECT_EVENT_RETURN_DISCONNECT;
            }
            else {
                endpoint_struct->recv_invalid_msg_counter++;
            }
        }
    }
    if (event_type & H9D_SELECT_EVENT_DISCONNECT) {
        h9d_endpoint_del(endpoint_struct);
        return H9D_SELECT_EVENT_RETURN_DEL;
    }
    return H9D_SELECT_EVENT_RETURN_OK;
}

int h9d_endpoint_send_msg(h9msg_t *msg) {
    if (msg->endpoint) {
        free(msg->endpoint);
    }
    msg->endpoint = strdup("h9d");

    for (h9d_endpoint_t *ep = endpoint_list_start; ep; ep = ep->next) {
        h9_slcan_send(ep->ep_imp, msg);
        ep->send_msg_counter++;
    }

    return 1;
}

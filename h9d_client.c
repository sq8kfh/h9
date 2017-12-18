#include "h9d_client.h"
#include "h9d_select_event.h"
#include "h9_xmlmsg.h"
#include "h9_log.h"
#include "h9d_trigger.h"
#include "h9d_endpoint.h"

static h9d_client_t *client_list_start;
//static h9d_client_t *client_list_end;
static size_t init_buffer_size;

static h9_counter_t global_recv_xmlmsg_counter;
static h9_counter_t global_recv_invalid_xmlmsg_counter;
static h9_counter_t global_send_xmlmsg_counter;

static int xmlmsg_schema_validation;

static void trigger_callback(h9d_client_t *client, uint32_t mask, void *param);

void h9d_client_init(size_t recv_buf_size, int xsd_validate) {
    init_buffer_size = recv_buf_size;

    client_list_start = NULL;
    //client_list_end = NULL;

    xmlmsg_schema_validation = xsd_validate;
}

h9d_client_t *h9d_client_addnew(int socket) {
    h9d_client_t *client_struct = malloc(sizeof(h9d_client_t));

    client_struct->xmlsocket = h9_xmlsocket_init(socket, init_buffer_size);

    client_struct->next = NULL;
    client_struct->prev = NULL;

    client_struct->recv_invalid_xmlmsg_counter = 0;
    client_struct->recv_xmlmsg_counter = 0;
    client_struct->send_xmlmsg_counter = 0;

    if (client_list_start) {
        client_struct->next = client_list_start;
        client_list_start->prev = client_struct;
        client_list_start = client_struct;
    }
    else {
        client_list_start = client_struct;
        client_struct->next = NULL;
    }

    return client_struct;
}

void h9d_client_del(h9d_client_t *client_struct) {
    h9_log_debug("client %p stats: send %u msg; recv %u msg; recv invalid %u msg",
                 client_struct->xmlsocket,
                 client_struct->send_xmlmsg_counter,
                 client_struct->recv_xmlmsg_counter,
                 client_struct->recv_invalid_xmlmsg_counter);

    global_recv_xmlmsg_counter += client_struct->send_xmlmsg_counter;
    global_recv_invalid_xmlmsg_counter += client_struct->recv_xmlmsg_counter;
    global_send_xmlmsg_counter += client_struct->recv_invalid_xmlmsg_counter;

    for (h9d_client_t *client = client_list_start; client; client = client->next) {
        if (client == client_struct) {
            if (client->prev) {
                client->prev->next = client->next;
            }
            else {
                client_list_start = client->next;
            }

            if (client->next) {
                client->next->prev = client->prev;
            }

            break;
        }
    }

    h9d_trigger_del_listener(H9D_TRIGGER_ALL, client_struct, (h9d_trigger_callback*)trigger_callback);

    h9_xmlsocket_free(client_struct->xmlsocket);
    free(client_struct);
}

static unsigned int process_xmlmsg(const char *msg, size_t length, h9d_client_t *client_struct) {
    client_struct->recv_xmlmsg_counter++;

    void *res;
    int ret = h9_xmlmsg_parse(msg, length, &res, xmlmsg_schema_validation);

    if (ret == H9_XMLMSG_H9SENDMSG && res) {
        h9msg_t *ms = (h9msg_t*)res;
        h9d_endpoint_send_msg(ms);
    }
    else if (ret == H9_XMLMSG_H9SUBSCRIBE) {
        h9d_trigger_add_listener(H9D_TRIGGER_RECV_MSG, client_struct, (h9d_trigger_callback*)trigger_callback);
    }
    else if (ret == H9_XMLMSG_H9UNSUBSCRIBE) {
        h9d_trigger_del_listener(H9D_TRIGGER_RECV_MSG, client_struct, (h9d_trigger_callback*)trigger_callback);
    }
    else {
        client_struct->recv_invalid_xmlmsg_counter++;
    }

    h9_xmlmsg_free_parse_data(ret, res);

    return 1;
}

int h9d_client_process_events(h9d_client_t *client_struct, int event_type, time_t elapsed) {
    if (event_type & H9D_SELECT_EVENT_READ) {
        if (h9_xmlsocket_recv(client_struct->xmlsocket, (h9_xmlsocket_read_callback_t*)process_xmlmsg, client_struct) <= 0) {
            return H9D_SELECT_EVENT_RETURN_DISCONNECT;
        }
    }
    if (event_type & H9D_SELECT_EVENT_DISCONNECT) {
        h9d_client_del(client_struct);
        return H9D_SELECT_EVENT_RETURN_DEL;
    }
    return H9D_SELECT_EVENT_RETURN_OK;
}

static void trigger_callback(h9d_client_t *client, uint32_t mask, void *param) {
    size_t length;
    char *buf;
    switch (mask) {
        case H9D_TRIGGER_RECV_MSG:
            buf = h9_xmlmsg_build_h9msg(&length, param, xmlmsg_schema_validation);
            if (buf) {
                h9_xmlsocket_send(client->xmlsocket, buf, length);
            }
            break;
    }
}
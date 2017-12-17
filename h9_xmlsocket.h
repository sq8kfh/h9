#ifndef _H9_XMLSOCKET_H_
#define _H9_XMLSOCKET_H_

#include <stdlib.h>

typedef struct {
    int socket_d;
    int state;
    size_t in_buf;
    size_t buf_size;
    char *buf;
    size_t msg_size;

    uint32_t recv_byte_counter;
    uint32_t send_byte_counter;
} h9_xmlsocket_t;

typedef unsigned int (h9_xmlsocket_read_callback_t)(const char *msg, size_t length, void *ud);

h9_xmlsocket_t *h9_xmlsocket_init(int socket, size_t init_buf_size);
h9_xmlsocket_t *h9_xmlsocket_connect(const char *hostname, const char *servname, size_t init_buf_size);
void h9_xmlsocket_free(h9_xmlsocket_t *xmlsocket);

int h9_xmlsocket_recv(h9_xmlsocket_t *xmlsocket, h9_xmlsocket_read_callback_t *callback, void *callback_data);
int h9_xmlsocket_send(h9_xmlsocket_t *xmlsocket, const char *msg, size_t length);

#endif //_H9_XMLSOCKET_H_

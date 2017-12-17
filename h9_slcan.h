#ifndef _H9_SLCAN_H_
#define _H9_SLCAN_H_

#include <stdlib.h>
#include "h9msg.h"

typedef struct {
    int fd;

    size_t in_buf;
    size_t buf_size;
    char *buf;
    size_t buf_ptr;

    uint32_t read_byte_counter;
    uint32_t write_byte_counter;
} h9_slcan_t;

typedef unsigned int (h9_slcan_recv_callback_t)(h9msg_t *msg, void *callback_data);


h9_slcan_t *h9_slcan_connect(const char *connect_string, size_t init_buf_size);
void h9_slcan_free(h9_slcan_t *slcan);

int h9_slcan_recv(h9_slcan_t *slcan, h9_slcan_recv_callback_t *callback, void *callback_data);
int h9_slcan_send(h9_slcan_t *slcan, const h9msg_t *msg);

#endif //_H9_SLCAN_H_

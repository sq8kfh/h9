#ifndef _H9_SLCAN_H_
#define _H9_SLCAN_H_

#include <stdlib.h>
#include "config.h"
#include "h9msg.h"

typedef struct {
    int fd;

    size_t in_buf;
    size_t buf_size;
    char *buf;
    size_t buf_ptr;

    h9_counter_t read_byte_counter;
    h9_counter_t last_readed_read_byte_counter;
    h9_counter_t write_byte_counter;
    h9_counter_t last_readed_write_byte_counter;
} h9_slcan_t;


#define ONSELECT_SUCCESS  0
#define ONSELECT_ERROR    1
#define ONSELECT_CRITICAL 2

typedef void (onselect_callback_t)(h9msg_t *msg, void *callback_data);

h9_slcan_t *h9_slcan_connect(const char *connect_string,
                             size_t init_buf_size,
                             int nonblock);
void h9_slcan_free(h9_slcan_t *slcan);

int h9_slcan_onselect_event(h9_slcan_t *slcan,
                            onselect_callback_t *recv_callback,
                            onselect_callback_t *send_callback,
                            void *callback_data);

int h9_slcan_send(h9_slcan_t *slcan, h9msg_t *msg);

#endif //_H9_SLCAN_H_

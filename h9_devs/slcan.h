#ifndef _H9_SLCAN_H_
#define _H9_SLCAN_H_

#include <stdlib.h>
#include "config.h"
#include "h9msg.h"
#include "h9_devs/h9_dev.h"

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
} slcan_t;


slcan_t *slcan_connect(const char *connect_string);
void slcan_free(slcan_t *slcan);

int slcan_onselect_event(slcan_t *slcan,
                         dev_onselect_callback_t *recv_callback,
                         dev_onselect_callback_t *send_callback,
                         void *callback_data);

int slcan_send(slcan_t *slcan, const h9msg_t *msg);
int slcan_getfd(slcan_t *slcan);

#endif //_SLCAN_H_

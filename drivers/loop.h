#ifndef _LOOP_H_
#define _LOOP_H_

#include <stdlib.h>
#include "config.h"
#include "h9msg.h"
#include "h9_dev.h"

typedef struct {
    int fd[2];
    //h9msg_t *buf;
} loop_t;


loop_t *loop_create(const char *connect_string);
void loop_free(loop_t *loop);

int loop_onselect_event(loop_t *slcan,
                        dev_onselect_callback_t *recv_callback,
                        dev_onselect_callback_t *send_callback,
                        void *callback_data);

int loop_send(loop_t *loop, const h9msg_t *msg);
int loop_getfd(loop_t *loop);

#endif //_LOOP_H_

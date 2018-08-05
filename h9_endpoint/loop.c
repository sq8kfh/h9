#include "loop.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

loop_t *loop_create(const char *connect_string) {
    loop_t *lo = malloc(sizeof(loop_t));
    socketpair(PF_LOCAL, SOCK_STREAM, 0, lo->fd);
    return lo;
}

void loop_free(loop_t *loop) {
    close(loop->fd[0]);
    close(loop->fd[1]);
    free(loop);
}

int loop_onselect_event(loop_t *loop,
                        endpoint_onselect_callback_t *recv_callback,
                        endpoint_onselect_callback_t *send_callback,
                        void *callback_data) {
    //send_callback(loop->buf, callback_data);
    //h9msg_free(loop->buf);
    //loop->buf = NULL;

    h9msg_t buf;
    recv(loop->fd[0], &buf, sizeof(h9msg_t), 0);
    buf.endpoint = NULL;

    send_callback(&buf, callback_data);
    //h9msg_t *tmp = h9msg_copy(&buf);
    recv_callback(&buf, callback_data);
    //h9msg_free(tmp);

    return ENDPOINT_ONSELECT_OK;
}

int loop_send(loop_t *loop, const h9msg_t *msg) {
    //loop->buf = h9msg_copy(msg);
    send(loop->fd[1], msg, sizeof(h9msg_t), 0);
    return ENDPOINT_ONSELECT_OK;
}

int loop_getfd(loop_t *loop) {
    return loop->fd[0];
}

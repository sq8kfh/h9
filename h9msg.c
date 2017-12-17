#include "h9msg.h"

#include <stdlib.h>

h9msg_t *h9msg_init(void) {
    h9msg_t *msg = malloc(sizeof(h9msg_t));
    msg->endpoint = NULL;
    return msg;
}

void h9msg_free(h9msg_t * msg) {
    if (msg->endpoint) {
        free(msg->endpoint);
    }
    free(msg);
}

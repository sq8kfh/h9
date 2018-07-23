#include "h9msg.h"

#include <stdlib.h>
#include <string.h>

static const char* const h9_type_name[] = {
        "NOP",
        "PAGE_START",
        "QUIT_BOOTLOADER",
        "PAGE_FILL",
        "ENTER_INTO_BOOTLOADER",
        "PAGE_FILL_NEXT",
        "PAGE_WRITED",
        "PAGE_FILL_BREAK",
        "REG_EXTERNALLY_CHANGED",
        "REG_INTERNALLY_CHANGED",
        "REG_VALUE_BROADCAST",
        "REG_VALUE",
        "NODE_HEARTBEAT",
        "NODE_ERROR",
        "U14",
        "U15",
        "SET_REG",
        "GET_REG",
        "NODE_INFO",
        "NODE_RESET",
        "NODE_UPGRADE",
        "U21",
        "U22",
        "U23",
        "DISCOVERY",
        "NODE_TURNED_ON",
        "POWER_OFF",
        "U27",
        "U28",
        "U29",
        "U30",
        "U31"
};

h9msg_t *h9msg_init(void) {
    h9msg_t *msg = malloc(sizeof(h9msg_t));
    msg->endpoint = NULL;
    msg->priority = H9MSG_PRIORITY_LOW;
    msg->type = H9MSG_TYPE_NOP;
    msg->dlc = 0;
    return msg;
}

void h9msg_free(h9msg_t * msg) {
    if (msg->endpoint) {
        free(msg->endpoint);
    }
    free(msg);
}

h9msg_t *h9msg_copy(h9msg_t * msg) {
    h9msg_t *ret = h9msg_init();
    *ret = *msg;
    if (msg->endpoint != NULL)
        ret->endpoint = strdup(msg->endpoint);
    return ret;
}

const char *h9msg_type_name(uint8_t type) {
    return h9_type_name[type];
}

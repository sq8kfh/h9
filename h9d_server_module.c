#include "h9d_server_module.h"

h9d_select_event_t * h9d_server_module_init(uint16_t port) {
    h9d_select_event_t *ev = h9d_select_event_create();

    port = port+1;

    return ev;
}

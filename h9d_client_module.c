#include "h9d_client_module.h"
#include "h9d_select_event.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

h9d_client_module_t *h9d_client_module(int socket){
    h9d_client_module_t *mc = malloc(sizeof(h9d_client_module_t));

    mc->socket_d = socket;

    return mc;
}

int h9d_client_module_process_events(h9d_client_module_t *ev_data, int event_type, time_t elapsed) {
    if (event_type == H9D_SELECT_EVENT_READ) {
        int nbytes;
        char buf[100];
        if ((nbytes = recv(ev_data->socket_d, buf, sizeof(buf)-1, 0)) <= 0) {
            if (nbytes == 0) {
                printf("selectserver: socket %d hung up\n", ev_data->socket_d);
            } else {
                perror("recv");
            }

            close(ev_data->socket_d);
            free(ev_data);

            return H9D_SELECT_EVENT_RETURN_DEL;
        } else {
            buf[nbytes] = '\0';
            printf("recv: %s\n", buf);
        }
    }

    return H9D_SELECT_EVENT_RETURN_OK;
}

#include "h9d_server_module.h"
#include "h9d_select_event.h"
#include "h9_log.h"
#include "h9d_client_module.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

h9d_server_module_t * h9d_server_module_init(uint16_t port) {
    h9d_server_module_t *sm = malloc(sizeof(h9d_server_module_t));

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, "7878", &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        sm->socket_d = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sm->socket_d < 0) {
            continue;
        }

        // lose the pesky "address already in use" error message
        setsockopt(sm->socket_d, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(sm->socket_d, p->ai_addr, p->ai_addrlen) < 0) {
            close(sm->socket_d);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(sm->socket_d, 10) == -1) {
        perror("listen");
        exit(3);
    }

    return sm;
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int h9d_server_module_process_events(h9d_server_module_t *ev_data, int event_type, time_t elapsed) {
    if (event_type == H9D_SELECT_EVENT_READ) {
        printf("asdasd\n");
        int newfd;
        struct sockaddr_storage remoteaddr;
        socklen_t addrlen;
        char remoteIP[INET6_ADDRSTRLEN];

        addrlen = sizeof(remoteaddr);
        newfd = accept(ev_data->socket_d, (struct sockaddr *)&remoteaddr, &addrlen);

        if (newfd == -1) {
            perror("accept");
        } else {
            h9_log_debug("selectserver: new connection from %s on socket %d\n",
                         inet_ntop(remoteaddr.ss_family, \
                         get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), newfd);

            h9d_client_module_t *new_client = h9d_client_module(newfd);
            h9d_select_event_add(newfd, H9D_SELECT_EVENT_READ,
                                 (h9d_select_event_func_t*)h9d_client_module_process_events, new_client);

        }
    }
    return H9D_SELECT_EVENT_RETURN_OK;
}

#include "h9d_server.h"
#include "h9d_select_event.h"
#include "h9_log.h"
#include "h9d_client.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

static void *get_in_addr(struct sockaddr *sa);

h9d_server_t * h9d_server_init(uint16_t port) {
    h9d_server_t *sm = malloc(sizeof(h9d_server_t));

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

void h9d_server_free(h9d_server_t *server) {
    close(server->socket_d);
    free(server);
}

int h9d_server_process_events(h9d_server_t *ev_data, int event_type, time_t elapsed) {
    if (event_type & H9D_SELECT_EVENT_READ) {
        int newfd;
        struct sockaddr_storage remoteaddr;
        socklen_t addrlen;
        char remoteIP[INET6_ADDRSTRLEN];

        addrlen = sizeof(remoteaddr);
        newfd = accept(ev_data->socket_d, (struct sockaddr *)&remoteaddr, &addrlen);

        if (newfd == -1) {
            perror("accept");
        } else {
            h9_log_notice("selectserver: new connection from %s on socket %d",
                         inet_ntop(remoteaddr.ss_family, \
                         get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), newfd);

            h9d_client_t *new_client = h9d_client_addnew(newfd);
            h9d_select_event_add(newfd, H9D_SELECT_EVENT_READ | H9D_SELECT_EVENT_DISCONNECT,
                                 (h9d_select_event_func_t*)h9d_client_process_events, new_client);

        }
    }
    if (event_type & H9D_SELECT_EVENT_DISCONNECT) {
        h9d_server_free(ev_data);
        return H9D_SELECT_EVENT_RETURN_DEL;
    }
    return H9D_SELECT_EVENT_RETURN_OK;
}

static void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

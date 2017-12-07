#include "h9d_select_event.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

h9d_select_event_t *event_list = NULL;

void h9d_select_event_init(void) {

}

void h9d_select_event_loop(void) {
    while (1) {
        fd_set rfds;
        struct timeval tv;
        int retval;

        /* Watch stdin (fd 0) to see when it has input. */

        FD_ZERO(&rfds);
        FD_SET(0, &rfds);

        /* Wait up to five seconds. */

        tv.tv_sec = 5;
        tv.tv_usec = 0;

        retval = select(1, &rfds, NULL, NULL, &tv);
        /* Don't rely on the value of tv now! */

        if (retval == -1) {
            perror("select()");
            break;
        }
        else if (retval) {
            printf("Data is available now.\n");
            char buf[100];
            read(1, buf, 100);
            /* FD_ISSET(0, &rfds) will be true. */
        }
        else
            printf("No data within five seconds.\n");
    }
}

h9d_select_event_t *h9d_select_event_create(void) {
    h9d_select_event_t *tmp = malloc(sizeof(h9d_select_event_t));
    tmp->fd = -1;
    tmp->prev = NULL;
    tmp->next = NULL;

    return tmp;
}

void h9d_select_event_free(h9d_select_event_t *ev) {
    free(ev);
}

void h9d_select_event_add(h9d_select_event_t *ev) {
    ev->prev = NULL;
    if (event_list) {
        ev->next = event_list;
        event_list->prev = ev;
        event_list = ev;
    }
    else {
        event_list = ev;
        ev->next = NULL;
    }
}

void h9d_select_event_del(h9d_select_event_t *ev) {
    if (ev->prev) {
        ev->prev->next = ev->next;
    }
    else {
        event_list = ev->next;
    }

    if (ev->next) {
        ev->next->prev = ev->prev;
    }

    ev->prev = NULL;
    ev->next = NULL;
}

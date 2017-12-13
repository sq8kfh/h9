#include "h9d_select_event.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/select.h>

#include "h9_log.h"

typedef struct h9d_select_event_t {
    int fd;
    h9d_select_event_func_t *process_events;
    void *ev_data;
    int event_types;

    struct h9d_select_event_t *prev;
    struct h9d_select_event_t *next;
} h9d_select_event_t;

h9d_select_event_t *event_list;
fd_set event_fd_set;

void h9d_select_event_init(void) {
    event_list = NULL;
    FD_ZERO(&event_fd_set);
}

void h9d_select_event_loop(void) {
    time_t last_time = time(NULL);
    while (1) {
        struct timeval tv;
        h9d_select_event_t tmp;
        fd_set rfds;

        time_t t = 5 + last_time - time(NULL);
        tv.tv_sec = t > 0 ? t : 0;
        tv.tv_usec = 1;

        FD_COPY(&event_fd_set, &rfds);

        int retval;
        int max_fd = 0;
        for (h9d_select_event_t *ev = event_list; ev; ev = ev->next) {
            if (ev->fd > max_fd) {
                max_fd = ev->fd;
            }
        }

        retval = select(max_fd+1, &rfds, NULL, NULL, &tv);

        if (retval == -1) {
            perror("select()");
            break;
        }
        else if (retval) {
            for (h9d_select_event_t *ev = event_list; ev; ev = ev->next) {
                if (ev->event_types & H9D_SELECT_EVENT_READ && FD_ISSET(ev->fd, &rfds)) {
                    int r = ev->process_events(ev->ev_data, H9D_SELECT_EVENT_READ, -1);
                    if (r == H9D_SELECT_EVENT_RETURN_DEL) {
                        tmp.next = ev->next;
                        h9d_select_event_del(ev->fd);
                        ev = &tmp;
                    }
                }
            }
        }
        if (retval == 0 || time(NULL) - last_time >= 5) {
            for (h9d_select_event_t *ev = event_list; ev; ev = ev->next) {
                if (ev->event_types & H9D_SELECT_EVENT_TIME) {
                    int r = ev->process_events(ev->ev_data, H9D_SELECT_EVENT_TIME, time(NULL) - last_time);
                    if (r == H9D_SELECT_EVENT_RETURN_DEL) {
                        tmp.next = ev->next;
                        h9d_select_event_del(ev->fd);
                        ev = &tmp;
                    }
                }
            }
            //h9_log_debug("%s: timer (%ds elapsed)\n", __func__, time(NULL) - last_time);
            last_time = time(NULL);
        }
    }
}

void h9d_select_event_add(int fd, int event_types, h9d_select_event_func_t *func, void *ev_data) {
    h9d_select_event_t *ev = malloc(sizeof(h9d_select_event_t));

    ev->fd = fd;
    ev->process_events = func;
    ev->event_types = event_types;
    ev->ev_data = ev_data;

    ev->prev = NULL;
    ev->next = NULL;

    if (event_list) {
        ev->next = event_list;
        event_list->prev = ev;
        event_list = ev;
    }
    else {
        event_list = ev;
        ev->next = NULL;
    }

    FD_SET(ev->fd, &event_fd_set);
}

void h9d_select_event_del(int fd) {
    for (h9d_select_event_t *ev = event_list; ev; ev = ev->next) {
        if (ev->fd == fd) {
            if (ev->prev) {
                ev->prev->next = ev->next;
            }
            else {
                event_list = ev->next;
            }

            if (ev->next) {
                ev->next->prev = ev->prev;
            }

            FD_CLR(ev->fd, &event_fd_set);
            free(ev);

            break;
        }
    }
}

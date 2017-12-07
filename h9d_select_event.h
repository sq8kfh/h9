#ifndef _H9D_SELECT_EVENT_H_
#define _H9D_SELECT_EVENT_H_

typedef struct h9d_select_event_t {
    int fd;

    struct h9d_select_event_t *prev;
    struct h9d_select_event_t *next;
} h9d_select_event_t;

void h9d_select_event_init(void);
void h9d_select_event_loop(void);

h9d_select_event_t *h9d_select_event_create(void);
void h9d_select_event_free(h9d_select_event_t *ev);
void h9d_select_event_add(h9d_select_event_t *ev);
void h9d_select_event_del(h9d_select_event_t *ev);

#endif //_H9D_SELECT_EVENT_H_

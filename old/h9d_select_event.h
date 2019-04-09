#ifndef _H9D_SELECT_EVENT_H_
#define _H9D_SELECT_EVENT_H_

#include <time.h>

#define H9D_SELECT_EVENT_READ (1<<0)
//#define H9D_SELECT_EVENT_TIME (1<<1)
#define H9D_SELECT_EVENT_DISCONNECT (1<<2)

#define H9D_SELECT_EVENT_RETURN_OK  0
#define H9D_SELECT_EVENT_RETURN_DEL 1
#define H9D_SELECT_EVENT_RETURN_DISCONNECT 2

typedef int (h9d_select_event_func_t)(void *ev_data, int event_type);

void h9d_select_event_init(void);
int h9d_select_event_loop(time_t time_trigger_period);
void h9d_select_event_stop(void);
void h9d_select_event_free(void);

void h9d_select_event_add(int fd, int event_types, h9d_select_event_func_t *func, void *ev_data);
void h9d_select_event_del(int fd);

#endif //_H9D_SELECT_EVENT_H_

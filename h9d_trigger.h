#ifndef _H9D_TRIGGER_H_
#define _H9D_TRIGGER_H_

#include <stdlib.h>
#include <stdint.h>

#define H9D_TRIGGER_ALL 0xffffffff
#define H9D_TRIGGER_TIMMER   (1<<0)
#define H9D_TRIGGER_RECV_MSG (1<<1)

typedef void (h9d_trigger_callback)(void *ud, uint32_t mask, void *param);

void h9d_trigger_init(void);
void h9d_trigger_free(void);
void h9d_trigger_call(uint32_t mask, void *param);

void h9d_trigger_add_listener(uint32_t mask, void *ud, h9d_trigger_callback *callback);
void h9d_trigger_del_listener(uint32_t mask, void *ud, h9d_trigger_callback *callback);

#endif //_H9D_TRIGGER_H_

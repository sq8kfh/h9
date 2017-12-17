#include "h9d_trigger.h"

typedef struct h9d_trigger_t{
    uint32_t mask;

    h9d_trigger_callback *callback;
    void *ud;

    struct h9d_trigger_t *next;
    struct h9d_trigger_t *prev;
} h9d_trigger_t;

static h9d_trigger_t *trigger_list_start;

static h9d_trigger_t *find(void *ud, h9d_trigger_callback *callback);

void h9d_trigger_init(void) {
    trigger_list_start = NULL;
}

void h9d_trigger_free(void) {
    while (trigger_list_start) {
        h9d_trigger_t *tmp = trigger_list_start;
        trigger_list_start = trigger_list_start->next;
        free(tmp);
    }
}

void h9d_trigger_call(uint32_t mask, void *param) {
    for (h9d_trigger_t *tr = trigger_list_start; tr; tr = tr->next) {
        if (tr->mask & mask) {
            tr->callback(tr->ud, mask, param);
        }
    }
}

void h9d_trigger_add_listener(uint32_t mask, void *ud, h9d_trigger_callback *callback) {
    h9d_trigger_t *tr = find(ud, callback);
    if (!tr) {
        tr = malloc(sizeof(h9d_trigger_t));
        tr->next = NULL;
        tr->prev = NULL;

        if (trigger_list_start) {
            tr->next = trigger_list_start;
            trigger_list_start->prev = tr;
            trigger_list_start = tr;
        }
        else {
            trigger_list_start = tr;
        }

        tr->ud = ud;
        tr->callback = callback;
    }
    tr->mask &= mask;
}

void h9d_trigger_del_listener(uint32_t mask, void *ud, h9d_trigger_callback *callback) {
    h9d_trigger_t *tr = find(ud, callback);

    if (!tr) {
        return;
    }

    tr->mask &= ~mask;

    if (!tr->mask) {
        if (tr->prev) {
            tr->prev->next = tr->next;
        } else {
            trigger_list_start = tr->next;
        }

        if (tr->next) {
            tr->next->prev = tr->prev;
        }
    }
}

static h9d_trigger_t *find(void *ud, h9d_trigger_callback *callback) {
    for (h9d_trigger_t *tr = trigger_list_start; tr; tr = tr->next) {
        if (tr->ud == ud && tr->callback == callback) {
            return tr;
        }
    }
    return NULL;
}

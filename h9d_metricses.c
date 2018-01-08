#include "h9d_metricses.h"
#include "h9d_trigger.h"
#include "h9_log.h"
#include "h9d_endpoint.h"
#include "h9d_client.h"

#include "config.h"
#include "h9_slcan.h"
#include <time.h>

#ifdef __APPLE__
#include <mach/mach_time.h>
#endif

#define TICKS_PER_SECOND 1000000000

static uint64_t metrices_clock;

static uint64_t get_metrices_clock(void) {
#ifdef __APPLE__
    return mach_absolute_time();
#else
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return tp.tv_sec * TICKS_PER_SECOND + tp.tv_nsec;
#endif
}

void h9d_metrices_init(void) {
    h9d_trigger_add_listener(H9D_TRIGGER_TIMMER, NULL, h9d_metrices_trigger_callback);
    metrices_clock = get_metrices_clock();
}

static h9_counter_t calc_diff_and_update_last(h9_counter_t *last_readed, h9_counter_t counter) {
    h9_counter_t counter_tmp = *last_readed;
    *last_readed = counter;
    return counter - counter_tmp;
}

void h9d_metrices_trigger_callback(void *ud, uint32_t mask, void *param) {
    if (mask == H9D_TRIGGER_TIMMER) {
        uint64_t tmp = metrices_clock;
        metrices_clock = get_metrices_clock();
        uint64_t delta_time = metrices_clock - tmp;

        for (h9d_endpoint_t *i = h9d_endpoint_first_endpoint(); i; i = h9d_endpoint_getnext_endpoint(i)) {
            h9_counter_t delta_recv_msg_counter =
                    calc_diff_and_update_last(&i->last_readed_recv_msg_counter, i->recv_msg_counter);

            h9_counter_t delta_recv_invalid_msg_counter =
                    calc_diff_and_update_last(&i->last_readed_recv_invalid_msg_counter, i->recv_invalid_msg_counter);

            h9_counter_t delta_send_msg_counter =
                    calc_diff_and_update_last(&i->last_readed_send_msg_counter, i->send_msg_counter);

            h9_counter_t delta_throttled_counter =
                    calc_diff_and_update_last(&i->last_readed_throttled_counter, i->throttled_counter);

            h9_log_err("endpoint metrices: %s; recv %.1lf m/s; inv %.1lf m/s; send %.1lf m/s; throttled %.1lf m/s",
                       i->endpoint_name,
                       (double)delta_recv_msg_counter/delta_time*TICKS_PER_SECOND,
                       (double)delta_recv_invalid_msg_counter/delta_time*TICKS_PER_SECOND,
                       (double)delta_send_msg_counter/delta_time*TICKS_PER_SECOND,
                       (double)delta_throttled_counter/delta_time*TICKS_PER_SECOND);

            h9_counter_t delta_r =
                    calc_diff_and_update_last(&i->ep_imp->last_readed_read_byte_counter, i->ep_imp->read_byte_counter);

            h9_counter_t delta_w =
                    calc_diff_and_update_last(&i->ep_imp->last_readed_write_byte_counter, i->ep_imp->write_byte_counter);

            h9_log_err("slcan metrices: %s; read %.1lf B/s; write %.1lf B/s",
                       "slcan0",
                       (double)delta_r/delta_time*TICKS_PER_SECOND,
                       (double)delta_w/delta_time*TICKS_PER_SECOND);
        }

        for (h9d_client_t *i = h9d_client_first_client(); i; i = h9d_client_getnext_client(i)) {
            h9_counter_t delta_recv_xmlmsg_counter =
                    calc_diff_and_update_last(&i->last_readed_recv_xmlmsg_counter, i->recv_xmlmsg_counter);

            h9_counter_t delta_recv_invalid_xmlmsg_counter =
                    calc_diff_and_update_last(&i->last_readed_recv_invalid_xmlmsg_counter, i->recv_invalid_xmlmsg_counter);

            h9_counter_t delta_send_msg_counter =
                    calc_diff_and_update_last(&i->last_readed_send_xmlmsg_counter, i->send_xmlmsg_counter);

            h9_log_err("client metrices: recv %.1lf xm/s; inv %.1lf xm/s; send %.1lf xm/s",
                       (double)delta_recv_xmlmsg_counter/delta_time*TICKS_PER_SECOND,
                       (double)delta_recv_invalid_xmlmsg_counter/delta_time*TICKS_PER_SECOND,
                       (double)delta_send_msg_counter/delta_time*TICKS_PER_SECOND);
        }

    }
}

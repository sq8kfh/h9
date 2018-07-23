#include "h9d_metricses.h"
#include "h9d_trigger.h"
#include "h9_log.h"
#include "h9d_endpoint.h"
#include "h9d_client.h"
#include "h9_xmlmsg.h"

#include "config.h"
#include <time.h>

#ifdef __APPLE__
#include <mach/mach_time.h>
#endif

#define TICKS_PER_SECOND 1000000000

static time_t start_time;

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
    start_time = time(0);
}

static h9_counter_t calc_diff_and_update_last(h9_counter_t *last_readed, h9_counter_t counter) {
    h9_counter_t counter_tmp = *last_readed;
    *last_readed = counter;
    return counter - counter_tmp;
}

void h9d_metrices_trigger_callback(void *ud, uint32_t mask, void *param) {
    const size_t buffer_size = 100;
    char name_buffer [buffer_size];
    char value_buffer [buffer_size];

    if (mask == H9D_TRIGGER_TIMMER) {
        h9_xmlmsg_t *xmlmsg = h9_xmlmsg_init(H9_XMLMSG_METRICSES);

        uint64_t tmp = metrices_clock;
        metrices_clock = get_metrices_clock();
        uint64_t delta_time = metrices_clock - tmp;

        snprintf(name_buffer, buffer_size, "uptime");
        snprintf(value_buffer, buffer_size, "%ld",  time(0) - start_time);
        h9_xmlmsg_add_metrics(xmlmsg, name_buffer, value_buffer);

        for (h9d_endpoint_t *i = h9d_endpoint_first_endpoint(); i; i = h9d_endpoint_getnext_endpoint(i)) {
            snprintf(name_buffer, buffer_size, "endpoint.%s.%s", i->endpoint_name, "recv_msg_counter");
            snprintf(value_buffer, buffer_size, "%u", i->recv_msg_counter);
            h9_xmlmsg_add_metrics(xmlmsg, name_buffer, value_buffer);

            snprintf(name_buffer, buffer_size, "endpoint.%s.%s", i->endpoint_name, "recv_invalid_msg_counter");
            snprintf(value_buffer, buffer_size, "%u", i->recv_invalid_msg_counter);
            h9_xmlmsg_add_metrics(xmlmsg, name_buffer, value_buffer);

            snprintf(name_buffer, buffer_size, "endpoint.%s.%s", i->endpoint_name, "send_msg_counter");
            snprintf(value_buffer, buffer_size, "%u", i->send_msg_counter);
            h9_xmlmsg_add_metrics(xmlmsg, name_buffer, value_buffer);

            snprintf(name_buffer, buffer_size, "endpoint.%s.%s", i->endpoint_name, "throttled_counter");
            snprintf(value_buffer, buffer_size, "%u", i->throttled_counter);
            h9_xmlmsg_add_metrics(xmlmsg, name_buffer, value_buffer);

            h9_counter_t delta_recv_msg_counter =
                    calc_diff_and_update_last(&i->last_readed_recv_msg_counter, i->recv_msg_counter);

            h9_counter_t delta_recv_invalid_msg_counter =
                    calc_diff_and_update_last(&i->last_readed_recv_invalid_msg_counter, i->recv_invalid_msg_counter);

            h9_counter_t delta_send_msg_counter =
                    calc_diff_and_update_last(&i->last_readed_send_msg_counter, i->send_msg_counter);

            h9_counter_t delta_throttled_counter =
                    calc_diff_and_update_last(&i->last_readed_throttled_counter, i->throttled_counter);

            h9_log_debug("endpoint %s metrices: recv %.1lf m/s; inv %.1lf m/s; send %.1lf m/s; throttled %.1lf m/s",
                       i->endpoint_name,
                       (double)delta_recv_msg_counter/delta_time*TICKS_PER_SECOND,
                       (double)delta_recv_invalid_msg_counter/delta_time*TICKS_PER_SECOND,
                       (double)delta_send_msg_counter/delta_time*TICKS_PER_SECOND,
                       (double)delta_throttled_counter/delta_time*TICKS_PER_SECOND);
        }

        for (h9d_client_t *i = h9d_client_first_client(); i; i = h9d_client_getnext_client(i)) {
            snprintf(name_buffer, buffer_size, "client.%p.%s", i->xmlsocket, "recv_xmlmsg_counter");
            snprintf(value_buffer, buffer_size, "%u", i->recv_xmlmsg_counter);
            h9_xmlmsg_add_metrics(xmlmsg, name_buffer, value_buffer);

            snprintf(name_buffer, buffer_size, "client.%p.%s", i->xmlsocket, "recv_invalid_xmlmsg_counter");
            snprintf(value_buffer, buffer_size, "%u", i->recv_invalid_xmlmsg_counter);
            h9_xmlmsg_add_metrics(xmlmsg, name_buffer, value_buffer);

            snprintf(name_buffer, buffer_size, "client.%p.%s", i->xmlsocket, "send_xmlmsg_counter");
            snprintf(value_buffer, buffer_size, "%u", i->send_xmlmsg_counter);
            h9_xmlmsg_add_metrics(xmlmsg, name_buffer, value_buffer);

            h9_counter_t delta_recv_xmlmsg_counter =
                    calc_diff_and_update_last(&i->last_readed_recv_xmlmsg_counter, i->recv_xmlmsg_counter);

            h9_counter_t delta_recv_invalid_xmlmsg_counter =
                    calc_diff_and_update_last(&i->last_readed_recv_invalid_xmlmsg_counter, i->recv_invalid_xmlmsg_counter);

            h9_counter_t delta_send_msg_counter =
                    calc_diff_and_update_last(&i->last_readed_send_xmlmsg_counter, i->send_xmlmsg_counter);

            h9_log_debug("client %p metrices: recv %.1lf xm/s; inv %.1lf xm/s; send %.1lf xm/s",
                       i->xmlsocket,
                       (double)delta_recv_xmlmsg_counter/delta_time*TICKS_PER_SECOND,
                       (double)delta_recv_invalid_xmlmsg_counter/delta_time*TICKS_PER_SECOND,
                       (double)delta_send_msg_counter/delta_time*TICKS_PER_SECOND);
        }

        size_t xml_length;
        char *msg = h9_xmlmsg_build(xmlmsg, &xml_length, 1);

        h9d_trigger_call(H9D_TRIGGER_METRICSES, msg);

        h9_xmlmsg_free(xmlmsg);
        free(msg);
    }
}

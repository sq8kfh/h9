#include "h9_log.h"

#include <stdio.h>

void h9_log_init(void) {

}

void h9_log_write(unsigned int level, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    h9_log_vwrite(level, fmt, args);

    va_end(args);
}

void h9_log_vwrite(unsigned int level, const char *fmt, va_list args) {
    switch (level) {
        case H9_LOG_STDERR:
            vfprintf(stderr, fmt, args);
            break;
        default:
            vprintf(fmt, args);
            break;
    }
}

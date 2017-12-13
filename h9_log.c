#include "h9_log.h"

#include <stdio.h>

static int _verbose;
static int _debug;

void h9_log_init(int verbose, int debug) {
    _verbose = verbose;
    _debug = debug;
}

void h9_log_set_verbose(int level) {
    _verbose = level;
}

void h9_log_write(unsigned int level, const char *file, int line_num, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    h9_log_vwrite(level, file, line_num, fmt, args);

    va_end(args);
}

void h9_log_vwrite(unsigned int level, const char *file, int line_num, const char *fmt, va_list args) {
    if (_verbose < level) {
        return;
    }
    if (_debug) {
        switch (level) {
            case H9_LOG_STDERR:
                fprintf(stderr, "%s:%d: ", file, line_num);
                break;
            default:
                printf("%s:%d: ", file, line_num);
                break;
        }
    }
    switch (level) {
        case H9_LOG_STDERR:
            vfprintf(stderr, fmt, args);
            fprintf(stderr, "\n");
            break;
        default:
            vprintf(fmt, args);
            printf("\n");
            break;
    }
}

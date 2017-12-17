#include "h9_log.h"

#include <stdio.h>

static int _verbose;
static int _debug;
static int _all_to_stderr = 0;
static char *level_name[7] = {"", "CRIT", "ERROR", "WARN", "NOTICE", "INFO", "DEBUG"};

void h9_log_init(int verbose, int debug, int all_to_stderr) {
    _verbose = verbose;
    _debug = debug;
    _all_to_stderr = all_to_stderr;
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

    FILE *out = stdout;
    if (_all_to_stderr || level == H9_LOG_STDERR) {
        out = stderr;
    }

    switch (level) {
        case H9_LOG_STDERR:
            break;
        default:
            fprintf(out, "%-6s ", level_name[level]);
            break;
    }
    if (_debug) {
        fprintf(out, "%s:%d: ", file, line_num);
    }

    vfprintf(out, fmt, args);
    fprintf(out, "\n");
}

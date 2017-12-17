#include "h9d_cfg.h"
#include "h9_log.h"

#include <stdlib.h>

#include <confuse.h>

cfg_opt_t opts[] = {
        CFG_STR("pid_file", NULL, CFGF_NONE),
        CFG_INT("client_recv_buffer_size", 100, CFGF_NONE),
        CFG_BOOL("xmlmsg_schema_validation", cfg_true, CFGF_NONE),
        CFG_INT("time_trigger_period", 5, CFGF_NONE),
        CFG_BOOL("daemonize", cfg_true, CFGF_NONE),
        CFG_INT("verbose", H9_LOG_WARN, CFGF_NONE),
        CFG_END()
};

cfg_t *cfg = NULL;

static void h9d_cfg_free(void) {
    if (cfg)
        cfg_free(cfg);
}

void h9d_cfg_init(const char *cfg_file) {
    cfg = cfg_init(opts, CFGF_NONE);

    int result = cfg_parse(cfg, cfg_file);
    if (result == CFG_PARSE_ERROR)
        exit(EXIT_FAILURE);
    else if (result == CFG_FILE_ERROR) {
        h9_log_stderr("can't open cfg file: %s\n", cfg_file);
        exit(EXIT_FAILURE);
    }

    atexit(h9d_cfg_free);
}

void h9d_cfg_setbool(const char *name, unsigned int value) {
    cfg_setbool(cfg, name, value);
}

void h9d_cfg_setint(const char *name, int value) {
    cfg_setint(cfg, name, value);
}

void h9d_cfg_setstr(const char *name, const char *value) {
    cfg_setstr(cfg, name, value);
}

unsigned int  h9d_cfg_getbool(const char *name) {
    return cfg_getbool(cfg, name) == cfg_true;
}

int h9d_cfg_getint(const char *name) {
    return cfg_getint(cfg, name);
}

char *h9d_cfg_getstr(const char *name) {
    return cfg_getstr(cfg, name);
}

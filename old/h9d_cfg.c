#include "h9d_cfg.h"
#include "common/h9_log.h"

#include <stdlib.h>

#include <confuse.h>

static cfg_opt_t endpoint_opts[] = {
        CFG_STR("connect", NULL, CFGF_NONE),
        //CFG_INT("recv_buf_size", 20, CFGF_NONE),
        CFG_INT("throttle_level", 10, CFGF_NONE),
        //CFG_BOOL("nonblock", cfg_false, CFGF_NONE),
        CFG_BOOL("auto_respawn", cfg_false, CFGF_NONE),
        CFG_INT("id", 0, CFGF_NODEFAULT),
        CFG_END()
};

static cfg_opt_t opts[] = {
        CFG_STR("pid_file", NULL, CFGF_NONE),
        CFG_INT("client_recv_buffer_size", 100, CFGF_NONE),
        CFG_BOOL("xmlmsg_schema_validation", cfg_true, CFGF_NONE),
        CFG_INT("time_trigger_period", 5, CFGF_NONE),
        CFG_BOOL("daemonize", cfg_true, CFGF_NONE),
        CFG_INT("verbose", H9_LOG_WARN, CFGF_NONE),
        CFG_SEC("endpoint", endpoint_opts, CFGF_MULTI | CFGF_TITLE),
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

unsigned int h9d_cfg_getbool(const char *name) {
    return cfg_getbool(cfg, name) == cfg_true;
}

int h9d_cfg_getint(const char *name) {
    return cfg_getint(cfg, name);
}

char *h9d_cfg_getstr(const char *name) {
    return cfg_getstr(cfg, name);
}

static cfg_t *cfg_endpoint = NULL;
static int i_endpoint = 0;

const char *h9d_cfg_endpoint(void) {
    if (i_endpoint < cfg_size(cfg, "endpoint")) {
        cfg_endpoint = cfg_getnsec(cfg, "endpoint", i_endpoint);
        i_endpoint++;
        return cfg_title(cfg_endpoint);
    }
    return NULL;
}

unsigned int h9d_cfg_endpoint_getbool(const char *name) {
    if (cfg_endpoint) {
        return cfg_getbool(cfg_endpoint, name) == cfg_true;
    }
    h9_log_crit("first call h9d_cfg_endpoint");
    exit(EXIT_FAILURE);
}

int h9d_cfg_endpoint_getint(const char *name) {
    if (cfg_endpoint) {
        return cfg_getint(cfg_endpoint, name);
    }
    exit(EXIT_FAILURE);
}

char *h9d_cfg_endpoint_getstr(const char *name) {
    if (cfg_endpoint) {
        return cfg_getstr(cfg_endpoint, name);
    }
    exit(EXIT_FAILURE);
}

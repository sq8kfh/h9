/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-04-15.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "busctx.h"
#include <cstdlib>
#include "common/logger.h"


static void cfg_err_func(cfg_t *cfg, const char* fmt, va_list args) {
    Logger::default_log.vlog(Log::Level::WARN, __FILE__, __LINE__, fmt, args);
}

void BusCtx::load_configuration(const std::string& conf_filename, bool override_daemonize,
        const std::string& override_logfile, const std::string& override_pidfile) {
    cfg_opt_t cfg_process_sec[] = {
            CFG_BOOL("daemonize", cfg_false, CFGF_NONE),
            CFG_STR("pidfile", nullptr, CFGF_NONE),
            CFG_INT("setuid", 0, CFGF_NONE),
            CFG_INT("setgid", 0, CFGF_NONE),
            CFG_END()
    };

    cfg_opt_t cfg_server_sec[] = {
            CFG_INT("port", H9_BUS_DEFAULT_PORT, CFGF_NONE),
            CFG_END()
    };

    cfg_opt_t cfg_log_sec[] = {
            CFG_STR("logfile", nullptr, CFGF_NONE),
            CFG_STR("send_frame_logfile", nullptr, CFGF_NONE),
            CFG_STR("recv_frame_logfile", nullptr, CFGF_NONE),
            CFG_END()
    };

    cfg_opt_t cfg_bus_sec[] = {
            CFG_STR("driver", nullptr, CFGF_NONE),
            CFG_STR("connection_string", nullptr, CFGF_NONE),
            CFG_END()
    };

    cfg_opt_t cfg_opts[] = {
            CFG_SEC("process", cfg_process_sec, CFGF_NONE),
            CFG_SEC("server", cfg_server_sec, CFGF_NONE),
            CFG_SEC("log", cfg_log_sec, CFGF_NONE),
            CFG_SEC("bus", cfg_bus_sec, CFGF_MULTI | CFGF_TITLE | CFGF_NO_TITLE_DUPES),
            CFG_END()
    };

    cfg = cfg_init(cfg_opts, CFGF_NONE);
    cfg_set_error_function(cfg, cfg_err_func);
    int ret = cfg_parse(cfg, conf_filename.c_str());

    if (ret == CFG_FILE_ERROR) {
        h9_log_stderr("config file error");
        cfg_free(cfg);
        exit(EXIT_FAILURE);
    } else if (ret == CFG_PARSE_ERROR) {
        h9_log_stderr("config file parse error");
        cfg_free(cfg);
        exit(EXIT_FAILURE);
    }

    if (override_daemonize)
        cfg_setbool(cfg, "process|daemonize", cfg_true);
    if (!override_logfile.empty())
        cfg_setstr(cfg, "log|logfile", override_logfile.c_str());
    if (!override_pidfile.empty())
        cfg_setstr(cfg, "process|pidfile", override_pidfile.c_str());

    if (cfg_log_logfile() != "stdout") {
        logger().redirect_to_file(cfg_log_logfile());
    }
}

BusCtx::~BusCtx() {
    cfg_free(cfg);
}

BusCtx::BusCtx(): DaemonCtx("h9bus", "H9 Bus daemon.") {
}

std::string BusCtx::cfg_pidfile() {
    cfg_t *cfg_process = cfg_getsec(cfg, "process");
    if (cfg_process && cfg_getstr(cfg_process, "pidfile")) {
        return std::string(cfg_getstr(cfg_process, "pidfile"));
    }
    return "";
}

bool BusCtx::cfg_daemonize() {
    cfg_t *cfg_process = cfg_getsec(cfg, "process");
    if (cfg_process) {
        return cfg_getbool(cfg_process, "daemonize");
    }
    return false;
}

bool BusCtx::cfg_drop_privileges() {
    cfg_t *cfg_process = cfg_getsec(cfg, "process");
    if (cfg_process) {
        return cfg_getint(cfg_process, "setuid") > 0 || cfg_getint(cfg_process, "setgid") > 0;
    }
    return false;
}

int BusCtx::cfg_drop_privileges_uid() {
    cfg_t *cfg_process = cfg_getsec(cfg, "process");
    if (cfg_process) {
        return cfg_getint(cfg_process, "setuid");
    }
    return 0;
}

int BusCtx::cfg_drop_privileges_gid() {
    cfg_t *cfg_process = cfg_getsec(cfg, "process");
    if (cfg_process) {
        return cfg_getint(cfg_process, "setgid");
    }
    return 0;
}

uint16_t BusCtx::cfg_server_port() {
    cfg_t *cfg_process = cfg_getsec(cfg, "server");
    if (cfg_process) {
        return cfg_getint(cfg_process, "port");
    }
    return H9_BUS_DEFAULT_PORT;
}

std::string BusCtx::cfg_log_logfile() {
    cfg_t *cfg_log = cfg_getsec(cfg, "log");
    if (cfg_log && cfg_getstr(cfg_log, "logfile")) {
        return std::string(cfg_getstr(cfg_log, "logfile"));
    }
    return "stdout";
}

std::string BusCtx::cfg_log_send_logfile() {
    cfg_t *cfg_log = cfg_getsec(cfg, "log");
    if (cfg_log && cfg_getstr(cfg_log, "send_frame_logfile")) {
        return std::string(cfg_getstr(cfg_log, "send_frame_logfile"));
    }
    return "";
}

std::string BusCtx::cfg_log_recv_logfile() {
    cfg_t *cfg_log = cfg_getsec(cfg, "log");
    if (cfg_log && cfg_getstr(cfg_log, "recv_frame_logfile")) {
        return std::string(cfg_getstr(cfg_log, "recv_frame_logfile"));
    }
    return "";
}

std::vector<std::string> BusCtx::cfg_bus_list() {
    int n = cfg_size(cfg, "bus");
    std::vector<std::string> ret;
    for (int i = 0; i < n; i++) {
        cfg_t *bus = cfg_getnsec(cfg, "bus", i);
        ret.emplace_back(cfg_title(bus));
    }
    return std::move(ret);
}

std::string BusCtx::cfg_bus_driver(const std::string &bus) {
    cfg_t *cfg_bus = cfg_gettsec(cfg, "bus", bus.c_str());
    if (cfg_bus && cfg_getstr(cfg_bus, "driver")) {
        return std::string(cfg_getstr(cfg_bus, "driver"));
    }
    return "";
}

std::string BusCtx::cfg_bus_connection_string(const std::string &bus) {
    cfg_t *cfg_bus = cfg_gettsec(cfg, "bus", bus.c_str());
    if (cfg_bus && cfg_getstr(cfg_bus, "connection_string")) {
        return std::string(cfg_getstr(cfg_bus, "connection_string"));
    }
    return "";
}

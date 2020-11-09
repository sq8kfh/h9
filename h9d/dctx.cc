/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "dctx.h"
#include <cstdlib>
#include "common/logger.h"


static void cfg_err_func(cfg_t *cfg, const char* fmt, va_list args) {
    Logger::default_log.vlog(Log::Level::WARN, __FILE__, __LINE__, fmt, args);
}

void DCtx::load_configuration(const std::string& conf_filename, bool override_daemonize,
                                const std::string& override_logfile, const std::string& override_pidfile) {
    cfg_opt_t cfg_process_sec[] = {
            CFG_BOOL("daemonize", cfg_false, CFGF_NONE),
            CFG_STR("pidfile", nullptr, CFGF_NONE),
            CFG_INT("setuid", 0, CFGF_NONE),
            CFG_INT("setgid", 0, CFGF_NONE),
            CFG_END()
    };

    cfg_opt_t cfg_server_sec[] = {
            CFG_INT("port", H9D_DEFAULT_PORT, CFGF_NONE),
            CFG_END()
    };

    cfg_opt_t cfg_bus_sec[] = {
            CFG_STR("h9bus_hostname", "localhost", CFGF_NONE),
            CFG_INT("h9bus_port", H9_BUS_DEFAULT_PORT, CFGF_NONE),
            CFG_END()
    };

    cfg_opt_t cfg_log_sec[] = {
            CFG_STR("logfile", nullptr, CFGF_NONE),
            CFG_STR("send_frame_logfile", nullptr, CFGF_NONE),
            CFG_STR("recv_frame_logfile", nullptr, CFGF_NONE),
            CFG_END()
    };

    cfg_opt_t cfg_opts[] = {
            CFG_SEC("process", cfg_process_sec, CFGF_NONE),
            CFG_SEC("server", cfg_server_sec, CFGF_NONE),
            CFG_SEC("bus", cfg_bus_sec, CFGF_NONE),
            CFG_SEC("log", cfg_log_sec, CFGF_NONE),
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

DCtx::~DCtx() {
    cfg_free(cfg);
}

DCtx::DCtx(): DaemonCtx("h9d", "H9 daemon.") {
}

std::string DCtx::cfg_pidfile() {
    cfg_t *cfg_process = cfg_getsec(cfg, "process");
    if (cfg_process && cfg_getstr(cfg_process, "pidfile")) {
        return std::string(cfg_getstr(cfg_process, "pidfile"));
    }
    return "";
}

bool DCtx::cfg_daemonize() {
    cfg_t *cfg_process = cfg_getsec(cfg, "process");
    if (cfg_process) {
        return cfg_getbool(cfg_process, "daemonize");
    }
    return false;
}

bool DCtx::cfg_drop_privileges() {
    cfg_t *cfg_process = cfg_getsec(cfg, "process");
    if (cfg_process) {
        return cfg_getint(cfg_process, "setuid") > 0 || cfg_getint(cfg_process, "setgid") > 0;
    }
    return false;
}

int DCtx::cfg_drop_privileges_uid() {
    cfg_t *cfg_process = cfg_getsec(cfg, "process");
    if (cfg_process) {
        return cfg_getint(cfg_process, "setuid");
    }
    return 0;
}

int DCtx::cfg_drop_privileges_gid() {
    cfg_t *cfg_process = cfg_getsec(cfg, "process");
    if (cfg_process) {
        return cfg_getint(cfg_process, "setgid");
    }
    return 0;
}

uint16_t DCtx::cfg_server_port() {
    cfg_t *cfg_process = cfg_getsec(cfg, "server");
    if (cfg_process) {
        return cfg_getint(cfg_process, "port");
    }
    return H9_BUS_DEFAULT_PORT;
}

std::string DCtx::cfg_h9bus_hostname() {
    cfg_t *bus_process = cfg_getsec(cfg, "bus");
    if (bus_process && cfg_getstr(bus_process, "h9bus_hostname")) {
        return std::string(cfg_getstr(bus_process, "h9bus_hostname"));
    }
    return "localhost";
}

uint16_t DCtx::cfg_h9bus_port() {
    cfg_t *bus_process = cfg_getsec(cfg, "bus");
    if (bus_process) {
        return cfg_getint(bus_process, "h9bus_port");
    }
    return H9D_DEFAULT_PORT;
}

std::string DCtx::cfg_log_logfile() {
    cfg_t *cfg_log = cfg_getsec(cfg, "log");
    if (cfg_log && cfg_getstr(cfg_log, "logfile")) {
        return std::string(cfg_getstr(cfg_log, "logfile"));
    }
    return "stdout";
}
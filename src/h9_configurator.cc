/*
 * H9 project
 *
 * Created by crowx on 2023-09-16.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#include "h9_configurator.h"

#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "git_version.h"

namespace {
void cfg_err_func(cfg_t* cfg, const char* fmt, va_list args) {
    /*std::string fmt_str = {fmt};
    fmt_str.erase(std::find_if(fmt_str.rbegin(), fmt_str.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), fmt_str.end());*/
    // Logger::default_log.vlog(Log::Level::INFO, __FILE__, __LINE__, fmt, args);
}
} // namespace

H9Configurator::H9Configurator(const std::string& app_name, const std::string& app_desc):
    options(app_name, app_desc),
    _app_name(app_desc),
    _app_desc(app_desc) {
    host = "";
    port = "";
    source_id = default_source_id;
}

cxxopts::ParseResult H9Configurator::parse_command_line_arg(int argc, char** argv) {
    // clang-format off
    options.add_options("other")
#ifdef H9_DEBUG
            ("D,debug", "Enable debugging")
#endif
            ("h,help", "Show help")
            ("v,verbose", "Be more verbose")
            ("V,version", "Show version")
            ;
    options.add_options("connection")
            ("c,connect", "Connection address", cxxopts::value<std::string>())
            ("p,port", "Connection port", cxxopts::value<int>()->default_value(std::to_string(default_h9d_port)))
            ("F,config", "User config file", cxxopts::value<std::string>()->default_value(default_user_config))
            ;
    // clang-format on

    add_app_specific_opt();

    try {
        cxxopts::ParseResult result = options.parse(argc, argv);

        debug = result.count("debug");

        if (result.count("help")) {
            std::cerr << options.help({"", "connection", "other"}) << std::endl;
            exit(EXIT_SUCCESS);
        }

        if (result.count("version")) {
            std::cerr << _app_name << " version " << H9_VERSION << " by crowx." << std::endl;
#ifdef GITVERSION_COMMIT_SHA
#ifdef GITVERSION_DIRTY
            constexpr char workdir[] = "dirty";
#else
            constexpr char workdir[] = "clean";
#endif
            std::cerr << "H9 git commit: " << GITVERSION_COMMIT_SHA << ", working directory " << workdir << "." << std::endl;
#endif
            std::cerr << "Copyright (C) 2017-2023 Kamil Palkowski. All rights reserved." << std::endl;
            exit(EXIT_SUCCESS);
        }

        verbose = result.count("verbose");

        if (result.count("connect")) {
            host = std::string(result["connect"].as<std::string>());
        }

        if (result.count("port")) {
            port = std::to_string(result["port"].as<int>());
        }

        config_file = std::string(result["config"].as<std::string>());


        parse_app_specific_opt(result);

        return result;
    }
    catch (cxxopts::exceptions::parsing e) {
        std::cerr << e.what() << ". Try '-h' for more information." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void H9Configurator::load_configuration() {
    cfg_opt_t cfg_h9bus_opts[] = {
        CFG_STR("HostName", nullptr, CFGF_NONE | CFGF_NODEFAULT),
        CFG_INT("Port", 0, CFGF_NONE | CFGF_NODEFAULT),
        CFG_INT("SourceID", 0, CFGF_NONE | CFGF_NODEFAULT),
        CFG_END()};
    cfg_opt_t cfg_opts[] = {
        CFG_INT("DefaultSourceID", default_source_id, CFGF_NONE),
        CFG_SEC("h9d", cfg_h9bus_opts, CFGF_MULTI | CFGF_TITLE),
        CFG_END()};

    cfg = cfg_init(cfg_opts, CFGF_NONE);

    cfg_set_error_function(cfg, cfg_err_func);

    int result = cfg_parse(cfg, config_file.c_str());
    if (result == CFG_PARSE_ERROR) {
        SPDLOG_WARN("Can't parse cfg file: {}.", config_file);
        cfg_free(cfg);
        cfg = nullptr;
        if (config_file != default_config) {
            config_file = default_config;
            load_configuration();
        }
    }
    else if (result == CFG_FILE_ERROR) {
        SPDLOG_WARN("Can't open cfg file: '{}'.", config_file);
        cfg_free(cfg);
        cfg = nullptr;
        if (config_file != default_config) {
            config_file = default_config;
            load_configuration();
        }
    }

    if (host.empty()) {
        host = std::string(default_h9d_host);
    }

    if (cfg) {
        source_id = cfg_getint(cfg, "DefaultSourceID");

        for (int i = 0; i < cfg_size(cfg, "h9d"); i++) {
            cfg_t* h9d_sec = cfg_getnsec(cfg, "h9d", i);
            if (host == cfg_title(h9d_sec)) {
                if (cfg_size(h9d_sec, "SourceID")) {
                    source_id = cfg_getint(h9d_sec, "SourceID");
                }
                if (cfg_size(h9d_sec, "HostName")) {
                    host = cfg_getstr(h9d_sec, "HostName");
                }
                if (port.empty() && cfg_size(h9d_sec, "Port")) {
                    port = std::to_string(cfg_getint(h9d_sec, "Port"));
                }
                break;
            }
        }
    }
    if (port.empty()) {
        port = std::to_string(default_h9d_port);
    }
}

void H9Configurator::logger_initial_setup() {
    auto h9 = spdlog::stderr_color_st("stderr");
    spdlog::set_default_logger(h9);
}

void H9Configurator::logger_setup() {
    auto h9 = spdlog::default_logger();
    if (debug)
        h9->set_pattern(log_debug_pattern);
    else
        h9->set_pattern(log_pattern);

    int tmp_level = spdlog::level::err - verbose;
    if (tmp_level < spdlog::level::trace)
        h9->set_level(spdlog::level::trace);
    else
        h9->set_level(static_cast<spdlog::level::level_enum>(tmp_level));
}

H9Connector H9Configurator::get_connector() {
    return H9Connector(host, port);
}

std::uint16_t H9Configurator::get_default_source_id() {
    return source_id;
}

std::string H9Configurator::get_host() const {
    return host;
}

std::string H9Configurator::get_port() const {
    return port;
}

bool H9Configurator::get_debug() const {
    return debug;
}

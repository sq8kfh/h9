/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-17.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "clientctx.h"
#include "logger.h"


ClientCtx::ClientCtx(const std::string& app_name, const std::string& app_desc):
    Ctx(app_name, app_desc),
    _options(app_name, app_desc),
    cfg_h9bus_opts {
        CFG_STR("HostName", nullptr, CFGF_NONE),
        CFG_INT("Port", 0, CFGF_NONE),
        CFG_END()
    },
    cfg_opts {
        CFG_SEC("H9bus", cfg_h9bus_opts, CFGF_MULTI | CFGF_TITLE),
        CFG_END()
    } {
    cfg = nullptr;
    _options.add_options("other")
#ifdef H9_DEBUG
            ("d,debug", "Enable debugging")
#endif
            ("h,help", "Show help")
            ("v,verbose", "Be more verbose")
            ("V,version", "Show version")
            ;
    _options.add_options("connection")
            ("c,connect", "Connection address", cxxopts::value<std::string>())
            ("p,port", "Connection port", cxxopts::value<int>()->default_value(std::to_string(H9_BUS_DEFAULT_PORT)))
            ("F,config", "User config file", cxxopts::value<std::string>()->default_value(H9_USER_CONFIG_FILE))
            ;

    h9bus_host = "127.0.0.1";
    h9bus_port = std::to_string(H9_BUS_DEFAULT_PORT);
}

static void cfg_err_func(cfg_t *cfg, const char* fmt, va_list args) {
    /*std::string fmt_str = {fmt};
    fmt_str.erase(std::find_if(fmt_str.rbegin(), fmt_str.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), fmt_str.end());*/
    Logger::default_log.vlog(Log::Level::INFO, __FILE__, __LINE__, fmt, args);
}

void ClientCtx::load_configuration(const cxxopts::ParseResult& opts) {
    cfg = cfg_init(cfg_opts, CFGF_NONE);

    cfg_set_error_function(cfg, cfg_err_func);

    int result = cfg_parse(cfg, opts["config"].as<std::string>().c_str());
    if (result == CFG_PARSE_ERROR) {
        h9_log_warn("can't parse cfg file: %s", opts["config"].as<std::string>().c_str());
        cfg_free(cfg);
        cfg = nullptr;
    }
    else if (result == CFG_FILE_ERROR) {
        h9_log_info("can't open cfg file: %s", opts["config"].as<std::string>().c_str());
        cfg_free(cfg);
        cfg = nullptr;
    }
    else {
        if (opts.count("connect") == 1 && opts.count("port") == 1) {
            h9bus_host = opts["connect"].as<std::string>();
            h9bus_port = std::to_string(opts["port"].as<int>());
        }
        else {
            std::string tmp_host = "default";
            h9bus_host = "127.0.0.1";
            h9bus_port = std::to_string(H9_BUS_DEFAULT_PORT);
            if (opts.count("connect") == 1) {
                tmp_host = opts["connect"].as<std::string>();
                h9bus_host = tmp_host;
            }
            if (opts.count("port") == 1) {
                h9bus_port = std::to_string(opts["port"].as<int>());
            }
            for (int i = 0; i < cfg_size(cfg, "H9bus"); i++) {
                cfg_t *h9bus_sec = cfg_getnsec(cfg, "H9bus", i);
                if (tmp_host == cfg_title(h9bus_sec)) {
                    h9bus_host = cfg_getstr(h9bus_sec, "HostName");
                    if (opts.count("port") == 0) {
                        h9bus_port = std::to_string(cfg_getint(h9bus_sec, "Port"));
                    }
                }
            }
        }
    }
    log().set_to_stderr(true);
}

void ClientCtx::add_options(const std::string& opts,
                      const std::string& desc,
                      std::shared_ptr<const cxxopts::Value> value,
                      std::string arg_help) {
    _options.add_options()(opts, desc, std::move(value), std::move(arg_help));
}

void ClientCtx::add_positional_options_list(const std::string& opts, const std::string& desc, const std::string& help) {
    _options.parse_positional(opts);
    _options.positional_help(desc);

    _options.add_options()(opts, help, cxxopts::value<std::vector<std::string>>());
}

void ClientCtx::add_positional_option(const std::string& opts, const std::string& desc, const std::string& help) {
    _options.parse_positional(opts);
    _options.positional_help(desc);

    _options.add_options()(opts, help, cxxopts::value<std::string>());
}

cxxopts::ParseResult ClientCtx::parse_options(int argc, char* argv[]) {
    try {
        cxxopts::ParseResult result = _options.parse(argc, argv);
#ifdef H9_DEBUG
        if (result.count("debug")) {
            enable_debug(true);
        }
#endif
        if (result.count("help"))
        {
            std::cerr << _options.help({"", "connection", "other"}) << std::endl;
            exit(EXIT_SUCCESS);
        }

        raise_verbose_level(result.count("verbose"));

        if (result.count("version"))
        {
            std::cerr << _app_name << " version " << H9_VERSION << " by SQ8KFH" << std::endl;
            std::cerr << "Copyright (C) 2017-2019 Kamil Palkowski" << std::endl;
            exit(EXIT_SUCCESS);
        }

        return result;
    }
    catch (cxxopts::OptionException& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

std::string ClientCtx::get_h9bus_host() {
    return h9bus_host;
}

std::string ClientCtx::get_h9bus_port() {
    return h9bus_port;
}

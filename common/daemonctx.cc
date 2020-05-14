/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-17.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "daemonctx.h"
#include <cxxopts/cxxopts.hpp>
#include "common/logger.h"


DaemonCtx::DaemonCtx(const std::string& app_name, const std::string& app_desc): Ctx(app_name, app_desc) {
}

void DaemonCtx::load(int argc, char* argv[]) {
    cxxopts::Options options = {_app_name, _app_desc};
    options.add_options("other")
#ifdef H9_DEBUG
            ("d,debug", "Enable debugging")
#endif
            ("h,help", "Show help")
            ("v,verbose", "Be more verbose")
            ("V,version", "Show version")
            ;
    options.add_options("daemon")
            ("c,config", "Config file", cxxopts::value<std::string>()->default_value(H9_CONFIG_PATH + _app_name + ".conf"))
            ("D,daemonize", "Run in the background")
            ("l,logfile", "Log file", cxxopts::value<std::string>())
            ("p,pidfile", "PID file", cxxopts::value<std::string>())
            ;

    cxxopts::ParseResult result = options.parse(argc, argv);
    if (result.count("help")) {
        std::cerr << options.help({"", "daemon", "other"}) << std::endl;
        exit(EXIT_SUCCESS);
    }

    if (result.count("version")) {
        std::cerr << _app_name << " version " << H9_VERSION << " by SQ8KFH." << std::endl;
        std::cerr << "Copyright (C) 2017-2020 Kamil Palkowski. All rights reserved." << std::endl;
        exit(EXIT_SUCCESS);
    }

#ifdef H9_DEBUG
    if (result.count("debug")) {
        enable_debug(true);
    }
#endif
    raise_verbose_level(result.count("verbose"));

    bool override_daemonize = result.count("daemonize") > 0;

    std::string override_logfile;

    if (result.count("logfile")) {
        override_logfile = std::string(result["logfile"].as<std::string>());
    }

    std::string override_pidfile;

    if (result.count("pidfile")) {
        override_pidfile = std::string(result["pidfile"].as<std::string>());
    }

    load_configuration(result["config"].as<std::string>(), override_daemonize, override_logfile, override_pidfile);

    h9_log_notice("Starting %s v%s (config: %s)", _app_name.c_str(), H9_VERSION, result["config"].as<std::string>().c_str());
}

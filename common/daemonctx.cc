/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-17.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "daemonctx.h"

#include <cxxopts/cxxopts.hpp>

DaemonCtx::DaemonCtx(const std::string& app_name, const std::string& app_desc): Ctx(app_name, app_desc) {

}

void DaemonCtx::load_configuration(int argc, char* argv[]) {
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
            ("c,config", "Config file", cxxopts::value<std::string>()->default_value(H9_BUS_CONFIG_FILE))
            ;

    cxxopts::ParseResult result = options.parse(argc, argv);
    if (result.count("help")) {
        std::cerr << options.help({"", "daemon", "other"}) << std::endl;
        exit(EXIT_SUCCESS);
    }

    if (result.count("version")) {
        std::cerr << _app_name << " version " << H9_VERSION << " by SQ8KFH" << std::endl;
        std::cerr << "Copyright (C) 2017-2019 Kamil Palkowski" << std::endl;
        exit(EXIT_SUCCESS);
    }

#ifdef H9_DEBUG
    if (result.count("debug")) {
        enable_debug(true);
    }
#endif
    raise_verbose_level(result.count("verbose"));

    if (result.count("config")) {
        std::cout << "config: " << result["config"].as<std::string>() << std::endl;
    }



}

//T& DaemonCtx<T>::get_cfg() {
//
//}

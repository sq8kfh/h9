/*
* H9 project
*
* Created by crowx on 2023-09-16.
*
* Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#include "h9_configurator.h"
#include <iostream>
#include "git_version.h"

H9Configurator::H9Configurator(const std::string& app_name, const std::string& app_desc): options(app_name, app_desc), _app_name(app_desc), _app_desc(app_desc)  {
    override_connect = "";
    override_port = "";
}

cxxopts::ParseResult H9Configurator::parse_command_line_arg(int argc, char** argv) {
    // clang-format off
    options.add_options("other")
#ifdef H9_DEBUG
            ("d,debug", "Enable debugging")
#endif
            ("h,help", "Show help")
            ("v,verbose", "Be more verbose")
            ("V,version", "Show version")
            ;
    options.add_options("connection")
            ("c,connect", "Connection address", cxxopts::value<std::string>())
            ("p,port", "Connection port", cxxopts::value<int>()->default_value(std::to_string(H9D_DEFAULT_PORT)))
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
            std::cerr << "H9 git commit: " << GITVERSION_COMMIT_SHA << ", working directory " << GITVERSION_COMMIT_SHA << "." << std::endl;
#endif
            std::cerr << "Copyright (C) 2017-2023 Kamil Palkowski. All rights reserved." << std::endl;
            exit(EXIT_SUCCESS);
        }

        verbose = result.count("verbose");

        if (result.count("config")) {
            config_file = std::string(result["config"].as<std::string>());
        }

        parse_app_specific_opt(result);

        return result;
    }
    catch (cxxopts::exceptions::parsing e) {
        std::cerr << e.what() << ". Try '-h' for more information." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void H9Configurator::load_configuration() {

}

H9Connector H9Configurator::get_connector() {
    return H9Connector("127.0.0.1", "7979");
}

std::uint16_t H9Configurator::get_default_source_id() {
    return 13;
}
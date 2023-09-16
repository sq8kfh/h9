/*
* H9 project
*
* Created by crowx on 2023-09-16.
*
* Copyright (C) 2023 Kamil Palkowski. All rights reserved.
*/

#pragma once

#include "h9connector.h"
#include <confuse.h>
#include <cxxopts/cxxopts.hpp>

class H9Configurator {
  private:
    constexpr static char default_config[] = H9_CONFIG_PATH "h9.conf";
    constexpr static char default_user_config[] = H9_CONFIG_PATH "~/.h9";
  protected:
    cfg_t* cfg;
    cxxopts::Options options;

    const std::string _app_name;
    const std::string _app_desc;

    bool debug;
    int verbose;
    std::string override_connect;
    std::string override_port;
    std::string config_file;

    virtual void add_app_specific_opt() {}
    virtual void parse_app_specific_opt(const cxxopts::ParseResult& result) {}
  public:
    H9Configurator(const std::string& app_name, const std::string& app_desc);
    cxxopts::ParseResult parse_command_line_arg(int argc, char** argv);
    void load_configuration();

    H9Connector get_connector();
    std::uint16_t get_default_source_id();
};

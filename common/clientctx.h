/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-17.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_CLIENTCTX_H_
#define _H9_CLIENTCTX_H_

#include "config.h"
#include <string>
#include <cxxopts/cxxopts.hpp>
#include <confuse.h>

#include "ctx.h"


class ClientCtx: public Ctx {
private:
    cfg_t *cfg;
    cxxopts::Options _options;
    cfg_opt_t cfg_opts[2];
    cfg_opt_t cfg_h9bus_opts[3];

    std::string h9bus_host;
    std::string h9bus_port;
public:
    ClientCtx(const std::string& app_name, const std::string& app_desc);
    void load_configuration(const cxxopts::ParseResult& opts);

    void add_options(const std::string& opts,
                     const std::string& desc,
                     std::shared_ptr<const cxxopts::Value> value = cxxopts::value<bool>(),
                     std::string arg_help = "");
    void add_positional_options_list(const std::string& opts, const std::string& desc, const std::string& help);
    void add_positional_option(const std::string& opts, const std::string& desc, const std::string& help);

    cxxopts::ParseResult parse_options(int argc, char* argv[]);

    std::string get_h9bus_host();
    std::string get_h9bus_port();
};


#endif //_H9_CLIENTCTX_H_

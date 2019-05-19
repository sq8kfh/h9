#ifndef _H9_CLIENTCTX_H_
#define _H9_CLIENTCTX_H_

#include "config.h"
#include <string>
#include <cxxopts/cxxopts.hpp>

#include "ctx.h"


class ClientCtx: public Ctx {
private:
    cxxopts::Options _options;
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
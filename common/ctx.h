#ifndef _H9_CTX_H_
#define _H9_CTX_H_

#include <string>

#include <cxxopts/cxxopts.hpp>

#include "config.h"

class Ctx {
private:
    unsigned int _verbose;
#ifdef DEBUG
    bool _debug;
#endif
protected:
    const std::string _app_name;
    cxxopts::Options _options;
    void raise_verbose_level(unsigned int how_much);
public:
    Ctx(const std::string& app_name, const std::string& app_desc);
    inline unsigned int verbose_level();
    inline bool debug_enabled();

    void add_options(const std::string& opts,
                     const std::string& desc,
                     std::shared_ptr<const cxxopts::Value> value = cxxopts::value<bool>(),
                     std::string arg_help = "");
    cxxopts::ParseResult parse_options(int argc, char* argv[]);
};


#endif //_H9_CTX_H_

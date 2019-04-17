#ifndef _H9_CLIENTCTX_H_
#define _H9_CLIENTCTX_H_

#include <string>

#include "ctx.h"

class ClientCtx: public Ctx {
public:
    ClientCtx(const std::string& app_name, const std::string& app_desc);
    void load_configuration(const cxxopts::ParseResult& opts);
};


#endif //_H9_CLIENTCTX_H_

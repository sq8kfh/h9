#ifndef _H9_DAEMONCTX_H_
#define _H9_DAEMONCTX_H_

#include "ctx.h"

class DaemonCtx: public Ctx {
public:
    DaemonCtx(const std::string& app_name, const std::string& app_desc);
    void load_configuration(const cxxopts::ParseResult& opts);
};


#endif //_H9_DAEMONCTX_H_

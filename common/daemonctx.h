/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-17.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_DAEMONCTX_H_
#define _H9_DAEMONCTX_H_

#include "config.h"
#include "ctx.h"


class DaemonCtx: public Ctx {
protected:
    DaemonCtx(const std::string& app_name, const std::string& app_desc);
    virtual void load_configuration(const std::string& conf_filename, bool override_daemonize, const std::string& override_pidfile) = 0;
public:
    void load(int argc, char* argv[]);
};


#endif //_H9_DAEMONCTX_H_

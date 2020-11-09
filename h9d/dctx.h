/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_DCTX_H
#define H9_DCTX_H

#include "config.h"
#include <confuse.h>
#include "common/daemonctx.h"


class DCtx: public DaemonCtx {
private:
    cfg_t *cfg;
    void load_configuration(const std::string& conf_filename, bool override_daemonize,
                            const std::string& override_logfile, const std::string& override_pidfile);
public:
    DCtx();
    ~DCtx();
    /* cfg */
    std::string cfg_pidfile();
    bool cfg_daemonize();
    bool cfg_drop_privileges();
    int cfg_drop_privileges_uid();
    int cfg_drop_privileges_gid();
    uint16_t cfg_server_port();
    std::string cfg_h9bus_hostname();
    uint16_t cfg_h9bus_port();
    std::string cfg_log_logfile();
};


#endif //H9_DCTX_H

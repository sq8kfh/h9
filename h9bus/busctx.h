/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-04-15.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_BUSCTX_H
#define H9_BUSCTX_H

#include "config.h"
#include <vector>
#include <confuse.h>
#include "common/daemonctx.h"


class BusCtx: public DaemonCtx {
private:
    cfg_t *cfg;
    void load_configuration(const std::string& conf_filename, bool override_daemonize,
            const std::string& override_logfile, const std::string& override_pidfile);
public:
    BusCtx();
    ~BusCtx();
    /* cfg */
    std::string cfg_pidfile();
    bool cfg_daemonize();
    bool cfg_drop_privileges();
    int cfg_drop_privileges_uid();
    int cfg_drop_privileges_gid();
    uint16_t cfg_server_port();
    std::string cfg_log_logfile();
    std::string cfg_log_send_logfile();
    std::string cfg_log_recv_logfile();
    std::vector<std::string> cfg_bus_list();
    std::string cfg_bus_driver(const std::string &bus);
    std::string cfg_bus_connection_string(const std::string &bus);
};


#endif //H9_BUSCTX_H

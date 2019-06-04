/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-01.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "config.h"
#include <cstdlib>
#include <unistd.h>

#include "common/clientctx.h"
#include "protocol/h9connector.h"
#include "protocol/methodcallmsg.h"
#include "protocol/methodresponsemsg.h"

int main(int argc, char **argv)
{
    ClientCtx ctx = ClientCtx("h9stat", "Show H9 statistic.");

    ctx.add_options("i,interval", "Refresh interval in a seconds", cxxopts::value<int>()->default_value("15"));

    auto res = ctx.parse_options(argc, argv);
    ctx.load_configuration(res);

    int interval = res["interval"].as<int>();

    H9Connector h9_connector = {ctx.get_h9bus_host(), ctx.get_h9bus_port()};

    if (h9_connector.connect() == -1) {
        return EXIT_FAILURE;
    }

    while (true) {
        h9_connector.send(MethodCallMsg("get_h9bus_stat"));

        GenericMsg raw_msg = h9_connector.recv();
        if (raw_msg.get_type() == GenericMsg::Type::METHODRESPONSE) {
            MethodResponseMsg msg = std::move(raw_msg);
            std::cout << msg.serialize() << std::endl;
        }

        sleep(interval);
    }

    return EXIT_SUCCESS;
}

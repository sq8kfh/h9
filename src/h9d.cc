/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-09.
 *
 * Copyright (C) 2019-2023 Kamil Palkowski. All rights reserved.
 */

#include "config.h"

#include <cstdlib>
#include <cstring>
#include <jsonrpcpp/jsonrpcpp.hpp>
#include <spdlog/cfg/helpers.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <thread>
#include <unistd.h>

#include "api.h"
#include "bus.h"
#include "git_version.h"
#include "h9d_configurator.h"
#include "tcpserver.h"
#include "metrics_collector.h"

int main(int argc, char** argv) {
    H9dConfigurator configurator;

    configurator.parse_command_line_arg(argc, argv);
    configurator.logger_initial_setup();

    SPDLOG_WARN("Starting h9d... Version: {}.", H9_VERSION);
#ifdef GITVERSION_COMMIT_SHA
#ifdef GITVERSION_DIRTY
    constexpr char workdir[] = "dirty";
#else
    constexpr char workdir[] = "clean";
#endif
    SPDLOG_WARN("H9 git commit: {}, working directory {}.", GITVERSION_COMMIT_SHA, workdir);
#endif

    configurator.load_configuration();
    configurator.logger_setup();
    configurator.daemonize();
    configurator.save_pid();
    configurator.drop_privileges();

    //MetricsCollector mc;
    MetricsCollector::counter_t& test_cnt = MetricsCollector::make_counter("test");
    test_cnt++;
    ++test_cnt;
    test_cnt += 10;
    SPDLOG_WARN("test: {}", MetricsCollector::metrics_to_json().dump());

    Bus bus;
    configurator.configure_bus(&bus);
    bus.activate();

    API api(&bus);
    SPDLOG_WARN("test: {}", MetricsCollector::metrics_to_json().dump());
    TCPServer server(&api);
    configurator.configure_tcpserver(&server);
    server.run();

    jsonrpcpp::Parser parser;
    jsonrpcpp::entity_ptr entity = parser.parse(R"({"jsonrpc": "2.0", "method": "sum", "params": [1, 2, 3, 4, 5], "id": 1})");
    entity->to_json();
    jsonrpcpp::entity_ptr y;

    //    std::cout << entity->is_request() << std::endl;

    /* while (1) {
         sleep(1);
         // bus.send();
         // sleep(5);
         ExtH9Frame h9frame;
         h9frame.type(H9frame::Type::DISCOVER);
         h9frame.destination_id(H9frame::BROADCAST_ID);
         h9frame.source_id(3);
         h9frame.dlc(0);
         // h9frame.data({1,3,4});

     }*/

    return EXIT_FAILURE;
}

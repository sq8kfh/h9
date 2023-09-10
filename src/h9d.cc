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
#include <spdlog/cfg/helpers.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <thread>
#include <unistd.h>

#include "bus.h"
#include "git_version.h"
#include "h9d_configurator.h"

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

    Bus bus;

    configurator.configure_bus(&bus);

    bus.activate();

    while (1) {
        sleep(1);
        // bus.send();
        // sleep(5);
        ExtH9Frame h9frame;
        h9frame.type(H9frame::Type::DISCOVER);
        h9frame.destination_id(H9frame::BROADCAST_ID);
        h9frame.source_id(3);
        h9frame.dlc(0);
        // h9frame.data({1,3,4});

        // auto frame = BusFrame(h9frame, "tcp", 0, 0);
        // loop.send_frame(frame);
        // std::cout << "Sending...\n";
        bus.send_frame_noblock(h9frame);
        // std::cout << "Send all\n";
    }
    while (1)
        ;
    ;
    /*loop.open();

    H9frame h9frame = H9frame();
    h9frame.type = H9frame::Type::NODE_HEARTBEAT;

    auto frame = BusFrame(h9frame, "tcp", 0 , 0);
    loop.send_frame(frame);
    BusFrame buf;
    loop.recv_frame(&buf);

    std::cout << buf.get_frame() << std::endl;*/

    return EXIT_SUCCESS;
}

/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-09.
 *
 * Copyright (C) 2019-2023 Kamil Palkowski. All rights reserved.
 */

#include "config.h"
#include <cstdlib>
#include <cxxopts/cxxopts.hpp>

#include <cstring>
#include <thread>
#include <unistd.h>

#include "bus.h"
#include "loop_driver.h"
#include "socketcan_driver.h"

#include "epoll.h"

#include "frameobserver.h"

class test: public FrameObserver {
public:
    test(FrameSubject *s): FrameObserver(s, H9FrameComparator()) {};

    void on_frame_recv(ExtH9Frame frame) {
        std::cout << frame.frame() << std::endl;
    }
};

/*void parse_arg(int argc, char **argv) {
    cxxopts::Options options = {"h9d", "H9 daemon."};
    options.add_options("other")
#ifdef H9_DEBUG
            ("d,debug", "Enable debugging")
#endif
            ("h,help", "Show help")
            ("v,verbose", "Be more verbose")
            ("V,version", "Show version")
            ;
    options.add_options("daemon")
            ("c,config", "Config file", cxxopts::value<std::string>()->default_value(H9_CONFIG_PATH + _app_name + ".conf"))
            ("D,daemonize", "Run in the background")
            ("l,logfile", "Log file", cxxopts::value<std::string>())
            ("p,pidfile", "PID file", cxxopts::value<std::string>())
            ;

    cxxopts::ParseResult result = options.parse(argc, argv);
    if (result.count("help")) {
        std::cerr << options.help({"", "daemon", "other"}) << std::endl;
        exit(EXIT_SUCCESS);
    }

    if (result.count("version")) {
        std::cerr << "h9d version " << H9_VERSION << " by crowx." << std::endl;
        std::cerr << "Copyright (C) 2017-2023 Kamil Palkowski. All rights reserved." << std::endl;
        exit(EXIT_SUCCESS);
    }
}*/

int main(int argc, char **argv) {
    LoopDriver loop0 = LoopDriver("loop0");
    LoopDriver loop1 = LoopDriver("loop1");
    SocketCANDriver can0 = SocketCANDriver("can0", "can0");

    Bus<Epoll> bus;
    //bus.add_driver(&can0);
    bus.add_driver(&loop1);

    bus.activate();

    test t{&bus};

    while(1) {
        sleep(2);
        //bus.send();
        //sleep(5);
        ExtH9Frame h9frame;
        h9frame.type(H9frame::Type::DISCOVER);
        h9frame.destination_id(H9frame::BROADCAST_ID);
	h9frame.source_id(3);
	h9frame.dlc(0);
        //auto frame = BusFrame(h9frame, "tcp", 0, 0);
        //loop.send_frame(frame);
        std::cout << "Sending...\n";
        bus.send_frame_noblock(h9frame);
        std::cout << "Send all\n";
    }
    while(1);
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

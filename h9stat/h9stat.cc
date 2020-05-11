/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-01.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "config.h"
#include <cstdlib>
#include <iomanip>
#include <unistd.h>

#include "common/clientctx.h"
#include "protocol/h9connector.h"
#include "protocol/callmsg.h"
#include "protocol/responsemsg.h"

void print_uptime(int n) {
    int day = n / (24 * 3600);

    n = n % (24 * 3600);
    int hour = n / 3600;

    n %= 3600;
    int minutes = n / 60 ;

    n %= 60;
    int seconds = n;

    if (day) std::cout << day << " " << "days ";
    if (hour) std::cout << hour << " " << "hours ";
    if (minutes) std::cout << minutes << " " << "minutes ";
    std::cout << seconds << " " << "seconds";
}

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

    int last_uptime_value = 0;
    int last_frames_sent = 0;
    int last_frames_received = 0;

    while (true) {
        h9_connector.send(CallMsg("h9bus_stat"));

        GenericMsg raw_msg = h9_connector.recv();
        if (raw_msg.get_type() == GenericMsg::Type::RESPONSE) {
            ResponseMsg msg = std::move(raw_msg);

            Value result = msg.result();

            std::system("clear");
            //std::cout << "\x1b\x5b\x48\x1b\x5b\x32\x4a"; //Esc[2J

            std::cout << "h9bus v" << result["version"].get_value_as_str() << " uptime: ";
            int uptime = result["uptime"].get_value_as_int();
            print_uptime(uptime);
            std::cout << std::endl;

            std::cout << " connected clients: " << result["connected_clients_count"].get_value_as_int() << std::endl;

            for (Value endpoint: result["endpoint"]) {
                std::cout << " Endpoint: " << endpoint.get_name() << std::endl;
                int frames_sent = endpoint["send_frames_counter"].get_value_as_int();
                float fps = frames_sent - last_frames_sent;
                fps /= uptime - last_uptime_value;
                std::cout << "  Frames sent: " << frames_sent << " (" << std::fixed <<  std::setprecision(2) << fps << " f/s)" << std::endl;
                int frames_received = endpoint["received_frames_counter"].get_value_as_int();
                fps = frames_received - last_frames_received;
                fps /= uptime - last_uptime_value;
                std::cout << "  Frames received: " << frames_received << " (" << std::fixed << std::setprecision(2) << fps << " f/s)" << std::endl;

                last_frames_sent = frames_sent;
                last_frames_received = frames_received;
            }

            if (ctx.cfg_debug())
                std::cout << "\n\n" << msg.serialize() << std::endl;

            last_uptime_value = uptime;
        }
        else {
            if (ctx.cfg_debug())
                std::cerr << raw_msg.serialize() << std::endl;
            std::cerr << "Receive unexpected message (type: " << static_cast<int>(raw_msg.get_type()) << ")\n";
            return EXIT_FAILURE;
        }
        sleep(interval);
    }
    return EXIT_SUCCESS;
}

#include <cstdlib>

#include "protocol/h9connector.h"
#include "protocol/sendframemsg.h"
#include "common/clientctx.h"
#include "bus/h9frame.h"

int main(int argc, char* argv[]) {
    ClientCtx ctx = ClientCtx("h9send", "Sends data to the H9 Bus.");

    ctx.add_options("l,local", "some local option", cxxopts::value<int>());

    auto res = ctx.parse_options(argc, argv);

    ctx.load_configuration(res);

    if (res.count("local")) {
        std::cout << res["local"].as<int>();
    }

    H9Connector h9_connector = {"127.0.0.1", "7878"};
    h9_connector.connect();
    H9frame frame;
    frame.source_id = 12;
    frame.destination_id = H9frame::BROADCAST_ID;

    frame.dlc = 2;
    frame.data[0] = 21;
    frame.data[1] = 212;

    std::cout << SendFrameMsg(frame).serialize() << std::endl;

    h9_connector.send(SendFrameMsg(frame));

    return EXIT_SUCCESS;
}

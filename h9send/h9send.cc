#include "config.h"
#include <cstdlib>

#include "protocol/h9connector.h"
#include "protocol/sendframemsg.h"
#include "common/clientctx.h"
#include "bus/h9frame.h"

int main(int argc, char* argv[]) {
    ClientCtx ctx = ClientCtx("h9send", "Sends frames to the H9 Bus.");

    ctx.add_options("s,src_id", "Source id", cxxopts::value<std::uint16_t>());
    ctx.add_options("i,dst_id", "Destination id", cxxopts::value<std::uint16_t>());
    ctx.add_options("H,high_priority", "High priority");
    ctx.add_options("t,type", "Frame type", cxxopts::value<std::underlying_type_t<H9frame::Type >>());
    ctx.add_positional_options_list("data", "[hex data]", "");

    auto res = ctx.parse_options(argc, argv);

    ctx.load_configuration(res);

    H9frame frame;

    if (res.count("src_id")) {
        frame.source_id = res["src_id"].as<std::uint16_t>();
    }

    if (res.count("dst_id")) {
        frame.destination_id = res["dst_id"].as<std::uint16_t>();
    }

    if (res.count("high_priority")) {
        frame.priority = H9frame::Priority::HIGH;
    }
    else {
        frame.priority = H9frame::Priority::LOW;
    }

    if (res.count("type")) {
        frame.type = static_cast<H9frame::Type>(res["type"].as<std::underlying_type_t<H9frame::Type >>());
    }

    frame.dlc = 0;
    if (res.count("data")) {
        auto& v = res["data"].as<std::vector<std::string>>();
        for (const auto& s : v) {
            frame.data[frame.dlc] = std::stoul(s, nullptr, 16);
            ++frame.dlc;
            if (frame.dlc == 8)
                break;
        }
    }

    H9Connector h9_connector = {ctx.get_h9bus_host(), ctx.get_h9bus_port()};
    h9_connector.connect();

    std::cout << SendFrameMsg(frame).serialize() << std::endl;

    h9_connector.send(SendFrameMsg(frame));

    return EXIT_SUCCESS;
}

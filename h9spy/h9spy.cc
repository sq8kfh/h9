/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-09.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "config.h"
#include <iomanip>
#include <cstdlib>

#include "protocol/h9connector.h"
#include "protocol/subscribemsg.h"
#include "protocol/framereceivedmsg.h"
#include "common/clientctx.h"

int main(int argc, char **argv)
{
    ClientCtx ctx = ClientCtx("h9spy", "The H9 bus packets sniffer.");

    ctx.add_options("e,extended", "Extended output");
    ctx.add_options("s,simple", "Simple output");

    auto res = ctx.parse_options(argc, argv);
    ctx.load_configuration(res);

    H9Connector h9_connector = {ctx.get_h9bus_host(), ctx.get_h9bus_port()};

    if (h9_connector.connect() == -1) {
        return EXIT_FAILURE;
    }

    h9_connector.send(SubscribeMsg(SubscribeMsg::Content::FRAME));

    int output = 1;
    if (res.count("simple") == 1 && res.count("extended") ==0) {
        output = 0;
        std::cout << "source_id,destination_id,priority,type,seqnum,dlc,data\n";
    }
    if (res.count("extended") == 1 && res.count("simple") ==0) {
        output = 2;
    }

    while (true) {
        GenericMsg raw_msg = h9_connector.recv();
        if (raw_msg.get_type() == GenericMsg::Type::FRAME_RECEIVED) {
            FrameReceivedMsg msg = std::move(raw_msg);

            H9frame frame = msg.get_frame();

            if (output == 0) {
                std::cout << frame.source_id << ","
                          << frame.destination_id << ","
                          << (frame.priority == H9frame::Priority::HIGH ? 'H' : 'L') << ","
                          << static_cast<unsigned int>(H9frame::to_underlying(frame.type)) << ','
                          << static_cast<unsigned int>(frame.seqnum) << ","
                          << static_cast<unsigned int>(frame.dlc) << ",";
                std::ios oldState(nullptr);
                oldState.copyfmt(std::cout);

                for (int i = 0; i < frame.dlc; ++i) {
                    std::cout << std::setfill('0') << std::hex << std::setw(2) << static_cast<unsigned int>(frame.data[i]);
                }
                std::cout.copyfmt(oldState);
                std::cout << std::endl;
            }
            else {
                std::cout << frame << std::endl;
                if (output == 2) {
                    if (frame.destination_id == H9frame::BROADCAST_ID)
                        std::cout << "    destination: BROADCAST\n";
                    std::cout << "    type name: " << H9frame::type_to_string(frame.type) << std::endl;
                }
            }
        }
        else {
            h9_log_info(" recv msg: [%s]", raw_msg.serialize().c_str());
        }
    }
    return EXIT_SUCCESS;
}

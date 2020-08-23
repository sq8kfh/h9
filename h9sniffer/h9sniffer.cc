/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-09.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "config.h"
#include <iomanip>
#include <cstdlib>

#include "protocol/h9connector.h"
#include "protocol/subscribemsg.h"
#include "protocol/framemsg.h"
#include "common/clientctx.h"

void print_reg_value(const H9frame& frame) {
    if (frame.dlc > 1) {
        std::cout << "    value: ";
        switch (frame.dlc) {
            case 2:
                std::cout << static_cast<int>(frame.data[1]);
                break;
            case 3:
                std::cout << static_cast<int>((frame.data[1] << 8) | frame.data[2]);
                break;
            case 5:
                std::cout << static_cast<int>((frame.data[1] << 24) | (frame.data[2] << 16) | (frame.data[3] << 8) | frame.data[4]);
                break;
        }
        char buf[8] = {'\0'};
        for (int i = frame.dlc - 1; i > 0; --i) {
            if (isprint(frame.data[i])) {
                buf[i-1] = frame.data[i];
            }
            else {
                break;
            }
        }
        std::cout << " " << buf << std::endl;
    }
}

void print_frame(const H9frame& frame) {
    std::cout << "    type name: " << H9frame::type_to_string(frame.type) << std::endl;
    if (frame.destination_id == H9frame::BROADCAST_ID)
        std::cout << "    destination: BROADCAST\n";
    if (frame.type == H9frame::Type::REG_EXTERNALLY_CHANGED ||
        frame.type == H9frame::Type::REG_INTERNALLY_CHANGED ||
        frame.type == H9frame::Type::REG_VALUE_BROADCAST ||
        frame.type == H9frame::Type::REG_VALUE ||
        frame.type == H9frame::Type::SET_REG ||
        frame.type == H9frame::Type::GET_REG) {

        std::cout << "    reg: " << static_cast<unsigned int>(frame.data[0]) << std::endl;
        print_reg_value(frame);
    }
    else if (frame.type == H9frame::Type::BULK_DATA || frame.type == H9frame::Type::GET_BULK_DATA) {
        std::cout << "    bulk msg type: " << static_cast<unsigned int>(frame.data[0]) << std::endl;
    }
    else if (frame.type == H9frame::Type::SET_BIT) {
        std::cout << "    reg: " << static_cast<unsigned int>(frame.data[0]) << std::endl;
        std::cout << "    set bit: " << static_cast<unsigned int>(frame.data[1]) << std::endl;
    }
    else if (frame.type == H9frame::Type::CLEAR_BIT) {
        std::cout << "    reg: " << static_cast<unsigned int>(frame.data[0]) << std::endl;
        std::cout << "    clear bit: " << static_cast<unsigned int>(frame.data[1]) << std::endl;
    }
    else if (frame.type == H9frame::Type::TOGGLE_BIT) {
        std::cout << "    reg: " << static_cast<unsigned int>(frame.data[0]) << std::endl;
        std::cout << "    toggle bit: " << static_cast<unsigned int>(frame.data[1]) << std::endl;
    }
    else if (frame.type == H9frame::Type::NODE_TURNED_ON) {
        std::cout << "    node type: " << static_cast<unsigned int>(frame.data[0] << 8 | frame.data[1]) << std::endl;
        std::cout << "    node firmware: " << static_cast<unsigned int>(frame.data[2]) << '.' << static_cast<unsigned int>(frame.data[3]) << std::endl;
    }
    else if (frame.type == H9frame::Type::BOOTLOADER_TURNED_ON) {
        std::cout << "    bootloader version: " << static_cast<unsigned int>(frame.data[0]) << '.' << static_cast<unsigned int>(frame.data[1]) << std::endl;
        std::cout << "    node MCU type: " << static_cast<unsigned int>(frame.data[2]) << std::endl;
        std::cout << "    node type: " << static_cast<unsigned int>(frame.data[3] << 8 | frame.data[4]) << std::endl;
    }
    else if (frame.type == H9frame::Type::ERROR) {
        int err_num = static_cast<int>(frame.data[0]);
        std::cout << "    error: " << err_num << " - " << H9frame::error_to_string(H9frame::from_underlying<H9frame::Error>(err_num)) << std::endl;
    }
}

int main(int argc, char **argv)
{
    ClientCtx ctx = ClientCtx("h9sniffer", "The H9 bus packets sniffer.");

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
        if (raw_msg.get_type() == GenericMsg::Type::FRAME) {
            FrameMsg msg = std::move(raw_msg);

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
                    print_frame(frame);
                }
            }
        }
        else {
            h9_log_info(" recv msg: [%s]", raw_msg.serialize().c_str());
        }
    }
    return EXIT_SUCCESS;
}

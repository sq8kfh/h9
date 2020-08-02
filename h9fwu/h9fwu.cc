/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-09.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "config.h"
#include <cstdlib>
#include <cstdio>

#include "protocol/h9connector.h"
#include "protocol/framemsg.h"
#include "protocol/sendframemsg.h"
#include "protocol/subscribemsg.h"
#include "common/clientctx.h"

char* mcu_map[] = {(char*)"UNKNOWN",       // 0
                   (char*)"ATmega16M1",    // 1
                   (char*)"ATmega32M1",    // 2
                   (char*)"ATmega64M1",    // 3
                   (char*)"AT90CAN128",    // 4
                   (char*)"PIC18F46K80"};  // 5

std::uint8_t hex2bin(const char *hex) {
    std::uint8_t val = 0;
    for (size_t n = 0; n < 2; n++) {
        char c = hex[n];
        if (c == '\0')
            break;

        if (c >= '0' && c <= '9')
            c = c - '0';
        else if (c >= 'a' && c <='f')
            c = c - 'a' + 10;
        else if (c >= 'A' && c <='F')
            c = c - 'A' + 10;

        val = (val << 4) | (c & 0xf);
    }

    return val;
}

static size_t read_ihex(const std::string& ihex_file, std::uint8_t **fw_data) {
    *fw_data = static_cast<uint8_t *>(std::malloc(128 * 1024));
    memset(*fw_data, 0xff, 128*1024);

    uint32_t extended_address = 0;
    size_t data_length = 0;

    //size_t ret = 0;
    FILE *fp = std::fopen(ihex_file.c_str(), "r");
    if (fp == nullptr) {
        h9_log_stderr("Failed to open file: %s", ihex_file.c_str());
        exit(EXIT_FAILURE);
    }

    char *line = nullptr;
    size_t linecap = 0;

    while (getline(&line, &linecap, fp) > 0) {
        if (line[0] != ':')
            continue;
        std::uint8_t tmp_crc = 0;

        std::uint8_t size = hex2bin(line + 1); //byte count
        tmp_crc += size;
        //TODO: fw_data size
        std::uint32_t address = extended_address;
        std::uint8_t tmp = hex2bin(line + 3); //address MSB
        tmp_crc += tmp;
        address += (tmp << 8);

        tmp = hex2bin(line + 5); //address LSB
        tmp_crc += tmp;
        address += tmp;

        std::uint8_t rtype = hex2bin(line + 7); //record type
        tmp_crc += rtype;
        if (rtype == 0) {
            if (data_length < address) data_length = address;

            size_t n = 0;
            for (; n < size; n++) {
                std::uint8_t tmp = hex2bin(line + 9 + n*2);
                (*fw_data)[data_length++] = tmp;
                tmp_crc += tmp;
            }
            std::uint8_t crc = hex2bin(line + 9 + n*2);

            if (static_cast<std::uint8_t>(0x100 - tmp_crc) != crc) {
                h9_log_stderr("Intel HEX file corrupted");
                data_length = 0;
                break;
            }
        }
        else if (rtype == 4) {
            std::uint8_t tmp = hex2bin(line + 9 + 0*2);
            tmp_crc += tmp;
            extended_address = tmp;
            extended_address <<= 8;

            tmp = hex2bin(line + 9 + 1*2);
            tmp_crc += tmp;
            extended_address += tmp;
            extended_address <<= 16;

            std::uint8_t crc = hex2bin(line + 9 + 2*2);

            if (static_cast<std::uint8_t>(0x100 - tmp_crc) != crc) {
                h9_log_stderr("Intel HEX file corrupted");
                data_length = 0;
                break;
            }

            if (extended_address > (1<<16)) { //more then 128kB
                break; //skip reading EEPROM, CONFIG e.t.c.
            }
        }
    }
    fclose(fp);
    if (line) {
        free(line);
    }
    return data_length;
}

int main(int argc, char **argv)
{
    ClientCtx ctx = ClientCtx("h9fwu", "Uploads a firmware to devices via the H9 bus.");

    ctx.add_options("s,src_id", "Source id", cxxopts::value<std::uint16_t>());
    ctx.add_options("i,dst_id", "Destination id", cxxopts::value<std::uint16_t>());
    ctx.add_options("b,noupgrademsg", "Don't send the NODE_UPGRADE frame before");
    ctx.add_positional_option("ihex", "<Intel HEX file>", "");

    auto res = ctx.parse_options(argc, argv);
    ctx.load_configuration(res);


    H9frame frame;
    std::uint8_t *fw = nullptr;
    size_t fw_size = 0;

    if (res.count("src_id")) {
        frame.source_id = res["src_id"].as<std::uint16_t>();
    }
    else {
        frame.source_id = ctx.get_default_source_id();
        if (frame.source_id == 0 || frame.source_id == H9frame::BROADCAST_ID) {
            h9_log_stderr("source id must be set");
            return EXIT_FAILURE;
        }
    }

    if (res.count("dst_id")) {
        frame.destination_id = res["dst_id"].as<std::uint16_t>();
    }
    else {
        h9_log_stderr("destination id must be set");
        return EXIT_FAILURE;
    }

    if (res.count("ihex")) {
        fw_size = read_ihex(res["ihex"].as<std::string>(), &fw);
        std::cout << "Loaded firmware: " << res["ihex"].as<std::string>() << " (" << fw_size << " B)\n";
    }
    else {
        h9_log_stderr("missing a Intel HEX file");
        return EXIT_FAILURE;
    }

    frame.priority = H9frame::Priority::HIGH;

    H9Connector h9_connector = {ctx.get_h9bus_host(), ctx.get_h9bus_port()};

    if (h9_connector.connect() == -1) {
        return EXIT_FAILURE;
    }

    h9_connector.send(SubscribeMsg(SubscribeMsg::Content::FRAME));

    uint8_t seqnum = 0;
    uint16_t page = 0;

    if (res.count("noupgrademsg") > 0) { //skip NODE_UPGRADE frame
        frame.type = H9frame::Type::PAGE_START;
        frame.dlc = 2;
        frame.seqnum = seqnum++;
        frame.data[0] = (uint8_t)((page >> 8) & 0xff);
        frame.data[1] = (uint8_t)((page) & 0xff);

        h9_connector.send(SendFrameMsg(frame));
    }
//    else {
//        frame.type = H9frame::Type::NODE_UPGRADE;
//        frame.dlc = 0;
//        frame.seqnum = seqnum++;
//
//        h9_connector.send(SendFrameMsg(frame));
//    }

    size_t fw_idx = 0;

    std::cout << "Waiting for node...\n";

    while (true) {
        GenericMsg raw_msg = h9_connector.recv();
        if (raw_msg.get_type() == GenericMsg::Type::FRAME) {
            FrameMsg msg = std::move(raw_msg);
            H9frame recv_frame = msg.get_frame();

            if (recv_frame.source_id != frame.destination_id) {
                continue;
            }

            if (recv_frame.type == H9frame::Type::BOOTLOADER_TURNED_ON || recv_frame.type == H9frame::Type::PAGE_WRITED) {
                if (recv_frame.type == H9frame::Type::PAGE_WRITED) {
                    constexpr int PBWIDTH = 60;
                    constexpr char* PBSTR = (char*)"============================================================";

                    unsigned int val = fw_idx * 100 / fw_size;
                    unsigned int lpad = fw_idx * PBWIDTH / fw_size;
                    unsigned int rpad = PBWIDTH - lpad;
                    printf("%3u%% [%.*s>%*s]\r", val, lpad, PBSTR, rpad, "");
                    fflush(stdout);

                    if (fw_idx >= fw_size) {
                        frame.type = H9frame::Type::QUIT_BOOTLOADER;
                        frame.dlc = 0;
                        frame.seqnum = seqnum++;

                        h9_connector.send(SendFrameMsg(frame));
                        printf("\nDone.\n");
                        exit(EXIT_SUCCESS);
                    }
                    page++;
                }
                else if (recv_frame.type == H9frame::Type::BOOTLOADER_TURNED_ON && recv_frame.dlc == 5) {
                    std::uint8_t bootloader_version_major = recv_frame.data[0];
                    std::uint8_t bootloader_version_minor = recv_frame.data[1];
                    std::uint8_t node_cpu = recv_frame.data[2];
                    std::uint16_t node_type = recv_frame.data[3];
                    node_type <<= 8;
                    node_type += recv_frame.data[4];
                    char *mcu = mcu_map[ node_cpu < (sizeof(mcu_map)/sizeof(char*)) ? node_cpu : 0 ];

                    printf("Bootloader version: %hhu.%hhu\n", bootloader_version_major, bootloader_version_minor);
                    printf("Target node id: %hu\n", recv_frame.source_id);
                    printf("Target node MCU: %s (%hhu)\n", mcu, node_cpu);
                    printf("Target node type: %hu\n", node_type);
                }

                frame.type = H9frame::Type::PAGE_START;
                frame.dlc = 2;
                frame.seqnum = seqnum++;
                frame.data[0] = (uint8_t)((page >> 8) & 0xff);
                frame.data[1] = (uint8_t)((page) & 0xff);

                h9_connector.send(SendFrameMsg(frame));
            }
            else if (recv_frame.type == H9frame::Type::PAGE_FILL_NEXT) {
                frame.type = H9frame::Type::PAGE_FILL;
                frame.dlc = 8;
                frame.seqnum = seqnum++;
                frame.data[0] = fw[fw_idx++];
                frame.data[1] = fw[fw_idx++];
                frame.data[2] = fw[fw_idx++];
                frame.data[3] = fw[fw_idx++];
                frame.data[4] = fw[fw_idx++];
                frame.data[5] = fw[fw_idx++];
                frame.data[6] = fw[fw_idx++];
                frame.data[7] = fw[fw_idx++];

                h9_connector.send(SendFrameMsg(frame));
            }
        }
    }

    return EXIT_SUCCESS;
}

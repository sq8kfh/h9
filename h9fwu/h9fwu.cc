#include "config.h"
#include <cstdlib>
#include <cstdio>

#include "protocol/h9connector.h"
#include "protocol/framereceivedmsg.h"
#include "protocol/sendframemsg.h"
#include "protocol/subscribemsg.h"
#include "common/clientctx.h"

std::uint32_t hex2bin(const char *hex, const size_t size) {
    std::uint32_t val = 0;
    const size_t _size = size * 2;
    for (size_t n = 0; n < _size; n++) {
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
    bzero(*fw_data, 128*1024);

    size_t ret = 0;
    FILE *fp = std::fopen(ihex_file.c_str(), "r");
    if (fp == nullptr) {
        h9_log_stderr("Failed to open file: %s", ihex_file.c_str());
        exit(EXIT_FAILURE);
    }

    char *line = nullptr;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) > 0) {
        if (line[0] != ':')
            continue;
        std::uint8_t tmp_crc = 0;

        std::uint8_t size = hex2bin(line + 1, 1); //byte count
        tmp_crc += size;
        //TODO: fw_data size
        tmp_crc += hex2bin(line + 3, 1); //address MSB
        tmp_crc += hex2bin(line + 5, 1); //address LSB
        std::uint8_t rtype = hex2bin(line + 7, 1); //record type
        tmp_crc += rtype;
        if (rtype == 0) {
            size_t n = 0;
            for (; n < size; n++) {
                std::uint8_t tmp = hex2bin(line + 9 + n*2, 1);
                (*fw_data)[ret++] = tmp;
                tmp_crc += tmp;
            }
            std::uint8_t crc = hex2bin(line + 9 + n*2, 1);

            if (static_cast<std::uint8_t>(0x100 - tmp_crc) != crc) {
                h9_log_stderr("Intel HEX file corrupted");
                ret = 0;
                break;
            }
        }
    }
    fclose(fp);
    if (line) {
        free(line);
    }
    return ret;
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
        return EXIT_FAILURE;
    }

    if (res.count("dst_id")) {
        frame.destination_id = res["dst_id"].as<std::uint16_t>();
        std::cout << "Target node id: " << frame.destination_id << std::endl;
    }
    else {
        return EXIT_FAILURE;
    }

    if (res.count("ihex")) {
        fw_size = read_ihex(res["ihex"].as<std::string>(), &fw);
        std::cout << "Loaded: " << res["ihex"].as<std::string>() << " (data size: " << fw_size << ")\n";
    }
    else {
        return EXIT_FAILURE;
    }

    frame.priority = H9frame::Priority::HIGH;

    H9Connector h9_connector = {ctx.get_h9bus_host(), ctx.get_h9bus_port()};
    h9_connector.connect();


    h9_connector.send(SubscribeMsg(SubscribeMsg::Content::FRAME));

    if (res.count("noupgrademsg") > 0) { //skip NODE_UPGRADE frame
        frame.type = H9frame::Type::PAGE_START;
        frame.dlc = 2;
        frame.data[0] = 0;
        frame.data[1] = 0;

        h9_connector.send(SendFrameMsg(frame));
    }
    else {
        frame.type = H9frame::Type::NODE_UPGRADE;
        frame.dlc = 0;

        h9_connector.send(SendFrameMsg(frame));
    }

    size_t fw_idx = 0;
    uint16_t page = 0;

    while (true) {
        GenericMsg raw_msg = h9_connector.recv();
        if (raw_msg.get_type() == GenericMsg::Type::FRAME_RECEIVED) {
            FrameReceivedMsg msg = std::move(raw_msg);
            H9frame recv_frame = msg.get_frame();
            //std::cout << recv_frame <<std::endl;

            if (recv_frame.type == H9frame::Type::ENTER_INTO_BOOTLOADER || recv_frame.type == H9frame::Type::PAGE_WRITED) {
                if (recv_frame.type == H9frame::Type::PAGE_WRITED) {
                    h9_log_stderr("Writted page %hu, byte %u", page, fw_idx);
                    if (fw_idx >= fw_size) {
                        frame.type = H9frame::Type::QUIT_BOOTLOADER;
                        frame.dlc = 0;

                        h9_connector.send(SendFrameMsg(frame));
                        exit(EXIT_SUCCESS);
                    }
                    page++;
                }
                frame.type = H9frame::Type::PAGE_START;
                frame.dlc = 2;
                frame.data[0] = (uint8_t)((page >> 8) & 0xff);
                frame.data[1] = (uint8_t)((page) & 0xff);

                h9_connector.send(SendFrameMsg(frame));
            }
            else if (recv_frame.type == H9frame::Type::PAGE_FILL_NEXT) {
                frame.type = H9frame::Type::PAGE_FILL;
                frame.dlc = 8;
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

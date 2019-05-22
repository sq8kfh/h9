/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-28.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "slcan.h"

#include <system_error>
#include <sstream>
#include <iomanip>
//#include <iostream>

#include <fcntl.h>
#include <termios.h>

Slcan::Slcan(BusMgr::EventCallback event_callback, const std::string& tty):
        Driver(std::move(event_callback)),
        _tty(tty) {
    noblock = false;
    last_send = nullptr;
}

void Slcan::open() {
    int fd = ::open(_tty.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    else {
        if (noblock)
            fcntl(fd, F_SETFL, O_NONBLOCK);
        else
            fcntl(fd, F_SETFL, 0);
    }

    struct termios options;

    tcgetattr(fd, &options);

    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    options.c_cflag |= (CLOCAL | CREAD); /* Enable the receiver and set local mode */
    /*
     * Select 8N1
     */
    options.c_iflag &= ~IGNBRK;         // disable break processing

    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    options.c_cflag &= ~CRTSCTS; /* Disable hardware flow control */
    options.c_iflag &= ~(IXON | IXOFF | IXANY); /* Disable software flow control */

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* Raw Input */

    options.c_iflag = 0;
    options.c_lflag = 0;
    options.c_oflag = 0; /* Raw Output */

    options.c_cc[VMIN]  = noblock ? 0 : 1;
    options.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    set_socket(fd, true);
}

std::string Slcan::build_slcan_msg(const H9frame& frame) {
    uint32_t id = 0;
    id |= H9frame::to_underlying(frame.priority) & ((1<<H9frame::H9FRAME_PRIORITY_BIT_LENGTH) - 1);
    id <<= H9frame::H9FRAME_TYPE_BIT_LENGTH;
    id |= H9frame::to_underlying(frame.type) & ((1<<H9frame::H9FRAME_TYPE_BIT_LENGTH) - 1);
    id <<= H9frame::H9FRAME_SEQNUM_BIT_LENGTH;
    id |= frame.seqnum & ((1<<H9frame::H9FRAME_SEQNUM_BIT_LENGTH) - 1);
    id <<= H9frame::H9FRAME_DESTINATION_ID_BIT_LENGTH;
    id |= frame.destination_id & ((1<<H9frame::H9FRAME_DESTINATION_ID_BIT_LENGTH) - 1);
    id <<= H9frame::H9FRAME_SOURCE_ID_BIT_LENGTH;
    id |= frame.source_id & ((1<<H9frame::H9FRAME_SOURCE_ID_BIT_LENGTH) - 1);


    std::ostringstream buf;
    buf << 'T';
    buf << std::setfill('0') << std::hex;
    buf << std::setw(8) << id;
    buf << std::setw(1) << static_cast<std::uint32_t >(frame.dlc);
    for (int i = 0; i < frame.dlc; ++i) {
        buf << std::setw(2) << static_cast<std::uint32_t >(frame.data[i]);
    }
    buf << "\r";
    return buf.str();
}

H9frame Slcan::parse_slcan_msg(const std::string& slcan_data) {
    H9frame res = H9frame();

    uint32_t id = std::stoi(slcan_data.substr(1, 8), nullptr, 16);

    res.priority = H9frame::from_underlying<H9frame::Priority >((id >> (H9frame::H9FRAME_TYPE_BIT_LENGTH + H9frame::H9FRAME_SEQNUM_BIT_LENGTH +
                                      H9frame::H9FRAME_DESTINATION_ID_BIT_LENGTH + H9frame::H9FRAME_SOURCE_ID_BIT_LENGTH)) & ((1<<H9frame::H9FRAME_PRIORITY_BIT_LENGTH) - 1));

    res.type = H9frame::from_underlying<H9frame::Type >((id >> (H9frame::H9FRAME_SEQNUM_BIT_LENGTH + H9frame::H9FRAME_DESTINATION_ID_BIT_LENGTH + H9frame::H9FRAME_SOURCE_ID_BIT_LENGTH)) \
		                    & ((1<<H9frame::H9FRAME_TYPE_BIT_LENGTH) - 1));

    res.seqnum = static_cast<std::uint8_t >((id >> (H9frame::H9FRAME_DESTINATION_ID_BIT_LENGTH + H9frame::H9FRAME_SOURCE_ID_BIT_LENGTH)) \
		                    & ((1<<H9frame::H9FRAME_TYPE_BIT_LENGTH) - 1));

    res.destination_id = static_cast<std::uint16_t >((id >> (H9frame::H9FRAME_SOURCE_ID_BIT_LENGTH)) & ((1<<H9frame::H9FRAME_DESTINATION_ID_BIT_LENGTH) - 1));

    res.source_id = static_cast<std::uint16_t >((id >> (0)) & ((1<<H9frame::H9FRAME_SOURCE_ID_BIT_LENGTH) - 1));

    uint32_t dlc = std::stoi(slcan_data.substr(9, 1), nullptr, 16);
    res.dlc = static_cast<std::uint8_t >(dlc);

    for (int i=0; i < dlc; ++i) {
        uint32_t tmp = std::stoi(slcan_data.substr(10 + i*2, 2), nullptr, 16);
        res.data[i] = static_cast<std::uint8_t >(tmp);
    }

    return res;
}

void Slcan::recv_data() {
    std::uint8_t buf[100];
    ssize_t nbyte = read(get_socket(), buf, sizeof(buf)-1);
    if (nbyte <= 0) {
        if (nbyte == 0 || errno == ENXIO) {
            close();
        }
        throw std::system_error(errno, std::system_category(), std::to_string(errno) + __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    buf[nbyte] = '\0';
    //std::cout << "recv raw(" << nbyte << "): " << buf << std::endl;
    for (int i = 0; i < nbyte; ++i) {
        recv_buf.push_back(buf[i]);
        if (buf[i] == '\r' || buf[i] == '\a') {
            parse_response(recv_buf);
            recv_buf.clear();
        }
    }
}

void Slcan::send_data(const H9frame& frame) {
    std::string buf = build_slcan_msg(frame);
    ssize_t nbyte = write(get_socket(), buf.c_str(), buf.size());
    //std::cout << "send raw: " << buf.c_str() << std::endl;
    if (nbyte <= 0) {
        if (nbyte == 0 || errno == ENXIO) {
            close();
        }
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    last_send = &frame;
}

void Slcan::parse_response(const std::string& response) {
    switch (response[0]) {
        case '\r':
            //std::cout << "pare \\r" << std::endl;
            if (last_send) {
                const H9frame* tmp = last_send;
                last_send = nullptr;
                //printf("debug: %p\n", tmp);
                on_frame_send(*tmp);
            }

            break;
        case 'T':
            if (response.size() >= 10)
                on_frame_recv(parse_slcan_msg(response));
            //send_ack();
            break;
    }
}

void Slcan::send_ack() {
    ssize_t nbyte = write(get_socket(), "\r", 1);
    if (nbyte <= 0) {
        if (nbyte == 0 || errno == ENXIO) {
            close();
        }
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
}

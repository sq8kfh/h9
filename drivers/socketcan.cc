/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-09.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "socketcan.h"

#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

SocketCAN::SocketCAN(const std::string& name, TRecvFrameCallback recv_frame_callback, TSendFrameCallback send_frame_callback, const std::string& interface):
        Driver(name, std::move(recv_frame_callback), std::move(send_frame_callback)), _interface(interface) {
}

void SocketCAN::open() {
    struct sockaddr_can addr;
    struct ifreq ifr;

    int sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sockfd == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    strcpy(ifr.ifr_name, _interface.c_str());
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    struct can_filter rfilter[1];
    rfilter[0].can_id = CAN_EFF_FLAG; //only frame with extended id
    rfilter[0].can_mask = CAN_EFF_FLAG;

    if (setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter)) < 0) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    set_socket(sockfd, true);
}

void SocketCAN::recv_data() {
    struct can_frame can_msg;

    size_t nbyte = read(get_socket(), &can_msg, sizeof(can_frame));
    if (nbyte <= 0) {
        if (nbyte == 0 || errno == ENXIO) {
            close();
        }
        throw std::system_error(errno, std::system_category(), std::to_string(errno) + __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    H9frame res = H9frame();

    res.priority = H9frame::from_underlying<H9frame::Priority >((can_msg.can_id >> (H9frame::H9FRAME_TYPE_BIT_LENGTH + H9frame::H9FRAME_SEQNUM_BIT_LENGTH +
                                                                        H9frame::H9FRAME_DESTINATION_ID_BIT_LENGTH + H9frame::H9FRAME_SOURCE_ID_BIT_LENGTH)) & ((1<<H9frame::H9FRAME_PRIORITY_BIT_LENGTH) - 1));

    res.type = H9frame::from_underlying<H9frame::Type >((can_msg.can_id >> (H9frame::H9FRAME_SEQNUM_BIT_LENGTH + H9frame::H9FRAME_DESTINATION_ID_BIT_LENGTH + H9frame::H9FRAME_SOURCE_ID_BIT_LENGTH)) \
		                    & ((1<<H9frame::H9FRAME_TYPE_BIT_LENGTH) - 1));

    res.seqnum = static_cast<std::uint8_t >((can_msg.can_id >> (H9frame::H9FRAME_DESTINATION_ID_BIT_LENGTH + H9frame::H9FRAME_SOURCE_ID_BIT_LENGTH)) \
		                    & ((1<<H9frame::H9FRAME_TYPE_BIT_LENGTH) - 1));

    res.destination_id = static_cast<std::uint16_t >((can_msg.can_id >> (H9frame::H9FRAME_SOURCE_ID_BIT_LENGTH)) & ((1<<H9frame::H9FRAME_DESTINATION_ID_BIT_LENGTH) - 1));

    res.source_id = static_cast<std::uint16_t >((can_msg.can_id >> (0)) & ((1<<H9frame::H9FRAME_SOURCE_ID_BIT_LENGTH) - 1));

    res.dlc = can_msg.can_dlc;
    for(int i = 0; i < 8; i++) {
        res.data[i] = can_msg.data[i];
    }

    on_frame_recv(res);
}

void SocketCAN::send_data(const H9frame& frame) {
    struct can_frame can_msg;
    memset(&can_msg, 0, sizeof(struct can_frame));

    can_msg.can_id |= H9frame::to_underlying(frame.priority) & ((1<<H9frame::H9FRAME_PRIORITY_BIT_LENGTH) - 1);
    can_msg.can_id <<= H9frame::H9FRAME_TYPE_BIT_LENGTH;
    can_msg.can_id |= H9frame::to_underlying(frame.type) & ((1<<H9frame::H9FRAME_TYPE_BIT_LENGTH) - 1);
    can_msg.can_id <<= H9frame::H9FRAME_SEQNUM_BIT_LENGTH;
    can_msg.can_id |= frame.seqnum & ((1<<H9frame::H9FRAME_SEQNUM_BIT_LENGTH) - 1);
    can_msg.can_id <<= H9frame::H9FRAME_DESTINATION_ID_BIT_LENGTH;
    can_msg.can_id |= frame.destination_id & ((1<<H9frame::H9FRAME_DESTINATION_ID_BIT_LENGTH) - 1);
    can_msg.can_id <<= H9frame::H9FRAME_SOURCE_ID_BIT_LENGTH;
    can_msg.can_id |= frame.source_id & ((1<<H9frame::H9FRAME_SOURCE_ID_BIT_LENGTH) - 1);

    can_msg.can_id |= CAN_EFF_FLAG;

    can_msg.can_dlc = frame.dlc;
    for(int i = 0; i < 8; i++) {
        can_msg.data[i] = frame.data[i];
    }

    ssize_t nbyte = write(get_socket(), &can_msg, sizeof(can_frame));
    if (nbyte <= 0) {
        if (nbyte == 0 || errno == ENXIO) {
            close();
        }
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    on_frame_send();
}

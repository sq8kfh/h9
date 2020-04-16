/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-04-14.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "framelogger.h"
#include <iomanip>

FrameLogger::FrameLogger(const std::string &send_frame_log_file, const std::string &recv_frame_log_file) {
    send_fram_fs = new std::ofstream;
    send_fram_fs->open(send_frame_log_file, std::ofstream::out | std::ofstream::app);
    if (send_frame_log_file != recv_frame_log_file) {
        recv_fram_fs = new std::ofstream;
        recv_fram_fs->open(recv_frame_log_file, std::ofstream::out | std::ofstream::app);
    }
    else {
        recv_fram_fs = send_fram_fs;
    }
}

FrameLogger::~FrameLogger() {
    if (send_fram_fs != recv_fram_fs) {
        delete recv_fram_fs;
        recv_fram_fs->close();
    }
    send_fram_fs->close();
    delete send_fram_fs;
}

void FrameLogger::log_send(const std::string& bus_id, const H9frame& frame) {
    *send_fram_fs << bus_id << ": " << frame.source_id << " -> " << frame.destination_id
       << " priority: " << (frame.priority == H9frame::Priority::HIGH ? 'H' : 'L')
       << " type: " << std::setw(2) << static_cast<unsigned int>(H9frame::to_underlying(frame.type))
       << " seqnum: " << std::setw(2) << static_cast<unsigned int>(frame.seqnum)
       << " dlc: " << static_cast<unsigned int>(frame.dlc);
    if (frame.dlc) {
        *send_fram_fs << " data:";
        std::ios oldState(nullptr);
        oldState.copyfmt(*send_fram_fs);

        for (int i = 0; i < frame.dlc; ++i) {
            *send_fram_fs << ' ' << std::setfill('0') << std::hex << std::setw(2)
                          << static_cast<unsigned int>(frame.data[i]);
        }
        send_fram_fs->copyfmt(oldState);
    }
    *send_fram_fs << std::endl;
}

void FrameLogger::log_recv(const std::string& bus_id, const H9frame& frame) {
    *recv_fram_fs << bus_id << ": " << frame.source_id << " -> " << frame.destination_id
                 << " priority: " << (frame.priority == H9frame::Priority::HIGH ? 'H' : 'L')
                 << " type: " << std::setw(2) << static_cast<unsigned int>(H9frame::to_underlying(frame.type))
                 << " seqnum: " << std::setw(2) << static_cast<unsigned int>(frame.seqnum)
                 << " dlc: " << static_cast<unsigned int>(frame.dlc);

    if (frame.dlc) {
        *recv_fram_fs << " data:";
        std::ios oldState(nullptr);
        oldState.copyfmt(*recv_fram_fs);

        for (int i = 0; i < frame.dlc; ++i) {
            *recv_fram_fs << ' ' << std::setfill('0') << std::hex << std::setw(2)
                          << static_cast<unsigned int>(frame.data[i]);
        }
        recv_fram_fs->copyfmt(oldState);
    }
    *recv_fram_fs << std::endl;
}

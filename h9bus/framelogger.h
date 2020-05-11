/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-04-14.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_FRAMELOGGER_H
#define H9_FRAMELOGGER_H

#include "config.h"
#include <fstream>
#include "busframe.h"

class FrameLogger {
private:
    std::ofstream* send_fram_fs;
    std::ofstream* recv_fram_fs;
public:
    FrameLogger(const std::string &send_frame_log_file, const std::string &recv_frame_log_file);
    ~FrameLogger();
    void log_send(const std::string& endpoint, BusFrame *busframe);
    void log_recv(const std::string& endpoint, BusFrame *busframe);
};


#endif //H9_FRAMELOGGER_H

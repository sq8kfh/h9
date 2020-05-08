/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-05-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_BUSFRAME_H
#define H9_BUSFRAME_H

#include "config.h"

#include "bus/h9frame.h"

class BusFrame {
private:
    const H9frame frame;
    const std::string origin;
    unsigned int total_endpoint_count;
    unsigned int completed_endpoint_count;
public:
    BusFrame(const H9frame& frame, const std::string& origin);
    const H9frame& get_frame() const;
    const std::string& get_origin() const;
    unsigned int inc_total_endpoint_count();
    unsigned int get_total_endpoint_count() const;
    unsigned int int_completed_endpoint_count();
    unsigned int get_completed_endpoint_count() const;
};


#endif //H9_BUSFRAME_H

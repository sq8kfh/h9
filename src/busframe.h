/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-05-08.
 *
 * Copyright (C) 2020-2023 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_BUSFRAME_H
#define H9_BUSFRAME_H

#include "config.h"
#include "ext_h9frame.h"
#include <future>

class BusFrame: public ExtH9Frame {
private:
    std::uint64_t orgin_client_id;
    std::uint64_t orgin_msg_id;

    std::promise<int> _send_promise;
    unsigned int _number_of_active_bus;
    unsigned int _send_counter;
public:
    BusFrame();
    //BusFrame(const BusFrame&) = delete;
    //BusFrame& operator=(const BusFrame&) = delete;
    BusFrame(const H9frame& frame, const std::string& origin, std::uint64_t orgin_client_id, std::uint64_t orgin_msg_id);
    BusFrame(ExtH9Frame && a) noexcept;


    std::uint64_t get_orgin_client_id(void) const;
    std::uint64_t get_orgin_msg_id(void) const;

    std::promise<int> &get_send_promise();
    void set_number_of_active_bus(unsigned int v);
    bool is_sent_finish();
    void inc_send_counter();
};


#endif //H9_BUSFRAME_H

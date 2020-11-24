/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-23.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_DEVICE_H
#define H9_DEVICE_H

#include "config.h"
#include <set>
#include <map>
#include <mutex>
#include "node.h"
#include "protocol/genericmsg.h"


class DevMgr;
class TCPClientThread;

class Device: protected Node {
private:
    const std::uint16_t node_type;
    const std::uint16_t node_version;

    friend class DevMgr;
    void update_device_state(H9frame frame);

    std::mutex event_name_mtx;
    std::map<std::string, std::set<TCPClientThread *>> event_observers;
    void attach_event_observer(TCPClientThread *observer, std::string event_name);
    void detach_event_observer(TCPClientThread *observer, std::string event_name);
protected:
    void notify_event_observer(std::string event_name, GenericMsg msg);
public:
    Device(Bus* bus, std::uint16_t node_id, std::uint16_t node_type, std::uint16_t node_version) noexcept;

    using Node::get_node_id;
    std::uint16_t get_node_type() noexcept;
    std::uint16_t get_node_version() noexcept;
};


#endif //H9_DEVICE_H

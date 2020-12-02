/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-21.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_EXECUTOR_H
#define H9_EXECUTOR_H

#include "config.h"
#include <ctime>
#include <string>
#include "bus.h"
#include "dctx.h"
#include "devmgr.h"
#include "protocol/devicemethodresponsemsg.h"
#include "protocol/executedevicemethodmsg.h"
#include "protocol/methodresponsemsg.h"


class Executor {
private:
    DCtx * const ctx;
    Bus * const bus;
    DevMgr * const devmgr;
public:
    explicit Executor(DCtx *ctx, Bus *bus, DevMgr *devmgr) noexcept;

    void get_stat(std::string &version, std::time_t &uptime);
    //void cleanup_connection(TCPClientThread *client);

    //DevMgr
    int active_devices_count();
    bool is_device_exist(std::uint16_t dev_id);
    std::vector<DevMgr::DeviceDsc> get_devices_list();
    MethodResponseMsg discover(TCPClientThread *client);

    //Device
    DeviceMethodResponseMsg attach_device_event_observer(TCPClientThread *client, std::uint16_t dev_id, std::string event_name);
    DeviceMethodResponseMsg detach_device_event_observer(TCPClientThread *client, std::uint16_t dev_id, std::string event_name);
    std::vector<std::string> get_device_events_list(std::uint16_t dev_id);

    std::vector<Device::RegisterDsc> get_device_registers_list(std::uint16_t dev_id);

    std::vector<std::string> get_device_methods_list(std::uint16_t dev_id);
    bool has_device_specific_method(std::uint16_t dev_id, std::string method_name);

    DeviceMethodResponseMsg execute_device_method(TCPClientThread *client, std::uint16_t device_id, std::string method_name, ExecuteDeviceMethodMsg &exedevcmsg);
};


#endif //H9_EXECUTOR_H

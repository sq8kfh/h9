/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-21.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "executoradapter.h"
#include "protocol/errormsg.h"
#include "tcpclientthread.h"


ExecutorAdapter::ExecutorAdapter(Executor *executor) noexcept: executor(executor) {
}

GenericMsg ExecutorAdapter::execute_method(CallMsg callmsg, TCPClientThread *client) {
    if (callmsg.get_method_name() == "h9d_stat") {
        std::string version;
        std::time_t uptime;

        executor->get_stat(version, uptime);

        ResponseMsg res("h9d_stat");
        res.add_value("version", version);
        res.add_value("uptime", uptime);
        //res.add_value("connected_clients_count", _server_mgr->connected_clients_count());

        res.set_request_id(callmsg.get_id());
        return std::move(res);
    }
    else if (callmsg.get_method_name() == "dev") {
        int active_antenna = executor->execute_object_method(callmsg["variable_antenna"].get_value_as_int(), client);

        ResponseMsg res("dev");
        res.add_value("object", callmsg["object"].get_value_as_str());
        res.add_value("id", callmsg["id"].get_value_as_int());
        res.add_value("method", callmsg["method"].get_value_as_str());
        res.add_value("response", active_antenna);

        res.set_request_id(callmsg.get_id());
        return std::move(res);
    }

    ErrorMsg err_msg(ErrorMsg::ErrorNumber::UNSUPPORTED_METHOD, "Unsupported method: " + callmsg.get_method_name());
    err_msg.set_request_id(callmsg.get_id());
    return std::move(err_msg);
}

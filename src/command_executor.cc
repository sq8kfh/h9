/*
 * Created by crowx on 26/10/2023.
 *
 */

#include "command_executor.h"
#include <spdlog/spdlog.h>
#include <iostream>

CommandExecutor::CommandExecutor(H9Connector* h9d_conn): h9d(h9d_conn) {

}

void CommandExecutor::get_and_parse_response(const jsonrpcpp::Id& request_id, const std::string& method) {
    jsonrpcpp::entity_ptr raw_msg;
    try {
        raw_msg = h9d->recv();
    }
    catch (std::system_error& e) {
        SPDLOG_ERROR("Messages receiving error: {}.", e.code().message());
        exit(EXIT_FAILURE);
    }
    catch (std::runtime_error& e) {
        SPDLOG_ERROR("Messages receiving error: {}.", e.what());
        exit(EXIT_FAILURE);
    }

    SPDLOG_TRACE("Received msg: {}.", raw_msg->to_json().dump());

    if (raw_msg->is_response()) {
        jsonrpcpp::response_ptr msg = std::dynamic_pointer_cast<jsonrpcpp::Response>(raw_msg);

        //if (msg->is_error()) { //it is now work
        if (msg->error().operator bool()) {
            auto err = msg->error();

            if (msg->id().int_id() != request_id.int_id() && err.code() != -32700) {
                SPDLOG_ERROR("Reguest, response id missmach: {} != {}.", request_id.int_id(), msg->id().int_id());
                SPDLOG_DEBUG("Dump response: {}", msg->to_json().dump());
            }

            printf("Error (%d): %s\n", err.code(), err.message().c_str());
        }
        else {
            if (msg->id().int_id() != request_id.int_id()) {
                SPDLOG_ERROR("Reguest, response id missmach: {} != {}.", request_id.int_id(), msg->id().int_id());
                SPDLOG_DEBUG("Dump response: {}", msg->to_json().dump());
            }

            auto result = msg->result();
            print_response(method, msg->result());
        }
    }
    else {
        SPDLOG_ERROR("Type {} message received, expected 'response' or 'error'.", raw_msg->type_str());
        SPDLOG_DEBUG("Dump response: {}", raw_msg->to_json().dump());
    }
}

void CommandExecutor::print_response(const std::string& method, const nlohmann::json& result) {
    if (method == "get_node_info") {
        tm ct_utc;
        strptime(result["created_time"].get<std::string>().c_str(), "%FT%TZ", &ct_utc);
        tm lst_utc;
        strptime(result["last_seen_time"].get<std::string>().c_str(), "%FT%TZ", &lst_utc);

        time_t ct_utc_ts = timegm(&ct_utc);
        time_t lst_utc_ts = timegm(&ct_utc);

        std::tm ct_tm;
        localtime_r(&ct_utc_ts, &ct_tm);
        std::tm lst_tm;
        localtime_r(&lst_utc_ts, &lst_tm);

        std::cout << "Name:          " << result["name"].get<std::string>() << std::endl;
        std::cout << "ID:            " << result["id"] << std::endl;
        std::cout << "Type:          " << result["type"] << std::endl;
        std::cout << "Version:       " << result["version_major"] << '.' << result["version_minor"] << '.' << result["version_patch"] << std::endl;
        std::cout << "Created time:  " << std::put_time(&ct_tm, "%Y-%m-%d %H:%M:%S")<< std::endl;
        std::cout << "Last seen:     " << std::put_time(&lst_tm, "%Y-%m-%d %H:%M:%S") << std::endl;
        std::cout << "Description:   " << result["description"] << std::endl;
    }
    else {
        std::cout << result << std::endl;
    }
}

void CommandExecutor::execute(const jsonrpcpp::Request& r) {
    jsonrpcpp::Id id(h9d->get_next_id());
    jsonrpcpp::Request request = {id, r.method(), r.params()};

    SPDLOG_DEBUG("Sending msg: {}.", request.to_json().dump().c_str());
    h9d->send(std::make_shared<jsonrpcpp::Request>(request));

    get_and_parse_response(id, r.method());
}

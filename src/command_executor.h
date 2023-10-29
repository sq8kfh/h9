/*
 * Created by crowx on 26/10/2023.
 *
 */

#pragma once

#include "h9connector.h"
#include <jsonrpcpp/jsonrpcpp.hpp>

class CommandExecutor {
  private:
    H9Connector* h9d;

    void get_and_parse_response(const jsonrpcpp::Id& request_id, const std::string& method);
    void print_response(const std::string& method, const nlohmann::json& result);
  public:
    explicit CommandExecutor(H9Connector* h9d_conn);

    void execute(const jsonrpcpp::Request& r);
};

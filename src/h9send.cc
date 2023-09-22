/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-09.
 *
 * Copyright (C) 2019-2023 Kamil Palkowski. All rights reserved.
 */

#include "config.h"

#include <cstdlib>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include "ext_h9frame.h"
#include "h9_configurator.h"
#include "h9connector.h"

namespace {

class H9SendConfigurator: public H9Configurator {
  private:
    void add_app_specific_opt() {
        // clang-format off
        options.add_options("")
                ("s,src_id", "Source id, if set, a raw frame will be sent", cxxopts::value<std::uint16_t>())
                ("d,dst_id", "Destination id", cxxopts::value<std::uint16_t>())
                ("S,seqnum", "Seqnum, if set, a raw frame will be sent", cxxopts::value<std::uint8_t>())
                ("H,high_priority", "High priority")
                ("t,type", "Frame type", cxxopts::value<std::underlying_type_t<H9frame::Type >>())
                ("r,repeat", "Repeat the frame every given time in seconds", cxxopts::value<unsigned int>())
                ;
//ctx.add_positional_options_list("data", "[hex data]", "");
                ;
        // clang-format on
        options.parse_positional("data");
        options.positional_help("data");

        options.add_options()("data", "[hex data]", cxxopts::value<std::vector<std::string>>());
    }

    void parse_app_specific_opt(const cxxopts::ParseResult& result) {
        extended = result.count("extended");
        simple = result.count("simple");
    }

  public:
    bool extended;
    bool simple;

    H9SendConfigurator():
        H9Configurator("h9send", "Sends frames to the H9 Bus.") {}
};

} // namespace

int main(int argc, char* argv[]) {
    H9SendConfigurator h9;
    h9.logger_initial_setup();
    cxxopts::ParseResult res = h9.parse_command_line_arg(argc, argv);
    h9.logger_setup();
    h9.load_configuration();
    /*
        ctx.add_options("s,src_id", "Source id", cxxopts::value<std::uint16_t>());
        ctx.add_options("i,dst_id", "Destination id", cxxopts::value<std::uint16_t>());
        ctx.add_options("H,high_priority", "High priority");
        ctx.add_options("t,type", "Frame type", cxxopts::value<std::underlying_type_t<H9frame::Type >>());
        ctx.add_options("r,repeat", "Repeat the frame every given time in seconds", cxxopts::value<unsigned int>());
        ctx.add_positional_options_list("data", "[hex data]", "");

        auto res = ctx.parse_options(argc, argv);

        ctx.load_configuration(res);
    */
    ExtH9Frame frame;
    bool raw = false;

    if (res.count("src_id")) {
        frame.source_id(res["src_id"].as<std::uint16_t>());
        raw = true;
    }
    else {
        frame.source_id(h9.get_default_source_id());
    }

    if (res.count("seqnum")) {
        frame.seqnum(res["seqnum"].as<std::uint8_t>());
        raw = true;
    }

    if (res.count("dst_id")) {
        frame.destination_id(res["dst_id"].as<std::uint16_t>());
    }

    if (res.count("high_priority")) {
        frame.priority(H9frame::Priority::HIGH);
    }
    else {
        frame.priority(H9frame::Priority::LOW);
    }

    if (res.count("type")) {
        frame.type(static_cast<H9frame::Type>(res["type"].as<std::underlying_type_t<H9frame::Type>>()));
    }

    frame.dlc(0);
    if (res.count("data")) {
        auto& v = res["data"].as<std::vector<std::string>>();
        std::vector<std::uint8_t> data;

        for (const auto& s : v) {
            data.push_back(std::stoul(s, nullptr, 16));
        }
        frame.dlc(data.size());
        frame.data(data);
    }

    H9Connector h9_connector = h9.get_connector();

    try {
        h9_connector.connect("h9send");
    }
    catch (std::system_error& e) {
        SPDLOG_ERROR("Can not connect to h9bus {}:{}: {}", h9.get_host(), h9.get_port(), e.code().message());
        exit(EXIT_FAILURE);
    }
    catch (std::runtime_error& e) {
        SPDLOG_ERROR("Can not connect to h9bus {}:{}: authentication fail", h9.get_host(), h9.get_port());
        exit(EXIT_FAILURE);
    }

    // std::cout << frame << std::endl;

    if (res.count("repeat")) {
        unsigned int sleep_time = res["repeat"].as<unsigned int>();

        int c = 0;
        while (true) {
            jsonrpcpp::Id id(c);
            frame.seqnum(c);
            jsonrpcpp::Request rf(std::move(id), "send_frame", nlohmann::json({{"frame", frame}, {"raw", raw}}));
            h9_connector.send(std::make_shared<jsonrpcpp::Request>(std::move(rf)));

            ++c;
            sleep(sleep_time);
        }
    }
    else {
        jsonrpcpp::Id id(1);
        jsonrpcpp::Request rf(id, "send_frame", nlohmann::json({{"frame", frame}, {"raw", raw}}));
        h9_connector.send(std::make_shared<jsonrpcpp::Request>(std::move(rf)));
    }
    return EXIT_SUCCESS;
}

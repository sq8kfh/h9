/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-01.
 *
 * Copyright (C) 2019-2023 Kamil Palkowski. All rights reserved.
 *
 * ./src/h9stats -s 2> /dev/null | jq '.bus.frames[] | select (.type == "TOGGLE_BIT")'
 *
 */

#include "config.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include "h9_configurator.h"
#include "h9connector.h"

void print_uptime(int n) {
    int day = n / (24 * 3600);

    n = n % (24 * 3600);
    int hour = n / 3600;

    n %= 3600;
    int minutes = n / 60;

    n %= 60;
    int seconds = n;

    if (day)
        std::cout << day << " "
                  << "days ";
    if (hour)
        std::cout << hour << " "
                  << "hours ";
    if (minutes)
        std::cout << minutes << " "
                  << "minutes ";
    std::cout << seconds << " "
              << "seconds";
}

class H9StatsConfigurator: public H9Configurator {
  private:
    void add_app_specific_opt() {
        // clang-format off
        options.add_options("")
            ("i,interval", "Refresh interval in a seconds.", cxxopts::value<int>()->default_value("15"))
            ("j,json", "JSON output.")
            ("s,oneshot", "One shot, forces JSON output.")
            ;
        // clang-format on
    }

    void parse_app_specific_opt(const cxxopts::ParseResult& result) {
        interval = result["interval"].as<int>();
        json = result.count("json");
        oneshot = result.count("oneshot");
    }

  public:
    int interval;
    bool json;
    bool oneshot;

    H9StatsConfigurator():
        H9Configurator("h9stats", "Show H9 statistic.") {}
};

int main(int argc, char** argv) {
    H9StatsConfigurator h9;
    h9.logger_initial_setup();
    h9.parse_command_line_arg(argc, argv);
    h9.logger_setup();
    h9.load_configuration();

    H9Connector h9_connector = h9.get_connector();

    try {
        h9_connector.connect("h9stat");
    }
    catch (std::system_error& e) {
        SPDLOG_ERROR("Can not connect to h9bus {}:{}: {}", h9.get_host(), h9.get_port(), e.code().message());
        exit(EXIT_FAILURE);
    }
    catch (std::runtime_error& e) {
        SPDLOG_ERROR("Can not connect to h9bus {}:{}: authentication fail", h9.get_host(), h9.get_port());
        exit(EXIT_FAILURE);
    }

    int id_i = 0;

    std::string h9d_version = "";
    if (!h9.json && !h9.oneshot) {
        jsonrpcpp::Id id(id_i);
        ++id_i;

        jsonrpcpp::Request r(id, "get_version");
        h9_connector.send(std::make_shared<jsonrpcpp::Request>(r));

        jsonrpcpp::entity_ptr raw_msg = h9_connector.recv();

        if (raw_msg->is_response()) {
            jsonrpcpp::response_ptr msg = std::dynamic_pointer_cast<jsonrpcpp::Response>(raw_msg);
            auto result = msg->result();
            h9d_version = result["version"].get<std::string>();
            if (result.contains("commit_sha")) {
                h9d_version += "-" + result["commit_sha"].get<std::string>();
                if (result.contains("dirty") && result["dirty"].get<bool>()) {
                    h9d_version += "-dirty";
                }
            }
        }
    }

    int last_uptime_value = 0;

    std::map<std::string, std::uint64_t> last_frames_sent;
    std::map<std::string, std::uint64_t> last_frames_received;

    do {
        jsonrpcpp::Id id(id_i);
        ++id_i;

        jsonrpcpp::Request r(id, "get_stats");
        h9_connector.send(std::make_shared<jsonrpcpp::Request>(r));

        jsonrpcpp::entity_ptr raw_msg = h9_connector.recv();

        if (raw_msg->is_response()) {
            jsonrpcpp::response_ptr msg = std::dynamic_pointer_cast<jsonrpcpp::Response>(raw_msg);
            auto result = msg->result();
            if (h9.json || h9.oneshot) {
                std::cout << result.dump() << std::endl;
                if (h9.oneshot) {
                    exit(EXIT_SUCCESS);
                }
            }
            else {
                std::system("clear");
                //  std::cout << "\x1b\x5b\x48\x1b\x5b\x32\x4a"; //Esc[2J

                std::cout << "h9d v" << h9d_version << " uptime: ";
                int uptime = result["uptime"].get<int>();
                print_uptime(uptime);
                std::cout << std::endl;

                //std::cout << " connected clients: " << result["connected_clients_count"].get_value_as_int() << std::endl;

                for (auto& endpoint : result["bus"]["endpoints"]) {
                    std::string name = endpoint["name"].get<std::string>();
                    std::cout << " Endpoint: " << name << std::endl;
                    std::uint64_t frames_sent = endpoint["send_frames"].get<std::uint64_t>();
                    float fps = frames_sent - last_frames_sent[name];
                    fps /= uptime - last_uptime_value;
                    std::cout << "  Frames sent: " << frames_sent << " (" << std::fixed << std::setprecision(2) << fps << " f/s)" << std::endl;
                    std::uint64_t frames_received = endpoint["received_frames"].get<std::uint64_t>();
                    fps = frames_received - last_frames_received[name];
                    fps /= uptime - last_uptime_value;
                    std::cout << "  Frames received: " << frames_received << " (" << std::fixed << std::setprecision(2) << fps << " f/s)" << std::endl;

                    last_frames_sent[name] = frames_sent;
                    last_frames_received[name] = frames_received;
                }

                if (h9.get_debug()) {
                    std::cout << msg->to_json().dump() << std::endl;
                }
                last_uptime_value = uptime;
            }
        }
        else {
            if (h9.get_debug())
                std::cerr << raw_msg->to_json().dump() << std::endl;
            std::cerr << "Receive unexpected message (type: " << raw_msg->type_str() << ")\n";
            return EXIT_FAILURE;
        }

        sleep(h9.interval);
    } while (!h9.oneshot);
    return EXIT_SUCCESS;
}

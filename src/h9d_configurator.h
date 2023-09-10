/*
 * H9 project
 *
 * Created by SQ8KFH on 2023-09-08.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include "config.h"
#include <confuse.h>
#include <string>
#include "bus.h"


class H9dConfigurator {
public:
    constexpr static char default_logger_name[] = "h9d";
    constexpr static char bus_logger_name[] = "bus";
    constexpr static char frames_logger_name[] = "frame";
    constexpr static char frames_recv_to_file_logger_name[] = "frames_recv_to_file";
    constexpr static char frames_sent_to_file_logger_name[] = "frames_sent_to_file";
private:
    constexpr static char log_debug_pattern[] = "%^[%L %T.%e] [%n] [%s:%#]%$ %v";
    constexpr static char log_pattern[] = "%^[%L %T.%e] [%n]%$ %v";

    constexpr static char default_config[] = H9_CONFIG_PATH  "h9d.conf";

    bool debug;
    int verbose;
    bool override_daemonize;
    bool foreground;
    std::string log_file;
    std::string logger_level_setting_string;
    std::string config_file;
    std::string override_pid_file;

    cfg_t *cfg;
    spdlog::sink_ptr log_file_sink;
public:
    H9dConfigurator();
    ~H9dConfigurator();
    void parse_command_line_arg(int argc, char **argv);
    void logger_initial_setup();
    void logger_setup();
    void load_configuration();

    void configure_bus(Bus *bus);

    void daemonize();
    void save_pid();
    void drop_privileges();
};
/*
 * H9 project
 *
 * Created by SQ8KFH on 2023-09-08.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#include "h9d_configurator.h"
#include <cxxopts/cxxopts.hpp>
#include <iostream>
#include <fstream>
#include <stdarg.h>
#include <spdlog/spdlog.h>
#include <spdlog/cfg/helpers.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "loop_driver.h"
#include "slcan_driver.h"
#include "socketcan_driver.h"


namespace {
    std::string last_confuse_error_message = "";
}

static void cfg_err_func(cfg_t *cfg, const char* fmt, va_list args) {
    char msgbuf[1024];
    vsnprintf(msgbuf, sizeof(msgbuf), fmt, args);
    last_confuse_error_message = msgbuf;
    SPDLOG_TRACE(msgbuf);
}

H9dConfigurator::H9dConfigurator():
    log_file(""),
    override_pid_file(""),
    config_file(default_config),
    cfg(nullptr),
    log_file_sink(nullptr),
    debug(false),
    verbose(0),
    override_daemonize(false),
    foreground(false) {

}

H9dConfigurator::~H9dConfigurator() {
    if (cfg) cfg_free(cfg);
}

void H9dConfigurator::parse_command_line_arg(int argc, char **argv) {
    cxxopts::Options options("h9d", "H9 daemon.");
    options.add_options("other")
#ifdef H9_DEBUG
            ("d,debug", "Enable debugging")
#endif
            ("h,help", "Show help")
            ("v,verbose", "Be more verbose")
            ("V,version", "Show version")
            ;
    options.add_options("daemon")
            ("c,config", "Config file", cxxopts::value<std::string>()->default_value(default_config))
            ("D,daemonize", "Run in the background")
            ("f,foreground", "Prevent go in the background")
            ("l,logfile", "Log file", cxxopts::value<std::string>())
            ("L,logger_level", "Logger level modification (e.g. 'frame=debug,bus=trace')", cxxopts::value<std::string>())
            ("p,pidfile", "PID file", cxxopts::value<std::string>())
            ;

    try {
        cxxopts::ParseResult result = options.parse(argc, argv);

        debug = result.count("debug");

        if (result.count("help")) {
            std::cerr << options.help({"", "daemon", "other"}) << std::endl;
            exit(EXIT_SUCCESS);
        }

        if (result.count("version")) {
            std::cerr << "h9d version " << H9_VERSION << " by crowx." << std::endl;
            std::cerr << "Copyright (C) 2017-2023 Kamil Palkowski. All rights reserved." << std::endl;
            exit(EXIT_SUCCESS);
        }

        verbose = result.count("verbose");

        if (result.count("config")) {
            config_file = std::string(result["config"].as<std::string>());
        }

        if (result.count("daemonize")) {
            override_daemonize = true;
        }

        if (result.count("foreground")) {
            foreground = true;
        }

        if (result.count("logfile")) {
            log_file = std::string(result["logfile"].as<std::string>());
        }

        if (result.count("logger_level")) {
            logger_level_setting_string = result["logger_level"].as<std::string>();
        }

        if (result.count("pidfile")) {
            override_pid_file = std::string(result["pidfile"].as<std::string>());
        }
    }
    catch (cxxopts::exceptions::parsing e) {
        std::cerr << e.what() << ". Try '-h' for more information." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void H9dConfigurator::logger_initial_setup() {
    if (log_file.empty()) {
        auto h9 = spdlog::stdout_color_mt(default_logger_name);
    }
    else {
        try {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            log_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file);

            std::vector<spdlog::sink_ptr> sinks = {console_sink, log_file_sink};
            auto h9 = std::make_shared<spdlog::logger>(default_logger_name, std::begin(sinks), std::end(sinks));

            h9->flush_on(spdlog::level::info);

            spdlog::register_logger(h9);
        }
        catch (spdlog::spdlog_ex e) {
            std::cerr << "Can not open log file: '" << log_file << "'." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    auto h9 = spdlog::get(default_logger_name);

    if (debug) h9->set_pattern(log_debug_pattern);
    else h9->set_pattern(log_pattern);

    spdlog::set_default_logger(h9);

    int tmp_level = h9->level() - verbose;
    if (tmp_level < spdlog::level::trace)
        h9->set_level(spdlog::level::trace);
    else
        h9->set_level(static_cast<spdlog::level::level_enum>(tmp_level));
}

void H9dConfigurator::logger_setup() {
    auto bus = spdlog::stdout_color_mt(bus_logger_name);
    auto frame_console = spdlog::stdout_color_mt(frames_logger_name);

    if (log_file_sink) {
        bus->sinks().push_back(log_file_sink);
        frame_console->sinks().push_back(log_file_sink);
    }

    spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) {
        if (debug) l->set_pattern(log_debug_pattern);
        else l->set_pattern(log_pattern);

        int tmp_level = l->level() - verbose;
        if (tmp_level < spdlog::level::trace)
            l->set_level(spdlog::level::trace);
        else
            l->set_level(static_cast<spdlog::level::level_enum>(tmp_level));
    });

    spdlog::cfg::helpers::load_levels(logger_level_setting_string);


    cfg_t *cfg_log = cfg_getsec(cfg, "log");
    std::shared_ptr<spdlog::sinks::basic_file_sink<spdlog::details::null_mutex>> frames_file_sink = nullptr;

    char *frames_logfile = nullptr;
    if (cfg_log) {
        frames_logfile = cfg_getstr(cfg_log, "frames_logfile");
        if (frames_logfile) {
            try {
                frames_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_st>(frames_logfile);
            }
            catch (spdlog::spdlog_ex e) {
                frames_file_sink = nullptr;
                SPDLOG_ERROR("Frames logger can not open file: {} - frames logging to the file disabled.", frames_logfile);
            }
        }
    }

    auto frames_recv_file = std::make_shared<spdlog::logger>(frames_recv_to_file_logger_name, frames_file_sink);
    auto frames_sent_file = std::make_shared<spdlog::logger>(frames_sent_to_file_logger_name, frames_file_sink);

    frames_recv_file->flush_on(spdlog::level::info);
    frames_sent_file->flush_on(spdlog::level::info);

    class frame_direction_flag : public spdlog::custom_flag_formatter
    {
    public:
        void format(const spdlog::details::log_msg &msg, const std::tm &, spdlog::memory_buf_t &dest) override
        {
            std::string direction = "?";
            if (msg.logger_name == frames_recv_to_file_logger_name) {
                direction = "in";
            }
            else if(msg.logger_name == frames_sent_to_file_logger_name) {
                direction = "out";
            }
            dest.append(direction.data(), direction.data() + direction.size());
        }

        std::unique_ptr<custom_flag_formatter> clone() const override
        {
            return spdlog::details::make_unique<frame_direction_flag>();
        }
    };

    if (!frames_file_sink) {
        frames_recv_file->set_level(spdlog::level::off);
        frames_sent_file->set_level(spdlog::level::off);
    }
    else {
        auto formatter = std::make_unique<spdlog::pattern_formatter>();
        formatter->add_flag<frame_direction_flag>('*').set_pattern(
                R"({"date": "%Y-%m-%dT%T.%e", "direction": "%*", %v})");
        frames_file_sink->set_formatter(std::move(formatter));

        if (cfg_getbool(cfg_log, "disable_recv_frames"))
            frames_recv_file->set_level(spdlog::level::off);
        else
            SPDLOG_INFO("Enabled logging recv frames to file: {}", frames_logfile);

        if (cfg_getbool(cfg_log, "disable_sent_frames"))
            frames_sent_file->set_level(spdlog::level::off);
        else
            SPDLOG_INFO("Enabled logging sent frames to file: {}", frames_logfile);
    }

    spdlog::register_logger(frames_sent_file);
    spdlog::register_logger(frames_recv_file);
}

void H9dConfigurator::load_configuration() {
    SPDLOG_INFO("Loading configuration from file '{}'...", config_file);

    cfg_opt_t cfg_process_sec[] = {
            CFG_BOOL("daemonize", cfg_false, CFGF_NONE),
            CFG_STR("pidfile", nullptr, CFGF_NONE),
            CFG_INT("setuid", 0, CFGF_NONE),
            CFG_INT("setgid", 0, CFGF_NONE),
            CFG_END()
    };

    cfg_opt_t cfg_server_sec[] = {
            CFG_INT("port", H9D_DEFAULT_PORT, CFGF_NONE),
            CFG_END()
    };

    cfg_opt_t cfg_log_sec[] = {
            //TODO: dodac log level
            CFG_STR("frames_logfile", nullptr, CFGF_NONE),
            CFG_BOOL("disable_sent_frames", cfg_false, CFGF_NONE),
            CFG_BOOL("disable_recv_frames", cfg_false, CFGF_NONE),
            CFG_END()
    };

    cfg_opt_t cfg_bus_sec[] = {
            CFG_STR("driver", nullptr, CFGF_NONE),
            CFG_STR("tty", nullptr, CFGF_NONE),
            CFG_STR("interface", nullptr, CFGF_NONE),
            CFG_END()
    };

    cfg_opt_t cfg_opts[] = {
            CFG_SEC("process", cfg_process_sec, CFGF_NONE),
            CFG_SEC("server", cfg_server_sec, CFGF_NONE),
            CFG_SEC("bus", cfg_bus_sec, CFGF_MULTI | CFGF_TITLE | CFGF_NO_TITLE_DUPES),
            CFG_SEC("log", cfg_log_sec, CFGF_NONE),
            CFG_END()
    };

    cfg = cfg_init(cfg_opts, CFGF_NONE);
    cfg_set_error_function(cfg, cfg_err_func);
    int ret = cfg_parse(cfg, config_file.c_str());

    if (ret == CFG_FILE_ERROR) {
        SPDLOG_CRITICAL("Unable to load a file: '{}'.", config_file);
        cfg_free(cfg);
        cfg = nullptr;
        exit(EXIT_FAILURE);
    } else if (ret == CFG_PARSE_ERROR) {
        SPDLOG_CRITICAL("Config file parse error: {}.", last_confuse_error_message);
        last_confuse_error_message = "";
        cfg_free(cfg);
        cfg = nullptr;
        exit(EXIT_FAILURE);
    }
}

void H9dConfigurator::configure_bus(Bus *bus) {
    //LoopDriver loop0 = LoopDriver("loop0");
    //LoopDriver loop1 = LoopDriver("loop1");
    //SocketCANDriver can0 = SocketCANDriver("can0", "can0");

    //Bus<Epoll> bus;
    //bus.add_driver(&can0);


    int n = cfg_size(cfg, "bus");
    for (int i = 0; i < n; i++) {
        cfg_t *bus_driver_section = cfg_getnsec(cfg, "bus", i);
        //if (bus_driver_section) {
        std::string bus_name = cfg_title(bus_driver_section);
        if (cfg_getstr(bus_driver_section, "driver")) {
            std::string driver = cfg_getstr(bus_driver_section, "driver");
            if (driver == "loop") {
                bus->add_driver(new LoopDriver(bus_name));
            }
            else if (driver == "SLCAN") {
                if (cfg_getstr(bus_driver_section, "tty")) {
                    std::string tty = cfg_getstr(bus_driver_section, "tty");
                    bus->add_driver(new SlcanDriver(bus_name, tty));
                }
                else {
                    SPDLOG_ERROR("Missing option 'connection_string' for {}.", cfg_title(bus_driver_section));
                }
            }
#ifdef H9_SOCKETCAN_DRIVER
            else if (driver == "SocketCAN") {
                if (cfg_getstr(bus_driver_section, "interface")) {
                    std::string interface = cfg_getstr(bus_driver_section, "interface");
                    bus->add_driver(new SocketCANDriver(bus_name, interface));
                    //SocketCANDriver("can0", "can0");
                }
                else {
                    SPDLOG_ERROR("Missing option 'interface' for {}.", cfg_title(bus_driver_section));
                }
            }
#endif
            else {
                SPDLOG_ERROR("Unsupported driver: '{}' for '{}' bus.", driver, bus_name);
            }
        }
        else {
            SPDLOG_ERROR("Missing option 'driver' for {}.", cfg_title(bus_driver_section));
        }
        //}
    }
}

void H9dConfigurator::daemonize() {
    if (foreground) return;

    cfg_t *cfg_process = cfg_getsec(cfg, "process");
    if (override_daemonize || (cfg_process && cfg_getbool(cfg_process, "daemonize"))) {

#ifdef __APPLE__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
#endif
        if (daemon(1, 0) != 0) {
#ifdef __APPLE__
#pragma GCC diagnostic pop
#endif
            SPDLOG_CRITICAL("Unable to daemonize: %s.", strerror(errno));
            exit(EXIT_FAILURE);
        }
        else {
            SPDLOG_INFO("Going to background - be like the daemon.");
        }
    }
}

void H9dConfigurator::save_pid() {
    std::string pid_file = "";
    if (override_pid_file.empty()) {
        cfg_t *cfg_process = cfg_getsec(cfg, "process");
        if (cfg_process) {
            char *ret = cfg_getstr(cfg_process, "pidfile");
            pid_file = ret ? ret : "";
        }
    }
    else {
        pid_file = override_pid_file;
    }
    if (!pid_file.empty()) {
        auto pid = getpid();
        std::ofstream _pid_file;
        _pid_file.open(pid_file, std::ofstream::out | std::ofstream::trunc);
        if (_pid_file.is_open()) {
            _pid_file << pid;
            _pid_file.close();
            SPDLOG_INFO("Saved PID ({}) in file: '{}'.", pid, pid_file);
        }
        else{
            SPDLOG_ERROR("Unable to save a PID in file: '{}'.", pid_file);
        }
    }
}

void H9dConfigurator::drop_privileges() {
    cfg_t *cfg_process = cfg_getsec(cfg, "process");
    if (cfg_process) {
        int uid = cfg_getint(cfg_process, "setuid");
        int gid = cfg_getint(cfg_process, "setgid");

        if (gid > 0) {
            if (setgid(gid) != 0) {
                SPDLOG_CRITICAL("Unable to drop group privileges to GID: {} - {}.", gid, strerror(errno));
                exit(EXIT_FAILURE);
            }
            else {
                SPDLOG_INFO("Dropped GID to: {}", gid);
            }
        }
        if (uid > 0) {
            if (getuid() != 0) {
                SPDLOG_ERROR("Unable to drop user privileges to GID: {} - you UID ({}) is not 0.", uid, getuid());
            }
            else if (setuid(uid) != 0) {
                SPDLOG_CRITICAL("Unable to drop user privileges to UID: {} - {}.", uid,strerror(errno));
                exit(EXIT_FAILURE);
            }
            else {
                SPDLOG_INFO("Dropped UID to: {}.", uid);
            }
        }
    }
}

#include "daemonctx.h"

DaemonCtx::DaemonCtx(const std::string& app_name, const std::string& app_desc): Ctx(app_name, app_desc) {
    _options.add_options("connection")
            ("c,config", "Config file")
            ;
}

void DaemonCtx::load_configuration(const cxxopts::ParseResult& opts) {
    std::cout << "config: " << opts["config"].as<std::string>() << std::endl;

}


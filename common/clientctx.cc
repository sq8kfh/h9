#include "clientctx.h"

ClientCtx::ClientCtx(const std::string& app_name, const std::string& app_desc): Ctx(app_name, app_desc) {
    _options.add_options("connection")
            ("c,connect", "Connection address", cxxopts::value<std::string>())
            ("p,port", "connection port", cxxopts::value<int>())
            ("F,config", "User config file", cxxopts::value<std::string>()->default_value(H9_USER_CONFIG_FILE))
            ;
}

void ClientCtx::load_configuration(const cxxopts::ParseResult& opts) {
    std::cout << "config: " << opts["config"].as<std::string>() << std::endl;

}

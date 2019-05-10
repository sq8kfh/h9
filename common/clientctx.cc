#include "clientctx.h"

ClientCtx::ClientCtx(const std::string& app_name, const std::string& app_desc):
        Ctx(app_name, app_desc),
        _options(app_name, app_desc) {
    _options.add_options("other")
#ifdef DEBUG
            ("d,debug", "Enable debugging")
#endif
            ("h,help", "Show help")
            ("v,verbose", "Be more verbose")
            ("V,version", "Show version")
            ;
    _options.add_options("connection")
            ("c,connect", "Connection address", cxxopts::value<std::string>())
            ("p,port", "connection port", cxxopts::value<int>())
            ("F,config", "User config file", cxxopts::value<std::string>()->default_value(H9_USER_CONFIG_FILE))
            ;
}

void ClientCtx::load_configuration(const cxxopts::ParseResult& opts) {
    std::cout << "config: " << opts["config"].as<std::string>() << std::endl;

}

void ClientCtx::add_options(const std::string& opts,
                      const std::string& desc,
                      std::shared_ptr<const cxxopts::Value> value,
                      std::string arg_help) {
    _options.add_options()(opts, desc, std::move(value), std::move(arg_help));
}

cxxopts::ParseResult ClientCtx::parse_options(int argc, char* argv[]) {
    try {
        cxxopts::ParseResult result = _options.parse(argc, argv);
#ifdef DEBUG
        if (result.count("debug")) {
            _debug = true;
        }
#endif
        if (result.count("help"))
        {
            std::cerr << _options.help({"", "connection", "other"}) << std::endl;
            exit(EXIT_SUCCESS);
        }

        if (result.count("verbose")) {
            _verbose = static_cast<unsigned int>(result.count("verbose"));
        }

        if (result.count("version"))
        {
            std::cerr << _app_name << " version " << H9_VERSION << " by SQ8KFH" << std::endl;
            exit(EXIT_SUCCESS);
        }

        return result;
    }
    catch (cxxopts::OptionException& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
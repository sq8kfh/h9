#include <utility>
#include "ctx.h"
#include "config.h"

void Ctx::raise_verbose_level(unsigned int how_much) {
    _verbose += how_much;
}

Ctx::Ctx(const std::string& app_name, const std::string& app_desc):
        _app_name(app_name),
        _debug(false),
        _verbose(0),
        _options(app_name, app_desc) {

    _options.add_options("other")
#ifdef DEBUG
            ("d,debug", "Enable debugging")
#endif
            ("h,help", "Show help")
            ("v,verbose", "Be more verbose")
            ("V,version", "Show version")
            ;
}

inline unsigned int Ctx::verbose_level() {
    return _verbose;
}

inline bool Ctx::debug_enabled() {
#ifdef DEBUG
    return _debug;
#else
    return false;
#endif
}

void Ctx::add_options(const std::string& opts,
                 const std::string& desc,
                 std::shared_ptr<const cxxopts::Value> value,
                 std::string arg_help) {
    _options.add_options()(opts, desc, std::move(value), std::move(arg_help));
}

cxxopts::ParseResult Ctx::parse_options(int argc, char* argv[]) {
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

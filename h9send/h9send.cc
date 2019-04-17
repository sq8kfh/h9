#include <cstdlib>

#include "common/clientctx.h"

int main(int argc, char* argv[]) {
    ClientCtx ctx = ClientCtx("h9send", "Sends data to the H9 Bus.");

    ctx.add_options("l,local", "some local option", cxxopts::value<int>());

    auto res = ctx.parse_options(argc, argv);

    ctx.load_configuration(res);

    if (res.count("local")) {
        std::cout << res["local"].as<int>();
    }
    return EXIT_SUCCESS;
}

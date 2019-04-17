#include <cstdlib>

#include "common/clientctx.h"

int main(int argc, char **argv)
{
    ClientCtx ctx = ClientCtx("h9spy", "The H9 bus packets sniffer.");

    auto res = ctx.parse_options(argc, argv);
    ctx.load_configuration(res);

    return EXIT_SUCCESS;
}

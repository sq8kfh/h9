#include <cstdlib>

#include "common/clientctx.h"

int main(int argc, char **argv)
{
    ClientCtx ctx = ClientCtx("h9fwu", "Uploads a firmware to devices via the H9 bus.");

    auto res = ctx.parse_options(argc, argv);
    ctx.load_configuration(res);

    return EXIT_SUCCESS;
}

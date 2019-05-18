#include "config.h"
#include <cstdlib>

#include "protocol/h9connector.h"
#include "common/clientctx.h"

int main(int argc, char **argv)
{
    ClientCtx ctx = ClientCtx("h9fwu", "Uploads a firmware to devices via the H9 bus.");

    auto res = ctx.parse_options(argc, argv);
    ctx.load_configuration(res);

    H9Connector h9_connector = {ctx.get_h9bus_host(), ctx.get_h9bus_port()};
    h9_connector.connect();


    return EXIT_SUCCESS;
}

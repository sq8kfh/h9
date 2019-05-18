#include "config.h"
#include <cstdlib>

#include "protocol/h9connector.h"
#include "common/clientctx.h"

int main(int argc, char **argv)
{
    ClientCtx ctx = ClientCtx("h9fwu", "Uploads a firmware to devices via the H9 bus.");

    ctx.add_options("s,src_id", "Source id", cxxopts::value<std::uint16_t>());
    ctx.add_options("i,dst_id", "Destination id", cxxopts::value<std::uint16_t>());
    ctx.add_options("b,noupgrademsg", "Don't send the NODE_UPGRADE frame before");
    ctx.add_positional_options("ihex", "<ihex file>", "");

    auto res = ctx.parse_options(argc, argv);
    ctx.load_configuration(res);

    H9Connector h9_connector = {ctx.get_h9bus_host(), ctx.get_h9bus_port()};
    h9_connector.connect();


    return EXIT_SUCCESS;
}

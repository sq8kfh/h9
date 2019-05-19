#include "config.h"
#include <cstdlib>

#include "protocol/h9connector.h"
#include "protocol/subscribemsg.h"
#include "protocol/framereceivedmsg.h"
#include "common/clientctx.h"

int main(int argc, char **argv)
{
    ClientCtx ctx = ClientCtx("h9spy", "The H9 bus packets sniffer.");

    ctx.add_options("e,extended", "Extended output");

    auto res = ctx.parse_options(argc, argv);
    ctx.load_configuration(res);

    H9Connector h9_connector = {ctx.get_h9bus_host(), ctx.get_h9bus_port()};
    h9_connector.connect();

    h9_connector.send(SubscribeMsg(SubscribeMsg::Content::FRAME));

    while (true) {
        GenericMsg raw_msg = h9_connector.recv();
        if (raw_msg.get_type() == GenericMsg::Type::FRAME_RECEIVED) {
            FrameReceivedMsg msg = std::move(raw_msg);

            std::cout << msg.get_frame() <<std::endl;
        }
    }
    return EXIT_SUCCESS;
}

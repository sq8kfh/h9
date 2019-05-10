#ifndef _H9_BUS_H_
#define _H9_BUS_H_

#include <map>

#include "driver.h"
#include "socketmgr.h"
#include "common/ctx.h"

class BusMgr {
private:
    std::map<int, Driver*> dev;
    SocketMgr const *_socket_mgr;
public:
    BusMgr(SocketMgr *socket_mgr);
    void load_config(Ctx *ctx);
};


#endif //_H9_BUS_H_

#include <utility>

#include <utility>

/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-16.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "h9connector.h"

#include <system_error>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "common/logger.h"


H9Connector::H9Connector(std::string hostname, std::string port) noexcept:
    H9Socket(std::move(hostname), std::move(port)) {
}

H9Connector::~H9Connector() noexcept {
}

int H9Connector::connect() noexcept {
    return H9Socket::connect();
}

GenericMsg H9Connector::recv() {
    std::string data;

    int res;
    for(res = H9Socket::recv(data); res == -2; res = H9Socket::recv(data));
    //printf("<%s>%d\n", data.c_str(), res);
    return GenericMsg(data);
}

void H9Connector::send(const GenericMsg &msg) {
    std::string raw_msg = msg.serialize();
    H9Socket::send(raw_msg);
}

#include <cstdlib>
#include <cstring>
#include "loop.h"

Loop::Loop() {
    bzero(&loopback_addr, sizeof(loopback_addr));
    loopback_addr.sin_family = AF_INET;
    loopback_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    loopback_addr.sin_port = htons(61432);
}

int Loop::open() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (bind(sockfd, (const struct sockaddr *)&loopback_addr, sizeof(loopback_addr)) == -1) {
        perror("bind failed");
        exit(1);
    }

    setSocket(sockfd);

    return sockfd;
}

void Loop::close() {
    ::close(getSocket());
    setSocket(0);
}

H9frame Loop::recv() {
    sockaddr_in tmp_addr;
    socklen_t len = sizeof(tmp_addr);
    bcopy(&loopback_addr, &tmp_addr, len);

    H9frame buf;
    if(recvfrom(getSocket(), &buf, sizeof(buf), 0, (struct sockaddr *)&tmp_addr, &len) == -1)
    {
        perror("recvfrom fails");
    }
    printf("recv \n");
    return buf;
}

void Loop::send(const H9frame& frame) {
    if( sendto(getSocket(), &frame, sizeof(frame), 0, (const struct sockaddr *)&loopback_addr, sizeof(loopback_addr)) == -1)
    {
        perror("sendto fails");
        exit(2);
    }
}

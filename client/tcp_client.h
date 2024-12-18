#pragma once
#ifdef _MSC_VER
#include <Winsock2.h>
#include <Ws2tcpip.h>

#define SHUT_RDWR SD_BOTH
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#endif


class tcp_client {
public:
    bool connect(const char* ip, int port);
    bool update(bool only_send);
    void close(bool is_shutdown);

private:
    bool send_package();
    bool recv_package();

private:
    int sock_fd = -1;
    int send_index = 0;
    char buffer[1024];
};

inline bool set_nonblocking(int sock_fd)
{
#ifndef _MSC_VER
    int flags = fcntl(sock_fd, F_GETFL, 0);
    if (fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK)) {
        return false;
    }
#endif
    return true;
}
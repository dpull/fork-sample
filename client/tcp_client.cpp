#include "tcp_client.h"
#include "util.h"
#include <cstdio>
#include <cstring>
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

bool tcp_client::connect(const char* ip, int port)
{
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (::connect(sock_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        log("error", "connect failed");
        return false;
    }
#ifndef _MSC_VER
    int flags = fcntl(sock_fd, F_GETFL, 0);
    if (fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK)) {
        log("error", "set non blocking failed");
        return false;
    }
#endif
    return true;
}

bool tcp_client::update(bool only_send)
{
    if (!only_send) {
        recv_package();
    }

    send_package();
    return true;
}

bool tcp_client::send_package()
{
    snprintf(buffer, sizeof(buffer), "%d@%d", os::getpid(), send_index++);

    if (send(sock_fd, buffer, sizeof(buffer), 0) != sizeof(buffer)) {
        log("error", "failed to send data: %s error:[%d] %s", buffer, errno, strerror(errno));
        return false;
    }
    return true;
}

bool tcp_client::recv_package()
{
#ifndef _MSC_VER
    u_long pending = 0;
    if (ioctl(sock_fd, FIONREAD, &pending) != 0) {
        log("error", "failed to get pending data size");
        return false;
    }

    if (pending < sizeof(buffer)) {
        return true;
    }
#endif

    int left = sizeof(buffer);
    while (left > 0) {
        int n = recv(sock_fd, buffer, left, 0);
        if (n == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                os::sleep(10);
                continue;
            }
            log("error", "failed to recv data");
            return false;
        } else if (n == 0) {
            log("error", "connection closed");
            return false;
        }
        left -= n;
    }
    log("info", "received data: %s", buffer);
    return true;
}

void tcp_client::close(bool is_shutdown)
{
    if (sock_fd == -1)
        return;

    if (is_shutdown)
        shutdown(sock_fd, SHUT_RDWR);

    close(sock_fd);
    sock_fd = -1;
}
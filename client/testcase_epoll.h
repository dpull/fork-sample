#pragma once
#include "testcase.h"
#include "util.h"
#include "tcp_client.h"
#include <unordered_map>


struct testcase_epoll : testcase {
    const int port = 8241;

    virtual ~testcase_epoll() 
    { 
        
    }

    bool add_epoll_in(int sock_fd)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = sock_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
            log("error", "epoll_ctl: server_fd");
            return false;
        }
        return true;
    }

    bool init_server_fd() {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            return false;
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
            log("error", "setsockopt");
            return false;
        }

        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            log("error", "bind failed");
            return false;
        }

        if (listen(server_fd, 3) < 0) {
            log("error", "listen");
            return false;
        }

        set_nonblocking(server_fd);
        add_epoll_in(server_fd);

        return true;
    }

    virtual bool parent_init() override
    {
        epoll_fd = epoll_create(0);
        if (epoll_fd == -1) {
            return false;
        }

        if (!init_server_fd()) {
            return false;
        }
        
        return true;
    }
    virtual bool parent_tick() override
    {
        int n = epoll_wait(epoll_fd, events, sizeof(events) / sizeof(events[0]), -1);
        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == server_fd) {
                int new_socket;
                while ((new_socket = accept(server_fd, NULL, NULL)) != -1) {
                    set_nonblocking(new_socket);
                    client_fd[new_socket] = add_epoll_in(new_socket);
                }
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    log("error", "accept");
                }
            } else {
                int sockfd = events[i].data.fd;
                if (!client_fd[sockfd]) { 
                    log("error", "not in epoll");
                }

                int bytes_read = read(sockfd, buffer, sizeof(buffer));
                if (bytes_read <= 0) { 
                    log("error", "close fd:%d", sockfd);
                    close(sockfd);
                }
            }
        }
        return true;
    }

    virtual bool child_init() override
    {
        
        return true;
    }

    virtual bool child_tick() override
    {

        return true;
    }


    char buffer[1024];
    struct epoll_event events[4096];
    std::unordered_map<int, bool> client_fd;
    int server_fd = -1;
    int client_fd = -1;
    int epoll_fd = -1;
};
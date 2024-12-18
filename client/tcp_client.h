#pragma once

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
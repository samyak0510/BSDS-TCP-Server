#pragma once
#include <cstdint>
#include <string>

int create_server_socket(uint16_t port, int backlog = 16);

int accept_client(int listen_fd);

int connect_to_server(const std::string &ip, uint16_t port);

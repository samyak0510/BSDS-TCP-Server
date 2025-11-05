#include "Net.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>
#include <string>

int create_server_socket(uint16_t port, int backlog)
{
  int fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
    throw std::runtime_error("socket() failed");

  int yes = 1;
  ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if (::bind(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
  {
    ::close(fd);
    throw std::runtime_error("bind() failed (port in use?)");
  }
  if (::listen(fd, backlog) < 0)
  {
    ::close(fd);
    throw std::runtime_error("listen() failed");
  }
  return fd;
}

int accept_client(int listen_fd)
{
  sockaddr_in cli{};
  socklen_t len = sizeof(cli);
  int cfd = ::accept(listen_fd, reinterpret_cast<sockaddr *>(&cli), &len);
  return cfd;
}

int connect_to_server(const std::string &ip, uint16_t port)
{
  int fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
    throw std::runtime_error("socket() failed");

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (::inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) != 1)
  {
    ::close(fd);
    throw std::runtime_error("inet_pton() failed (bad IP?)");
  }
  if (::connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
  {
    ::close(fd);
    throw std::runtime_error("connect() failed");
  }
  return fd;
}

#include "ClientStub.h"
#include "Net.h"

#include <stdexcept>
#include <unistd.h>

ClientStub::ClientStub() : fd_(-1) {}
ClientStub::~ClientStub() { Close(); }

void ClientStub::Init(const std::string &ip, int port)
{
  if (fd_ != -1)
    Close();
  fd_ = connect_to_server(ip, static_cast<uint16_t>(port));
}

RobotInfo ClientStub::Order(const ::Order &order)
{
  if (fd_ == -1)
    throw std::runtime_error("ClientStub not initialized");
  if (!send_order(fd_, order))
    throw std::runtime_error("send_order failed");
  RobotInfo r{};
  if (!recv_robot(fd_, r))
    throw std::runtime_error("recv_robot failed");
  return r;
}

void ClientStub::Close()
{
  if (fd_ != -1)
  {
    ::close(fd_);
    fd_ = -1;
  }
}

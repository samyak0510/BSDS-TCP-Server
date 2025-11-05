#pragma once
#include <string>
#include "Protocol.h"

class ClientStub
{
public:
  ClientStub();
  ~ClientStub();

  // TCP Connection
  void Init(const std::string &ip, int port);

  // sends an order and waiting for robot info.
  RobotInfo Order(const Order &order);

  void Close();

private:
  int fd_; // connected socket
};

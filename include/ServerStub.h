#pragma once
#include "Protocol.h"

class ServerStub
{
public:
  explicit ServerStub(int connected_fd);
  ~ServerStub();

  bool ReceiveOrder(Order &out);

  bool ShipRobot(const RobotInfo &robot);

  int fd() const { return fd_; }

private:
  int fd_;
};

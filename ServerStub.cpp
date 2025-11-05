#include "ServerStub.h"
#include <unistd.h>

ServerStub::ServerStub(int connected_fd) : fd_(connected_fd) {}
ServerStub::~ServerStub()
{
  if (fd_ != -1)
    ::close(fd_);
}

bool ServerStub::ReceiveOrder(Order &out)
{
  return recv_order(fd_, out);
}

bool ServerStub::ShipRobot(const RobotInfo &robot)
{
  return send_robot(fd_, robot);
}

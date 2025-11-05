#pragma once
#include <cstdint>
#include <vector>
#include <stdexcept>

enum class MsgType : uint32_t
{
  ORDER = 1,
  ROBOT = 2
};

struct Order
{
  int32_t customer_id;
  int32_t order_number;
  int32_t robot_type;
};

struct RobotInfo
{
  int32_t customer_id;
  int32_t order_number;
  int32_t robot_type;
  int32_t engineer_id;
  int32_t expert_id;
};

// read/write
ssize_t read_n(int fd, void *buf, size_t n);
ssize_t write_n(int fd, const void *buf, size_t n);

// Framing
bool send_frame(int fd, MsgType type, const std::vector<uint8_t> &payload);
bool recv_frame(int fd, MsgType &type, std::vector<uint8_t> &payload);

// helpers
bool send_order(int fd, const Order &o);
bool recv_order(int fd, Order &out);

bool send_robot(int fd, const RobotInfo &r);
bool recv_robot(int fd, RobotInfo &out);

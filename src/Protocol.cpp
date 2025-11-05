#include "Protocol.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>

namespace
{
  inline void put32(std::vector<uint8_t> &v, int32_t host_i)
  {
    uint32_t n = htonl(static_cast<uint32_t>(host_i));
    uint8_t *p = reinterpret_cast<uint8_t *>(&n);
    v.insert(v.end(), p, p + 4);
  }

  inline int32_t get32(const std::vector<uint8_t> &v, size_t &off)
  {
    if (off + 4 > v.size())
      throw std::runtime_error("decode int32: out of bounds");
    uint32_t n = 0;
    std::memcpy(&n, v.data() + off, 4);
    off += 4;
    return static_cast<int32_t>(ntohl(n));
  }

  std::vector<uint8_t> serialize_order(const Order &o)
  {
    std::vector<uint8_t> b;
    b.reserve(12);
    put32(b, o.customer_id);
    put32(b, o.order_number);
    put32(b, o.robot_type);
    return b;
  }

  Order deserialize_order(const std::vector<uint8_t> &b)
  {
    size_t off = 0;
    Order o{};
    o.customer_id = get32(b, off);
    o.order_number = get32(b, off);
    o.robot_type = get32(b, off);
    if (off != b.size())
      throw std::runtime_error("order trailing bytes");
    return o;
  }

  std::vector<uint8_t> serialize_robot(const RobotInfo &r)
  {
    std::vector<uint8_t> b;
    b.reserve(20);
    put32(b, r.customer_id);
    put32(b, r.order_number);
    put32(b, r.robot_type);
    put32(b, r.engineer_id);
    put32(b, r.expert_id);
    return b;
  }

  RobotInfo deserialize_robot(const std::vector<uint8_t> &b)
  {
    size_t off = 0;
    RobotInfo r{};
    r.customer_id = get32(b, off);
    r.order_number = get32(b, off);
    r.robot_type = get32(b, off);
    r.engineer_id = get32(b, off);
    r.expert_id = get32(b, off);
    if (off != b.size())
      throw std::runtime_error("robot trailing bytes");
    return r;
  }
} // namespace

ssize_t read_n(int fd, void *buf, size_t n)
{
  uint8_t *p = static_cast<uint8_t *>(buf);
  size_t left = n;
  while (left > 0)
  {
    ssize_t r = ::read(fd, p, left);
    if (r < 0)
    {
      if (errno == EINTR)
        continue;
      return -1;
    }
    if (r == 0)
      return n - left; // EOF
    left -= static_cast<size_t>(r);
    p += r;
  }
  return static_cast<ssize_t>(n);
}

ssize_t write_n(int fd, const void *buf, size_t n)
{
  const uint8_t *p = static_cast<const uint8_t *>(buf);
  size_t left = n;
  while (left > 0)
  {
    ssize_t w = ::write(fd, p, left);
    if (w < 0)
    {
      if (errno == EINTR)
        continue;
      return -1;
    }
    if (w == 0)
      continue;
    left -= static_cast<size_t>(w);
    p += w;
  }
  return static_cast<ssize_t>(n);
}

bool send_frame(int fd, MsgType type, const std::vector<uint8_t> &payload)
{
  uint32_t t = htonl(static_cast<uint32_t>(type));
  uint32_t l = htonl(static_cast<uint32_t>(payload.size()));
  if (write_n(fd, &t, 4) != 4)
    return false;
  if (write_n(fd, &l, 4) != 4)
    return false;
  if (!payload.empty() && write_n(fd, payload.data(), payload.size()) != (ssize_t)payload.size())
    return false;
  return true;
}

bool recv_frame(int fd, MsgType &type, std::vector<uint8_t> &payload)
{
  uint32_t t = 0, l = 0;
  ssize_t r1 = read_n(fd, &t, 4);
  if (r1 == 0)
    return false;
  if (r1 != 4)
    return false; // error or partial
  ssize_t r2 = read_n(fd, &l, 4);
  if (r2 != 4)
    return false;

  t = ntohl(t);
  l = ntohl(l);
  if (l > (32u * 1024u * 1024u))
    return false; // just in case if the frame is too large
  payload.assign(l, 0);
  if (l > 0 && read_n(fd, payload.data(), l) != (ssize_t)l)
    return false;

  type = static_cast<MsgType>(t);
  return true;
}

bool send_order(int fd, const Order &o)
{
  return send_frame(fd, MsgType::ORDER, serialize_order(o));
}

bool recv_order(int fd, Order &out)
{
  MsgType t;
  std::vector<uint8_t> p;
  if (!recv_frame(fd, t, p))
    return false;
  if (t != MsgType::ORDER)
    return false;
  out = deserialize_order(p);
  return true;
}

bool send_robot(int fd, const RobotInfo &r)
{
  return send_frame(fd, MsgType::ROBOT, serialize_robot(r));
}

bool recv_robot(int fd, RobotInfo &out)
{
  MsgType t;
  std::vector<uint8_t> p;
  if (!recv_frame(fd, t, p))
    return false;
  if (t != MsgType::ROBOT)
    return false;
  out = deserialize_robot(p);
  return true;
}

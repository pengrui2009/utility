/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-11-21 18:13:41
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-15 11:22:55
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#include <ara/log/logger.h>
#include <ara/socket/socket.h>
#include <gtest/gtest.h>

#include <atomic>
#include <functional>
#include <thread>
using namespace ara::socket::udp;
using namespace ara::socket;

TEST(Udp, Sender) {
  int32_t port = 8888;
  char data[10];
  auto buf = make_buffer(10);
  SocketUdp sender("0.0.0.0", 0, 1);
  EXPECT_EQ(sender.Mac(), "");
  auto f = sender.SendTo("192.168.1.3", port, data, 10);
  EXPECT_EQ(f.get(), 10);
  auto f2 = sender.SendTo("192.168.1.3", port, buf);
  EXPECT_EQ(f2.get(), 10);
}

TEST(Udp, Receiver) {
  int32_t port = 8888;
  char data[10];
  auto buf = make_buffer(10);
  bool exit = false;
  SocketUdp receiver("0.0.0.0", port, 1);
  receiver.SetReceiveCallback([&](ara::socket::SocketBufferType&& data,
                                  const std::string& ip, const uint32_t& port) {
    EXPECT_EQ(data.Size(), 10);
    EXPECT_EQ(ip, "127.0.0.1");
    EXPECT_EQ(port, 8887);
    exit = true;
  });
  receiver.StartReceiving();
  SocketUdp sender("0.0.0.0", 8887, 1);
  auto f = sender.SendTo("0.0.0.0", port, data, 10);
  EXPECT_EQ(f.get(), 10);
  while (!exit) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  sender.Close();
  receiver.Close();
}

TEST(Udp, Multicast) {
  int32_t port = 8888;
  const char* multicast_addr = "226.0.0.1";
  char data[10];
  auto buf = make_buffer(10);
  bool exit = false;
  SocketUdp receiver("0.0.0.0", port, 1);
  receiver.JoinGroup(multicast_addr);
  receiver.SetReceiveCallback([&](ara::socket::SocketBufferType&& data,
                                  const std::string& ip, const uint32_t& port) {
    EXPECT_EQ(data.Size(), 10);
    EXPECT_EQ(port, 8887);
    exit = true;
  });
  receiver.StartReceiving();
  SocketUdp sender("0.0.0.0", 8887, 1);
  auto f = sender.SendTo(multicast_addr, port, data, 10);
  EXPECT_EQ(f.get(), 10);
  while (!exit) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  sender.Close();
  receiver.Close();
}

TEST(Udp, Broadcast) {
  int32_t port = 8888;
  const char* broadcast_addr = "255.255.255.255";
  char data[10];
  auto buf = make_buffer(10);
  bool exit = false;
  SocketUdp receiver("0.0.0.0", port, 1);
  EXPECT_EQ(receiver.GetBroadcastIP(), "");
  receiver.SetReceiveCallback([&](ara::socket::SocketBufferType&& data,
                                  const std::string& ip, const uint32_t& port) {
    EXPECT_EQ(data.Size(), 10);
    EXPECT_EQ(port, 8887);
    exit = true;
  });
  receiver.StartReceiving();
  SocketUdp sender("0.0.0.0", 8887, 1);
  auto f = sender.SendTo(broadcast_addr, port, data, 10);
  EXPECT_EQ(f.get(), 10);
  while (!exit) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  sender.Close();
  receiver.Close();
}

int main(int argc, char** argv) {
  try {
    ::testing::InitGoogleTest(&argc, argv);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  } catch (...) {
  }
  return RUN_ALL_TESTS();
}
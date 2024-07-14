/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-16 16:01:46
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-28 14:10:29
 * @FilePath: /aeg-adaptive-autosar/ara-api/common/ipc/test/test_connect.cc
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include <ara/ipc/ipc.h>
#include <gtest/gtest.h>

using namespace ara::ipc;

using MSG = ara::ipc::IPCMessage;
using NEWMSG = ara::ipc::NewIpcMessage;

template <typename T>
class MyProcessor : public ara::ipc::ProcessHandler<T> {
 public:
  virtual T OnReadCallBack(T& msg) override { return msg; };
};

TEST(IPCMsgClient, connect) {
  ClientDomainSocket<> client(1, "gtest1.sock");
  auto f = client.connect();
  EXPECT_FALSE(f.get());
  std::shared_ptr<MyProcessor<MSG>> processor(new MyProcessor<MSG>);
  ServerDomainSocket<> server(4, "gtest1.sock", processor);
  server.Run();
  f = client.connect();
  EXPECT_TRUE(f.get());
  client.disconnect();
  std::this_thread::sleep_for(
      std::chrono::milliseconds(10));  // for server response
  EXPECT_EQ(server.Connetions(), 0);
}

TEST(NewIpcMsgClient, connect) {
  ClientDomainSocket<NEWMSG> client(1, "gtest1.sock");
  auto f = client.connect();
  EXPECT_FALSE(f.get());
  std::shared_ptr<MyProcessor<NEWMSG>> processor(new MyProcessor<NEWMSG>);
  ServerDomainSocket<NEWMSG> server(4, "gtest1.sock", processor);
  server.Run();
  f = client.connect();
  EXPECT_TRUE(f.get());
  client.disconnect();
  std::this_thread::sleep_for(
      std::chrono::milliseconds(10));  // for server response
  EXPECT_EQ(server.Connetions(), 0);
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
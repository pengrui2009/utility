/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-16 16:01:46
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-28 14:11:34
 * @FilePath: /aeg-adaptive-autosar/ara-api/common/ipc/test/test_client.cc
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

TEST(IPCMsgClient, disconnect_and_reconnect) {
  /* connect */
  std::shared_ptr<MyProcessor<MSG>> processor(new MyProcessor<MSG>);
  ServerDomainSocket<> server(4, "gtest3.sock", processor);
  server.Run();
  ClientDomainSocket<> client(1, "gtest3.sock");
  auto f = client.connect();
  EXPECT_TRUE(f.get());

  /* disconnect */
  client.disconnect();
  EXPECT_FALSE(client.Connected());
  /* reconnect */
  auto f2 = client.connect();
  EXPECT_TRUE(f2.get());
  /* communication */
  char request[1024] = "ipc test";
  MSG msg(strlen(request), request);
  auto s = client.Send(msg);
  auto r = s.get();
  EXPECT_EQ(msg.payload_length(), r.payload_length());
  EXPECT_EQ(msg.timestamp(), r.timestamp());
  for (int i = 0; i < msg.payload_length(); i++) {
    EXPECT_EQ(msg.payload()[i], r.payload()[i]);
  }
  server.Stop();
}

TEST(NewIpcMsgClient, disconnect_and_reconnect) {
  /* connect */
  std::shared_ptr<MyProcessor<NEWMSG>> processor(new MyProcessor<NEWMSG>);
  ServerDomainSocket<NEWMSG> server(4, "gtest3.sock", processor);
  server.Run();
  ClientDomainSocket<NEWMSG> client(1, "gtest3.sock");
  auto f = client.connect();
  EXPECT_TRUE(f.get());

  /* disconnect */
  client.disconnect();
  EXPECT_FALSE(client.Connected());
  /* reconnect */
  auto f2 = client.connect();
  EXPECT_TRUE(f2.get());
  /* communication */
  char request[1024] = "ipc test";
  NEWMSG msg(strlen(request), request);
  auto s = client.Send(msg);
  auto r = s.get();
  EXPECT_EQ(msg.payload_length(), r.payload_length());
  EXPECT_EQ(msg.timestamp(), r.timestamp());
  for (int i = 0; i < msg.payload_length(); i++) {
    EXPECT_EQ(msg.payload()[i], r.payload()[i]);
  }
  server.Stop();
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
/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-16 16:01:46
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-07-31 15:38:22
 * @FilePath:
 * /aeg-adaptive-autosar/ara-api/common/ipc/test/test_communication.cc
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

TEST(IPCMsgClient, communication) {
  /* connect */
  std::shared_ptr<MyProcessor<MSG>> processor(new MyProcessor<MSG>);
  ServerDomainSocket<MSG> server(4, "gtest2.sock", processor);
  server.Run();
  ClientDomainSocket<MSG> client(1, "gtest2.sock");
  auto f = client.connect();
  EXPECT_TRUE(f.get());

  /* communication */
  char request[1024] = "ipc test";
  MSG msg(strlen(request), request, IPCMessageMode::RPC);
  auto s = client.Send(msg);
  auto r = s.get();
  EXPECT_EQ(server.Connetions(), 1);
  EXPECT_EQ(msg.payload_length(), r.payload_length());
  EXPECT_EQ(msg.timestamp(), r.timestamp());
  for (int i = 0; i < msg.payload_length(); i++) {
    EXPECT_EQ(msg.payload()[i], r.payload()[i]);
  }
  /* disconnect */
  client.disconnect();
  EXPECT_FALSE(client.Connected());
  auto f1 = client.connect();
  EXPECT_TRUE(f1.get());

  auto s1 = client.Send(msg);
  auto r1 = s1.get();
  EXPECT_EQ(server.Connetions(), 1);
  EXPECT_EQ(msg.payload_length(), r1.payload_length());
  EXPECT_EQ(msg.timestamp(), r1.timestamp());
  for (int i = 0; i < msg.payload_length(); i++) {
    EXPECT_EQ(msg.payload()[i], r1.payload()[i]);
  }
  server.Stop();
}

TEST(IPCMsgClient, communication_fire_and_forget) {
  /* connect */
  std::shared_ptr<MyProcessor<MSG>> processor(new MyProcessor<MSG>);
  ServerDomainSocket<MSG> server(4, "gtest2.sock", processor);
  server.Run();
  ClientDomainSocket<MSG> client(1, "gtest2.sock");
  auto f = client.connect();
  EXPECT_TRUE(f.get());

  /* communication */
  char request[1024] = "ipc test";
  MSG msg(strlen(request), request, IPCMessageMode::FIRE_AND_FORGET);
  auto s = client.Send(msg);
  auto r = s.get();
  EXPECT_EQ(IPCMessageMode::FIRE_AND_FORGET, r.ipc_mode());
  EXPECT_EQ(server.Connetions(), 1);
  /* disconnect */
  client.disconnect();
  EXPECT_FALSE(client.Connected());
  server.Stop();
}

TEST(NEWIPCMsgClient, communication_fire_and_forget) {
  /* connect */
  std::shared_ptr<MyProcessor<NEWMSG>> processor(new MyProcessor<NEWMSG>);
  ServerDomainSocket<NEWMSG> server(4, "gtest2.sock", processor);
  server.Run();
  ClientDomainSocket<NEWMSG> client(1, "gtest2.sock");
  auto f = client.connect();
  EXPECT_TRUE(f.get());

  /* communication */
  char request[1024] = "ipc test";
  NEWMSG msg(strlen(request), request, IPCMessageMode::FIRE_AND_FORGET);
  auto s = client.Send(msg);
  auto r = s.get();
  EXPECT_EQ(IPCMessageMode::FIRE_AND_FORGET, r.ipc_mode());
  EXPECT_EQ(server.Connetions(), 1);
  /* disconnect */
  client.disconnect();
  EXPECT_FALSE(client.Connected());
  server.Stop();
}

TEST(NewIpcMsgClient, communication) {
  /* connect */
  std::shared_ptr<MyProcessor<NEWMSG>> processor(new MyProcessor<NEWMSG>);
  ServerDomainSocket<NEWMSG> server(4, "gtest2.sock", processor);
  server.Run();
  ClientDomainSocket<NEWMSG> client(1, "gtest2.sock");
  auto f = client.connect();
  EXPECT_TRUE(f.get());

  /* communication */
  char request[1024] = "ipc test";
  NEWMSG msg(strlen(request), request);
  auto s = client.Send(msg);
  auto r = s.get();
  EXPECT_EQ(server.Connetions(), 1);
  EXPECT_EQ(msg.payload_length(), r.payload_length());
  EXPECT_EQ(msg.timestamp(), r.timestamp());
  for (int i = 0; i < msg.payload_length(); i++) {
    EXPECT_EQ(msg.payload()[i], r.payload()[i]);
  }
  /* disconnect */
  client.disconnect();
  EXPECT_FALSE(client.Connected());
  auto f1 = client.connect();
  EXPECT_TRUE(f1.get());

  auto s1 = client.Send(msg);
  auto r1 = s1.get();
  EXPECT_EQ(server.Connetions(), 1);
  EXPECT_EQ(msg.payload_length(), r1.payload_length());
  EXPECT_EQ(msg.timestamp(), r1.timestamp());
  for (int i = 0; i < msg.payload_length(); i++) {
    EXPECT_EQ(msg.payload()[i], r1.payload()[i]);
  }
  server.Stop();
}

TEST(IPCMsgClient, communication_with_env) {
#ifdef __QNX__
  char cwd[] = "/storage/";
  setenv("AP_DOMAIN_SOCKET_FILE_PREFIX_PATH", cwd, 1);
#else
  char cwd[] = "/tmp/";
  setenv("AP_DOMAIN_SOCKET_FILE_PREFIX_PATH", cwd, 1);
#endif
  /* connect */
  std::shared_ptr<MyProcessor<MSG>> processor(new MyProcessor<MSG>);
  ServerDomainSocket<MSG> server(4, "gtest3.sock", processor);
  server.Run();
  ClientDomainSocket<MSG> client(1, "gtest3.sock");
  auto f = client.connect();
  EXPECT_TRUE(f.get());

  /* communication */
  char request[1024] = "ipc test";
  MSG msg(strlen(request), request, IPCMessageMode::RPC);
  auto s = client.Send(msg);
  auto r = s.get();
  EXPECT_EQ(server.Connetions(), 1);
  EXPECT_EQ(msg.payload_length(), r.payload_length());
  EXPECT_EQ(msg.timestamp(), r.timestamp());
  for (int i = 0; i < msg.payload_length(); i++) {
    EXPECT_EQ(msg.payload()[i], r.payload()[i]);
  }
  /* disconnect */
  client.disconnect();
  EXPECT_FALSE(client.Connected());
  auto f1 = client.connect();
  EXPECT_TRUE(f1.get());

  auto s1 = client.Send(msg);
  auto r1 = s1.get();
  EXPECT_EQ(server.Connetions(), 1);
  EXPECT_EQ(msg.payload_length(), r1.payload_length());
  EXPECT_EQ(msg.timestamp(), r1.timestamp());
  for (int i = 0; i < msg.payload_length(); i++) {
    EXPECT_EQ(msg.payload()[i], r1.payload()[i]);
  }
  unsetenv("AP_DOMAIN_SOCKET_FILE_PREFIX_PATH");
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
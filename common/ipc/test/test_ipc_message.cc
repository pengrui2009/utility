/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-16 16:01:46
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-07-31 14:28:02
 * @FilePath: /aeg-adaptive-autosar/ara-api/common/ipc/test/test_ipc_message.cc
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include <ara/ipc/ipc_message.h>
#include <ara/ipc/new_ipc_message.h>
#include <gtest/gtest.h>

using namespace ara::ipc;

TEST(IPCMessage, ctor) {
  IPCMessage msg;
  EXPECT_EQ(msg.payload_length(), 0U);
  EXPECT_EQ(msg.payload(), msg.header() + msg.header_length);
  const char* test =
      "2022/08/25 23:28:00.997612  541103708 010 ECU1 ---- ipc- log info V 13 "
      "[rec_msg_ after read header: 0x60 0x00 "
      "0x00 0x00 0x5c 0xf8 0x9d 0xd5 0x82 0x01 0x00 0x00";
  // int len = 1024;
  int len = strlen(test);
  char d[len];
  memset(d, 'a', len);
  IPCMessage msg1(len, test);
  std::cout << std::hex
            << static_cast<uint16_t>(*(static_cast<char*>(msg1.header())))
            << std::endl;
  EXPECT_EQ(msg1.payload_length(), len);
  for (int i = 0; i < msg1.payload_length(); i++) {
    EXPECT_EQ(msg1.payload()[i], test[i]);
  }
  std::cout << std::dec << msg1.timestamp() << std::endl;
  std::cout << msg1.timestamp_format() << std::endl;
}

TEST(NewIpcMessage, ctor) {
  NewIpcMessage msg;
  EXPECT_EQ(msg.payload_length(), 0U);
  EXPECT_NE(msg.payload(), msg.header() + msg.header_length);
  const char* test =
      "2022/08/25 23:28:00.997612  541103708 010 ECU1 ---- ipc- log info V 13 "
      "[rec_msg_ after read header: 0x60 0x00 "
      "0x00 0x00 0x5c 0xf8 0x9d 0xd5 0x82 0x01 0x00 0x00";
  // int len = 1024;
  int len = strlen(test);
  char d[len];
  memset(d, 'a', len);
  IPCMessage msg1(len, test);
  std::cout << std::hex
            << static_cast<uint16_t>(*(static_cast<char*>(msg1.header())))
            << std::endl;
  EXPECT_EQ(msg1.payload_length(), len);
  for (int i = 0; i < msg1.payload_length(); i++) {
    EXPECT_EQ(msg1.payload()[i], test[i]);
  }
  std::cout << std::dec << msg1.timestamp() << std::endl;
  std::cout << msg1.timestamp_format() << std::endl;
}

TEST(NewIpcMessage, copy) {
  int len = 1024;
  char d[len];
  memset(d, 'a', len);
  NewIpcMessage msg1(len, d);
  // copy ctor
  NewIpcMessage msg2 = msg1;
  for (int i = 0; i < msg2.payload_length(); i++) {
    EXPECT_EQ(msg2.payload()[i], 'a');
  }
  EXPECT_FALSE(msg2.header() == msg1.header());
  // assignment ctor
  NewIpcMessage msg3;
  msg3 = msg1;
  for (int i = 0; i < msg3.payload_length(); i++) {
    EXPECT_EQ(msg3.payload()[i], 'a');
  }
  EXPECT_FALSE(msg3.header() == msg1.header());
}

TEST(IPCMessage, copy) {
  int len = 1024;
  char d[len];
  memset(d, 'a', len);
  IPCMessage msg1(len, d);
  // copy ctor
  IPCMessage msg2 = msg1;
  for (int i = 0; i < msg2.payload_length(); i++) {
    EXPECT_EQ(msg2.payload()[i], 'a');
  }
  EXPECT_FALSE(msg2.header() == msg1.header());
  // assignment ctor
  IPCMessage msg3;
  msg3 = msg1;
  for (int i = 0; i < msg3.payload_length(); i++) {
    EXPECT_EQ(msg3.payload()[i], 'a');
  }
  EXPECT_FALSE(msg3.header() == msg1.header());
}

TEST(IPCMessage, move) {
  char d[30];
  memset(d, 'a', 30);
  IPCMessage msg1(30, d);
  IPCMessage msg2 = std::move(msg1);
  EXPECT_EQ(msg1.payload(), nullptr);
  for (int i = 0; i < msg2.payload_length(); i++) {
    EXPECT_EQ(msg2.payload()[i], 'a');
  }

  IPCMessage msg3 = std::move(IPCMessage(4, d));
  for (int i = 0; i < msg3.payload_length(); i++) {
    EXPECT_EQ(msg3.payload()[i], 'a');
  }
}

TEST(NewIpcMessage, move) {
  char d[30];
  memset(d, 'a', 30);
  NewIpcMessage msg1(30, d);
  std::cout << "123" << std::endl;
  NewIpcMessage msg2 = std::move(msg1);
  std::cout << "1234" << std::endl;
  EXPECT_EQ(msg1.payload(), nullptr);
  for (int i = 0; i < msg2.payload_length(); i++) {
    EXPECT_EQ(msg2.payload()[i], 'a');
  }

  NewIpcMessage msg3 = std::move(NewIpcMessage(4, d));
  for (int i = 0; i < msg3.payload_length(); i++) {
    EXPECT_EQ(msg3.payload()[i], 'a');
  }
}

TEST(IPCMessage, destroy) {
  std::vector<IPCMessage> msgs;
  for (int i = 0; i < 1; i++) {
    IPCMessage msg;
    msgs.emplace_back(msg);
  }

  char d[30];
  memset(d, 'a', 30);
  { IPCMessage msg1(30, d); }  // will not destroy d
  EXPECT_EQ(d[29], 'a');
}

TEST(IPCMessage, mode) {
  IPCMessage msg(IPCMessageMode::RPC);
  EXPECT_EQ(msg.payload_length(), 0U);
  EXPECT_EQ(msg.payload(), msg.header() + msg.header_length);
  EXPECT_EQ(msg.ipc_mode(), IPCMessageMode::RPC);
  const char* test =
      "2022/08/25 23:28:00.997612  541103708 010 ECU1 ---- ipc- log info V 13 "
      "[rec_msg_ after read header: 0x60 0x00 "
      "0x00 0x00 0x5c 0xf8 0x9d 0xd5 0x82 0x01 0x00 0x00";
  // int len = 1024;
  int len = strlen(test);
  char d[len];
  memset(d, 'a', len);
  IPCMessage msg1(len, test);
  std::cout << std::hex
            << static_cast<uint16_t>(*(static_cast<char*>(msg1.header())))
            << std::endl;
  EXPECT_EQ(msg1.payload_length(), len);
  for (int i = 0; i < msg1.payload_length(); i++) {
    EXPECT_EQ(msg1.payload()[i], test[i]);
  }
  std::cout << std::dec << msg1.timestamp() << std::endl;
  std::cout << msg1.timestamp_format() << std::endl;
}

TEST(NewIpcMessage, mode) {
  NewIpcMessage msg(IPCMessageMode::RPC);
  EXPECT_EQ(msg.payload_length(), 0U);
  std::cout << msg.header_length << std::endl;
  EXPECT_EQ(msg.payload(), nullptr);
  EXPECT_EQ(msg.ipc_mode(), IPCMessageMode::RPC);
  const char* test =
      "2022/08/25 23:28:00.997612  541103708 010 ECU1 ---- ipc- log info V 13 "
      "[rec_msg_ after read header: 0x60 0x00 "
      "0x00 0x00 0x5c 0xf8 0x9d 0xd5 0x82 0x01 0x00 0x00";
  // int len = 1024;
  int len = strlen(test);
  char d[len];
  memset(d, 'a', len);
  IPCMessage msg1(len, test, IPCMessageMode::FIRE_AND_FORGET);
  std::cout << std::hex
            << static_cast<uint16_t>(*(static_cast<char*>(msg1.header())))
            << std::endl;
  EXPECT_EQ(msg1.payload_length(), len);
  EXPECT_EQ(msg1.ipc_mode(), IPCMessageMode::FIRE_AND_FORGET);
  for (int i = 0; i < msg1.payload_length(); i++) {
    EXPECT_EQ(msg1.payload()[i], test[i]);
  }
  std::cout << std::dec << msg1.timestamp() << std::endl;
  std::cout << msg1.timestamp_format() << std::endl;
}

TEST(NewIpcMessage, destroy) {
  std::vector<NewIpcMessage> msgs;
  for (int i = 0; i < 1; i++) {
    NewIpcMessage msg;
    msgs.emplace_back(msg);
  }

  char d[30];
  memset(d, 'a', 30);
  { NewIpcMessage msg1(30, d); }  // will not destroy d
  EXPECT_EQ(d[29], 'a');
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
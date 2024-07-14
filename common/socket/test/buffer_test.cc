/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-11-21 18:13:41
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-15 11:21:25
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#include <ara/log/logger.h>
#include <ara/socket/data_buffer.h>
#include <gtest/gtest.h>

#include <atomic>
#include <functional>
#include <thread>
using namespace ara::socket;

TEST(DataBuffer, Construct) {
  auto buf = make_buffer(2000);
  EXPECT_EQ(buf.Size(), 2000);
  EXPECT_NE(buf.Data(), nullptr);
}

TEST(DataBuffer, Copy) {
  auto buf = make_buffer(5);
  for (size_t i = 0; i < buf.Size(); i++) {
    *(static_cast<DefaultBufferType*>(buf.Data() + i)) = i;
  }
  auto buf1 = buf;
  for (size_t i = 0; i < buf1.Size(); i++) {
    EXPECT_EQ(buf1.Data()[i], buf.Data()[i]);
  }
  auto buf2(buf);
  for (size_t i = 0; i < buf2.Size(); i++) {
    EXPECT_EQ(buf2.Data()[i], buf.Data()[i]);
  }
}

TEST(DataBuffer, Move) {
  auto buf = make_buffer(5);
  for (size_t i = 0; i < buf.Size(); i++) {
    *(static_cast<DefaultBufferType*>(buf.Data() + i)) = i;
  }
  auto buf_copy = buf;
  auto buf1 = std::move(buf);
  EXPECT_EQ(buf.Size(), 0);
  EXPECT_EQ(buf.Data(), nullptr);
  EXPECT_NE(buf1.Data(), nullptr);
  for (size_t i = 0; i < buf1.Size(); i++) {
    EXPECT_EQ(buf1.Data()[i], buf_copy.Data()[i]);
  }
  auto buf2(std::move(buf1));
  EXPECT_EQ(buf1.Size(), 0);
  EXPECT_EQ(buf1.Data(), nullptr);
  EXPECT_NE(buf2.Data(), nullptr);
  for (size_t i = 0; i < buf2.Size(); i++) {
    EXPECT_EQ(buf2.Data()[i], buf_copy.Data()[i]);
  }
}

TEST(DataBuffer, Resize) {
  auto buf = make_buffer(5);
  EXPECT_EQ(buf.Size(), 5);
  EXPECT_NE(buf.Data(), nullptr);
  auto data_add = buf.Data();
  for (size_t i = 0; i < buf.Size(); i++) {
    *(static_cast<DefaultBufferType*>(buf.Data() + i)) = i;
  }
  buf.Resize(10);
  EXPECT_EQ(buf.Size(), 10);
  EXPECT_NE(buf.Data(), data_add);
}

int main(int argc, char** argv) {
  try {
    ara::log::InitLogging("buf", "ara socket data buffer unit test",
                          ara::log::LogLevel::kInfo,
                          ara::log::LogMode::kConsole);
    ::testing::InitGoogleTest(&argc, argv);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  } catch (...) {
  }
  return RUN_ALL_TESTS();
}
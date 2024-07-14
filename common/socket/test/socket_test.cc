/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2023-01-12 16:25:20
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-09 15:06:19
 * @Description:
 *
 * Copyright (c) 2023 by Tusimple, All Rights Reserved.
 */
#include <ara/log/logger.h>
#include <ara/socket/socket.h>
#include <gtest/gtest.h>

#include <atomic>
#include <functional>
#include <thread>

using namespace ara::socket::tcp;
using namespace ara::socket;

TEST(Tcp, Sender) {}

int main(int argc, char** argv) {
  try {
    ::testing::InitGoogleTest(&argc, argv);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  } catch (...) {
  }
  return RUN_ALL_TESTS();
}

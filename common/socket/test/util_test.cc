/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-11-21 18:13:41
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-08 17:15:10
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#include <ara/log/logger.h>
#include <ara/socket/util.h>
#include <gtest/gtest.h>

#include <atomic>
#include <functional>
#include <thread>
using namespace ara::socket;

TEST(Util, GetMacFromName) {
  auto res = GetAllInterfaces();
  for (auto interface : res) {
    EXPECT_EQ(interface.mac, GetMacFromName(interface.name));
  }
}

TEST(Util, GetNameFromIp) {
  auto res = GetAllInterfaces();
  for (auto interface : res) {
    EXPECT_EQ(interface.name, GetNameFromIp(interface.address));
  }
}

TEST(Util, GetMacFromIp) {
  auto res = GetAllInterfaces();
  for (auto interface : res) {
    EXPECT_EQ(interface.mac, GetMacFromIp(interface.address));
  }
}

TEST(Util, GetInterfaceFromName) {
  auto res = GetAllInterfaces();
  for (auto interface : res) {
    EXPECT_EQ(interface, GetInterfaceFromName(interface.name));
  }
}

int main(int argc, char** argv) {
  try {
    ara::log::InitLogging("util", "ara socket util unit test",
                          ara::log::LogLevel::kInfo,
                          ara::log::LogMode::kConsole);
    ::testing::InitGoogleTest(&argc, argv);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  } catch (...) {
  }
  return RUN_ALL_TESTS();
}
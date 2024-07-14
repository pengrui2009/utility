/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-06-20 19:29:19
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-11-28 20:00:32
 * @FilePath: /adaptive_autosar/ara-api/log/test/initial_test.cpp
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include <gtest/gtest.h>

#include <thread>

#include "ara/log/logger.h"
#include "ara/log/logmanager.h"

TEST(LOGTEST, LogManager) {
  auto& inst = ara::log::LogManager::instance();
  EXPECT_EQ(&inst, &ara::log::LogManager::instance());
  inst.LogManagerInitialize(
      ara::core::StringView("TESTID"), ara::core::StringView("TEST APP"),
      ara::log::LogLevel::kVerbose,
      ara::log::LogMode::kConsole | ara::log::LogMode::kFile,
      ara::log::MessageMode::kNonModeled,
      ara::log::internal::FileModeDescription("", ""));
  auto& logger = inst.createLogContext(ara::core::StringView("ABCDEDF"),
                                       ara::core::StringView("TEST CTX"), 0,
                                       ara::log::LogLevel::kError);
  EXPECT_EQ(logger.getId(), "ABCD");
}

TEST(LOGTEST, CreateContext) {
  auto& inst = ara::log::LogManager::instance();
  auto& logger = inst.createLogContext(ara::core::StringView("INTM"),
                                       ara::core::StringView("TEST INTM"), 0,
                                       ara::log::LogLevel::kError);
  EXPECT_EQ(logger.getId(), "XXXX");
  for (int i = 0; i < 10; i++) {
    std::thread t([&]() {
      auto& logger = inst.createLogContext(
          ara::core::StringView(
              ara::core::StringView(std::to_string(i).c_str())),
          ara::core::StringView("TEST INTM"), 0, ara::log::LogLevel::kVerbose);
      logger.LogInfo() << logger.getId();
    });
    t.join();
  }
}

TEST(LOGTEST, InitiLogging) {
  ara::log::InitLogging(
      "TSN1", "example for AP logger", ara::log::LogLevel::kVerbose,
      (ara::log::LogMode::kConsole | ara::log::LogMode::kRemote),
      ara::log::MessageMode::kNonModeled,
      ara::log::internal::FileModeDescription(
          "./log/", "", true, ara::log::internal::FileEncode::kBinary,
          ara::log::internal::FileSaveMode::kSize, 5U, 10240U));

  ara::log::InitLogging(
      "TSN2", "example for AP logger", ara::log::LogLevel::kVerbose,
      ara::log::LogMode::kConsole, ara::log::MessageMode::kNonModeled,
      ara::log::internal::FileModeDescription(
          "/home/tusen/workspace/aeg-adaptive-autosar/ara-api/log/build/test",
          "", false, ara::log::internal::FileEncode::kBinary));
  ara::log::Initialize();
  ara::log::Initialize();
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
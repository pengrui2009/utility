/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-06-20 19:29:19
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-06-29 10:05:53
 * @FilePath: /aeg-adaptive-autosar/ara-api/log/test/common_test.cpp
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include <gtest/gtest.h>
#include "ara/log/logger.h"
#include "ara/log/logmanager.h"

TEST(COMMON, LogLevel) {
  EXPECT_EQ(0U, static_cast<uint8_t>(ara::log::LogLevel::kOff));
  EXPECT_EQ(1U, static_cast<uint8_t>(ara::log::LogLevel::kFatal));
  EXPECT_EQ(2U, static_cast<uint8_t>(ara::log::LogLevel::kError));
  EXPECT_EQ(3U, static_cast<uint8_t>(ara::log::LogLevel::kWarn));
  EXPECT_EQ(4U, static_cast<uint8_t>(ara::log::LogLevel::kInfo));
  EXPECT_EQ(5U, static_cast<uint8_t>(ara::log::LogLevel::kDebug));
  EXPECT_EQ(6U, static_cast<uint8_t>(ara::log::LogLevel::kVerbose));
}

TEST(COMMON, LogMode) {
  EXPECT_EQ(1U, static_cast<uint8_t>(ara::log::LogMode::kRemote));
  EXPECT_EQ(2U, static_cast<uint8_t>(ara::log::LogMode::kFile));
  EXPECT_EQ(4U, static_cast<uint8_t>(ara::log::LogMode::kConsole));
}

TEST(COMMON, MessageMode) {
  EXPECT_EQ(1U, static_cast<uint8_t>(ara::log::MessageMode::kModeled));
  EXPECT_EQ(2U, static_cast<uint8_t>(ara::log::MessageMode::kNonModeled));
}

TEST(COMMON, FileSaveMode) {
  EXPECT_EQ(1U, static_cast<uint8_t>(ara::log::internal::FileSaveMode::kSize));
  EXPECT_EQ(2U, static_cast<uint8_t>(ara::log::internal::FileSaveMode::kMinutely));
  EXPECT_EQ(3U, static_cast<uint8_t>(ara::log::internal::FileSaveMode::kHourly));
  EXPECT_EQ(4U, static_cast<uint8_t>(ara::log::internal::FileSaveMode::kDaily));
}

TEST(COMMON, FileEncode) {
  EXPECT_EQ(1U, static_cast<uint8_t>(ara::log::internal::FileEncode::kAscii));
  EXPECT_EQ(2U, static_cast<uint8_t>(ara::log::internal::FileEncode::kBinary));
}

TEST(COMMON, LogReturnValue) {
  EXPECT_EQ(-7, static_cast<int8_t>(ara::log::internal::LogReturnValue::kReturnLoggingDisabled));
  EXPECT_EQ(-6, static_cast<int8_t>(ara::log::internal::LogReturnValue::kReturnUserBufferFull));
  EXPECT_EQ(-5, static_cast<int8_t>(ara::log::internal::LogReturnValue::kReturnWrongParameter));
  EXPECT_EQ(-4, static_cast<int8_t>(ara::log::internal::LogReturnValue::kReturnBufferFull));
  EXPECT_EQ(-3, static_cast<int8_t>(ara::log::internal::LogReturnValue::kReturnPipeFull));
  EXPECT_EQ(-2, static_cast<int8_t>(ara::log::internal::LogReturnValue::kReturnPipeError));
  EXPECT_EQ(-1, static_cast<int8_t>(ara::log::internal::LogReturnValue::kReturnError));
  EXPECT_EQ(0, static_cast<int8_t>(ara::log::internal::LogReturnValue::kReturnOk));
  EXPECT_EQ(1, static_cast<int8_t>(ara::log::internal::LogReturnValue::kReturnTrue));
}

TEST(COMMON, operator) {
  auto log_mode1 = ara::log::LogMode::kConsole | ara::log::LogMode::kFile | ara::log::LogMode::kRemote;
  auto log_mode2 = ara::log::LogMode::kConsole | (ara::log::LogMode::kFile | ara::log::LogMode::kRemote);
  EXPECT_EQ(log_mode1, 7U);
  EXPECT_EQ(log_mode2, 7U);
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
/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-07-05 15:04:22
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-11-28 21:41:28
 * @FilePath: /aeg-adaptive-autosar/ara-api/log/test/func_test.cpp
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include <gtest/gtest.h>

#include "ara/log/logger.h"
#include "ara/log/logmanager.h"

void LevelOutput(ara::log::Logger &logger) {
  logger.LogFatal() << "should have fatal message";
  logger.WithLevel(ara::log::LogLevel::kFatal) << "should have fatal message";
  logger.LogError() << "should have error message";
  logger.WithLevel(ara::log::LogLevel::kError) << "should have error message";
  logger.LogWarn() << "should have warn message";
  logger.WithLevel(ara::log::LogLevel::kWarn) << "should have warn message";
  logger.LogInfo() << "should have info message";
  logger.WithLevel(ara::log::LogLevel::kInfo) << "should have info message";
  logger.LogDebug() << "should have debug message";
  logger.WithLevel(ara::log::LogLevel::kDebug) << "should have debug message";
  logger.LogVerbose() << "should have verbose message";
  logger.WithLevel(ara::log::LogLevel::kVerbose)
      << "should have verbose message";
}

TEST(OutputTest, CreateLogger) {
  auto &logger1 = ara::log::CreateLogger("dflg", "use default app logger");
  EXPECT_TRUE(logger1.IsEnabled(ara::log::LogLevel::kFatal));
  EXPECT_TRUE(logger1.IsEnabled(ara::log::LogLevel::kError));
  EXPECT_TRUE(logger1.IsEnabled(ara::log::LogLevel::kWarn));
  EXPECT_FALSE(logger1.IsEnabled(ara::log::LogLevel::kInfo));
  EXPECT_FALSE(logger1.IsEnabled(ara::log::LogLevel::kDebug));
  EXPECT_FALSE(logger1.IsEnabled(ara::log::LogLevel::kVerbose));

  auto &logger2 = ara::log::CreateLogger("delg", "use debug app logger",
                                         ara::log::LogLevel::kDebug);
  EXPECT_TRUE(logger2.IsEnabled(ara::log::LogLevel::kFatal));
  EXPECT_TRUE(logger2.IsEnabled(ara::log::LogLevel::kError));
  EXPECT_TRUE(logger2.IsEnabled(ara::log::LogLevel::kWarn));
  EXPECT_TRUE(logger2.IsEnabled(ara::log::LogLevel::kInfo));
  EXPECT_TRUE(logger2.IsEnabled(ara::log::LogLevel::kDebug));
  EXPECT_FALSE(logger2.IsEnabled(ara::log::LogLevel::kVerbose));

  auto &logger3 = ara::log::CreateLogger("inlg", "use info app logger",
                                         ara::log::LogLevel::kInfo);
  EXPECT_TRUE(logger3.IsEnabled(ara::log::LogLevel::kFatal));
  EXPECT_TRUE(logger3.IsEnabled(ara::log::LogLevel::kError));
  EXPECT_TRUE(logger3.IsEnabled(ara::log::LogLevel::kWarn));
  EXPECT_TRUE(logger3.IsEnabled(ara::log::LogLevel::kInfo));
  EXPECT_FALSE(logger3.IsEnabled(ara::log::LogLevel::kDebug));
  EXPECT_FALSE(logger3.IsEnabled(ara::log::LogLevel::kVerbose));

  auto &logger4 = ara::log::CreateLogger("erlg", "use error app logger",
                                         ara::log::LogLevel::kError);
  EXPECT_TRUE(logger4.IsEnabled(ara::log::LogLevel::kFatal));
  EXPECT_TRUE(logger4.IsEnabled(ara::log::LogLevel::kError));
  EXPECT_FALSE(logger4.IsEnabled(ara::log::LogLevel::kWarn));
  EXPECT_FALSE(logger4.IsEnabled(ara::log::LogLevel::kInfo));
  EXPECT_FALSE(logger4.IsEnabled(ara::log::LogLevel::kDebug));
  EXPECT_FALSE(logger4.IsEnabled(ara::log::LogLevel::kVerbose));

  ara::log::InitLogging("XXXX", "", ara::log::LogLevel::kInfo,
                        ara::log::LogMode::kConsole);
  /* after initialization */
  EXPECT_TRUE(logger1.IsEnabled(ara::log::LogLevel::kFatal));
  EXPECT_TRUE(logger1.IsEnabled(ara::log::LogLevel::kError));
  EXPECT_TRUE(logger1.IsEnabled(ara::log::LogLevel::kWarn));
  EXPECT_TRUE(logger1.IsEnabled(ara::log::LogLevel::kInfo));
  EXPECT_FALSE(logger1.IsEnabled(ara::log::LogLevel::kDebug));
  EXPECT_FALSE(logger1.IsEnabled(ara::log::LogLevel::kVerbose));

  EXPECT_TRUE(logger2.IsEnabled(ara::log::LogLevel::kFatal));
  EXPECT_TRUE(logger2.IsEnabled(ara::log::LogLevel::kError));
  EXPECT_TRUE(logger2.IsEnabled(ara::log::LogLevel::kWarn));
  EXPECT_TRUE(logger2.IsEnabled(ara::log::LogLevel::kInfo));
  EXPECT_TRUE(logger2.IsEnabled(ara::log::LogLevel::kDebug));
  EXPECT_FALSE(logger2.IsEnabled(ara::log::LogLevel::kVerbose));

  EXPECT_TRUE(logger3.IsEnabled(ara::log::LogLevel::kFatal));
  EXPECT_TRUE(logger3.IsEnabled(ara::log::LogLevel::kError));
  EXPECT_TRUE(logger3.IsEnabled(ara::log::LogLevel::kWarn));
  EXPECT_TRUE(logger3.IsEnabled(ara::log::LogLevel::kInfo));
  EXPECT_FALSE(logger3.IsEnabled(ara::log::LogLevel::kDebug));
  EXPECT_FALSE(logger3.IsEnabled(ara::log::LogLevel::kVerbose));

  EXPECT_TRUE(logger4.IsEnabled(ara::log::LogLevel::kFatal));
  EXPECT_TRUE(logger4.IsEnabled(ara::log::LogLevel::kError));
  EXPECT_FALSE(logger4.IsEnabled(ara::log::LogLevel::kWarn));
  EXPECT_FALSE(logger4.IsEnabled(ara::log::LogLevel::kInfo));
  EXPECT_FALSE(logger4.IsEnabled(ara::log::LogLevel::kDebug));
  EXPECT_FALSE(logger4.IsEnabled(ara::log::LogLevel::kVerbose));
}

TEST(OutputTest, Remote) {
  try {
    ara::log::Initialize();
    ara::core::StringView ctxid{"log"};
    ara::core::StringView ctxdesc{"log context"};
    ara::log::Logger &logger{
        ara::log::CreateLogger(ctxid, ctxdesc, ara::log::LogLevel::kVerbose)};

    LevelOutput(logger);
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
}

TEST(OutputTest, Level) {
  try {
    ara::core::StringView ctxid{"levl"};
    ara::core::StringView ctxdesc{"log context"};
    ara::log::Logger &logger{ara::log::CreateLogger(ctxid, ctxdesc)};

    EXPECT_TRUE(logger.IsEnabled(ara::log::LogLevel::kWarn));
    EXPECT_TRUE(logger.IsEnabled(ara::log::LogLevel::kError));
    EXPECT_TRUE(logger.IsEnabled(ara::log::LogLevel::kFatal));
    EXPECT_TRUE(logger.IsEnabled(ara::log::LogLevel::kInfo));
    EXPECT_FALSE(logger.IsEnabled(ara::log::LogLevel::kDebug));
    EXPECT_FALSE(logger.IsEnabled(ara::log::LogLevel::kVerbose));

  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
}

int main(int argc, char **argv) {
  try {
    ::testing::InitGoogleTest(&argc, argv);
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  } catch (...) {
  }
  return RUN_ALL_TESTS();
}
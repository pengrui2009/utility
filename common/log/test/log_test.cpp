/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-06-20 19:29:19
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-11-24 18:34:40
 * @FilePath: /adaptive_autosar/ara-api/log/test/log_test.cpp
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include <gtest/gtest.h>

#include <fstream>
#include <type_traits>

#include "ara/log/logger.h"
#include "ara/log/logmanager.h"

TEST(LOG, LogLevel) {
  ara::core::StringView ctxid = "leve";
  ara::core::StringView ctxdesc = "level test";
  ara::log::Logger& m_logger{
      ara::log::CreateLogger(ctxid, ctxdesc, ara::log::LogLevel::kVerbose)};
  auto stream_fatal = std::move(m_logger.LogFatal());
  EXPECT_NE(&stream_fatal, nullptr);
  auto with_stream_fatal =
      std::move(m_logger.WithLevel(ara::log::LogLevel::kFatal));
  EXPECT_NE(&with_stream_fatal, nullptr);

  auto stream_error = std::move(m_logger.LogError());
  EXPECT_NE(&stream_error, nullptr);
  auto with_stream_error =
      std::move(m_logger.WithLevel(ara::log::LogLevel::kError));
  EXPECT_NE(&with_stream_error, nullptr);

  auto stream_warn = std::move(m_logger.LogWarn());
  EXPECT_NE(&stream_warn, nullptr);
  auto with_stream_warn =
      std::move(m_logger.WithLevel(ara::log::LogLevel::kWarn));
  EXPECT_NE(&with_stream_warn, nullptr);

  auto stream_info = std::move(m_logger.LogInfo());
  EXPECT_NE(&stream_info, nullptr);
  auto with_stream_info =
      std::move(m_logger.WithLevel(ara::log::LogLevel::kInfo));
  EXPECT_NE(&with_stream_info, nullptr);

  auto stream_debug = std::move(m_logger.LogDebug());
  EXPECT_NE(&stream_debug, nullptr);
  auto with_stream_debug =
      std::move(m_logger.WithLevel(ara::log::LogLevel::kDebug));
  EXPECT_NE(&with_stream_debug, nullptr);

  auto stream_verbose = std::move(m_logger.LogVerbose());
  EXPECT_NE(&stream_verbose, nullptr);
  auto with_stream_verbose =
      std::move(m_logger.WithLevel(ara::log::LogLevel::kVerbose));
  EXPECT_NE(&with_stream_verbose, nullptr);
}

TEST(LOG, BaseTypes) {
  ara::core::StringView ctxid = "type";
  ara::core::StringView ctxdesc = "main log context";
  ara::log::Logger& m_logger{
      ara::log::CreateLogger(ctxid, ctxdesc, ara::log::LogLevel::kVerbose)};
  EXPECT_TRUE(m_logger.IsEnabled(ara::log::LogLevel::kDebug));
  constexpr uint8_t a1 = 100U;
  constexpr uint16_t a2 = 100U;
  constexpr uint32_t a3 = 100U;
  constexpr uint64_t a4 = 100UL;
  constexpr int8_t b1 = 100;
  constexpr int16_t b2 = -100;
  constexpr int32_t b3 = 100;
  constexpr int64_t b4 = -100;
  constexpr float f1 = 1.23F;
  constexpr double f2 = 3.14159;
  auto stream = std::move(m_logger.LogDebug());
  EXPECT_NE(&stream, nullptr);
  auto s1 = std::move(stream << a1);
  EXPECT_NE(&s1, nullptr);
  auto s2 = std::move(s1 << a2);
  EXPECT_NE(&s2, nullptr);
  auto s3 = std::move(s2 << a3);
  EXPECT_NE(&s3, nullptr);
  auto s4 = std::move(s3 << a4);
  EXPECT_NE(&s4, nullptr);
  auto s5 = std::move(s4 << b1);
  EXPECT_NE(&s5, nullptr);
  auto s6 = std::move(s5 << b2);
  EXPECT_NE(&s6, nullptr);
  auto s7 = std::move(s6 << b3);
  EXPECT_NE(&s7, nullptr);
  auto s8 = std::move(s7 << b4);
  EXPECT_NE(&s8, nullptr);
  auto s9 = std::move(s8 << f1);
  EXPECT_NE(&s9, nullptr);
  auto s10 = std::move(s9 << f2);
  EXPECT_NE(&s10, nullptr);
}

TEST(LOG, OtherTypes) {
  ara::core::StringView ctxid = "othr";
  ara::core::StringView ctxdesc = "main log context";
  ara::log::Logger& m_logger{
      ara::log::CreateLogger(ctxid, ctxdesc, ara::log::LogLevel::kVerbose)};
  constexpr uint8_t a1 = 100U;
  constexpr uint16_t a2 = 100U;
  constexpr uint32_t a3 = 100U;
  constexpr uint64_t a4 = 100UL;
  constexpr int8_t b1 = 100;
  constexpr int16_t b2 = -100;
  constexpr int32_t b3 = 100;
  constexpr int64_t b4 = -100;
  const ara::core::StringView sv = "string view";
  const std::string str = "hello tusimple";
  const ara::core::Span<ara::core::Byte> span("spantest", 8);
  // m_logger.LogInfo() << span;
  auto s1 =
      std::move(m_logger.LogDebug()
                << ara::log::BinFormat(a1) << ara::log::BinFormat(a2)
                << ara::log::BinFormat(a3) << ara::log::BinFormat(a4)
                << ara::log::BinFormat(b1) << ara::log::BinFormat(b2)
                << ara::log::BinFormat(b3) << ara::log::BinFormat(b4) << span);
  s1.Flush();
  EXPECT_NE(&s1, nullptr);
  auto s2 = std::move(s1 << ara::log::HexFormat(b1) << ara::log::HexFormat(b2)
                         << ara::log::HexFormat(b3) << ara::log::HexFormat(b4)
                         << ara::log::HexFormat(a1) << ara::log::HexFormat(a2)
                         << ara::log::HexFormat(a3) << ara::log::HexFormat(a4));
  s2.Flush();
  EXPECT_NE(&s2, nullptr);
  auto s3 = std::move(s2 << sv << str);
  s3.Flush();
  EXPECT_NE(&s3, nullptr);
}

void getState(ara::log::ClientState cs) {}
std::function<void(ara::log::ClientState)> cb = [](ara::log::ClientState cs) {};
TEST(LOG, Callback) {
  ara::log::RegisterConnectionStateHandler(getState);
  ara::log::RegisterConnectionStateHandler(cb);
}

int main(int argc, char** argv) {
  try {
    ::testing::InitGoogleTest(&argc, argv);
    ara::log::Initialize();
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  } catch (...) {
  }
  return RUN_ALL_TESTS();
}
/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2023-04-10 10:07:35
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-04-10 19:27:39
 * @Description:
 *
 * Copyright (c) 2023 by Tusimple, All Rights Reserved.
 */
#pragma once

#ifndef __QNX__
#include <features.h>
#if defined(__GLIBC__) && defined(__GLIBC_PREREQ) && __GLIBC_PREREQ(2, 30)
    #define _GNU_SOURCE
    #include <unistd.h>
#else
    #include <unistd.h>
    #include <sys/syscall.h>
    #define gettid() syscall(SYS_gettid)
#endif
#else
#include <process.h>
#endif

#include <ara/log/logger.h>
#include <fmt/format.h>

namespace ara {
namespace log {
namespace extend {

#define __APFILENAME__                                \
  (__builtin_strrchr(__builtin_FILE(), '/')           \
       ? __builtin_strrchr(__builtin_FILE(), '/') + 1 \
       : __builtin_FILE())

#define APLOG_FATA ara::log::LogLevel::kFatal
#define APLOG_ERROR ara::log::LogLevel::kError
#define APLOG_WARN ara::log::LogLevel::kWarn
#define APLOG_INFO ara::log::LogLevel::kInfo
#define APLOG_DEBUG ara::log::LogLevel::kDebug
#define APLOG_VERBOSE ara::log::LogLevel::kVerbose

#define APLOG_INITIALIZE() ara::log::Initialize()

#define APLOG_LOGGER_DEFAULT_LEVEL(log_inst, logger_name, logger_desc) \
  ara::log::extend::Aplog log_inst(logger_name, logger_desc);

#define APLOG_LOGGER_WITH_LEVEL(log_inst, logger_name, logger_desc, \
                                logger_level)                       \
  ara::log::extend::Aplog log_inst(logger_name, logger_desc, logger_level);

struct source_location {
  static constexpr source_location current(
      const char* __file = __APFILENAME__,
      int __line = __builtin_LINE()) noexcept {
    source_location __loc;
    __loc._M_file = __file;
    __loc._M_line = __line;
    return __loc;
  }

  constexpr source_location() noexcept : _M_file("unknown"), _M_line(0) {}
  constexpr uint32_t line() const noexcept { return _M_line; }
  constexpr const char* file_name() const noexcept { return _M_file; }

 private:
  const char* _M_file;
  uint32_t _M_line;
};

struct LogMessageInfo {
  const char* const format_;
  source_location location_;
  LogMessageInfo(const char* format,
                 source_location location = source_location::current())
      : format_(format), location_(location) {}
};

class Aplog {
 private:
  ara::log::Logger& inner_logger_;

 public:
  Aplog(const char* ctxid, const char* desc, const ara::log::LogLevel& level)
      : inner_logger_{ara::log::CreateLogger(ctxid, desc, level)} {};

  Aplog(const char* ctxid, const char* desc)
      : inner_logger_{ara::log::CreateLogger(ctxid, desc)} {};

  Aplog(const Aplog& rhs) = delete;
  Aplog(Aplog&& rhs) = delete;
  Aplog& operator=(const Aplog& rhs) = delete;
  Aplog& operator=(Aplog&& rhs) = delete;

  ~Aplog() = default;

  template <typename... Args>
  void Fatal(LogMessageInfo param, Args&&... args) {
    fmt::memory_buffer out;
    fmt::format_to(std::back_inserter(out), param.format_,
                   std::forward<Args>(args)...);
    inner_logger_.LogFatal().WithLocation(param.location_.file_name(),
                                          param.location_.line())
        << gettid() << ara::core::StringView(out.data(), out.size());
  }

  template <typename... Args>
  void Error(LogMessageInfo param, Args&&... args) {
    fmt::memory_buffer out;
    fmt::format_to(std::back_inserter(out), param.format_,
                   std::forward<Args>(args)...);
    inner_logger_.LogError().WithLocation(param.location_.file_name(),
                                          param.location_.line())
        << gettid() << ara::core::StringView(out.data(), out.size());
  }

  template <typename... Args>
  void Warn(LogMessageInfo param, Args&&... args) {
    fmt::memory_buffer out;
    fmt::format_to(std::back_inserter(out), param.format_,
                   std::forward<Args>(args)...);
    inner_logger_.LogWarn().WithLocation(param.location_.file_name(),
                                         param.location_.line())
        << gettid() << ara::core::StringView(out.data(), out.size());
  }

  template <typename... Args>
  void Info(LogMessageInfo param, Args&&... args) {
    fmt::memory_buffer out;
    fmt::format_to(std::back_inserter(out), param.format_,
                   std::forward<Args>(args)...);
    inner_logger_.LogInfo().WithLocation(param.location_.file_name(),
                                         param.location_.line())
        << gettid() << ara::core::StringView(out.data(), out.size());
  }

  template <typename... Args>
  void Debug(LogMessageInfo param, Args&&... args) {
    fmt::memory_buffer out;
    fmt::format_to(std::back_inserter(out), param.format_,
                   std::forward<Args>(args)...);

    inner_logger_.LogDebug().WithLocation(param.location_.file_name(),
                                          param.location_.line())
        << gettid() << ara::core::StringView(out.data(), out.size());
  }

  template <typename... Args>
  void Verbose(LogMessageInfo param, Args&&... args) {
    fmt::memory_buffer out;
    fmt::format_to(std::back_inserter(out), param.format_,
                   std::forward<Args>(args)...);
    inner_logger_.LogVerbose().WithLocation(param.location_.file_name(),
                                            param.location_.line())
        << gettid() << ara::core::StringView(out.data(), out.size());
  }
};

}  // namespace extend
}  // namespace log
}  // namespace ara

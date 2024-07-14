/**
 * @file     hal_log.h
 * @author   zhaobing.lv
 * @date     May 09, 2023
 */
#ifndef _HAL_LOG_H
#define _HAL_LOG_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/timeb.h>

#ifdef __cplusplus

#if __QNX__
#include <process.h>
#else
#include <sys/types.h>
#endif

#include <stdarg.h>
#include <string>
#include <libgen.h>
#include <fmt/format.h>
// #include <ara/log/logger.h>

extern "C"
{
#endif /* __cplusplus */

    static uint64_t getTimeStamp()
    {
        struct timeval time_now;
        gettimeofday(&time_now, NULL);
        uint64_t timestamp = 1000000L * time_now.tv_sec + time_now.tv_usec;
        return timestamp;
    }

    static char *log_time(void)
    {
        struct tm *ptm;
        struct timeb stTimeb;
        static char szTime[128];

        ftime(&stTimeb);
        ptm = localtime(&stTimeb.time);
        sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, stTimeb.millitm);
        szTime[23] = 0;
        return szTime;
    }

#define HAL_TIMESTAMP getTimeStamp

    static const char *log_levels[] = {"Off", "Fatal", "Error", "Warn", "Info", "Debug", "Verbose"};

#define HAL_LOG_LEVEL_OFF (0)
#define HAL_LOG_LEVEL_FATAL (1)
#define HAL_LOG_LEVEL_ERROR (2)
#define HAL_LOG_LEVEL_WARN (3)
#define HAL_LOG_LEVEL_INFO (4)
#define HAL_LOG_LEVEL_DEBUG (5)
#define HAL_LOG_LEVEL_VERBOSE (6)

#define HAL_PRINT(level, format, args...)                                                                                                                     \
    {                                                                                                                                                         \
        printf("%s [%s] %s:%d <%d> %lu " format "\n", log_time(), log_levels[level], basename((char *)__FILE__), __LINE__, gettid(), getTimeStamp(), ##args); \
    }

#ifdef __cplusplus
    extern void hal_log_init(void);
    extern void hal_log_init_level(ara::log::LogLevel level);

    extern ara::log::Logger *hal_logger;
#define HAL_LOG_FATAL() \
    hal_log_init();     \
    hal_logger->LogFatal() << basename((char *)__FILE__) << __LINE__ << gettid()
#define HAL_LOG_ERROR() \
    hal_log_init();     \
    hal_logger->LogError() << basename((char *)__FILE__) << __LINE__ << gettid()
#define HAL_LOG_WARN() \
    hal_log_init();    \
    hal_logger->LogWarn() << basename((char *)__FILE__) << __LINE__ << gettid()
#define HAL_LOG_INFO() \
    hal_log_init();    \
    hal_logger->LogInfo() << basename((char *)__FILE__) << __LINE__ << gettid()
#define HAL_LOG_DEBUG() \
    hal_log_init();     \
    hal_logger->LogDebug() << basename((char *)__FILE__) << __LINE__ << gettid()
#define HAL_LOG_VERBOSE() \
    hal_log_init();       \
    hal_logger->LogVerbose() << basename((char *)__FILE__) << __LINE__ << gettid()

#define HAL_LOG_FATAL_FMT(format, args...)                                \
    {                                                                     \
        fmt::memory_buffer out;                                           \
        fmt::format_to(std::back_inserter(out), format, ##args);          \
        HAL_LOG_FATAL() << ara::core::StringView(out.data(), out.size()); \
    }

#define HAL_LOG_ERROR_FMT(format, args...)                                \
    {                                                                     \
        fmt::memory_buffer out;                                           \
        fmt::format_to(std::back_inserter(out), format, ##args);          \
        HAL_LOG_ERROR() << ara::core::StringView(out.data(), out.size()); \
    }

#define HAL_LOG_WARN_FMT(format, args...)                                \
    {                                                                    \
        fmt::memory_buffer out;                                          \
        fmt::format_to(std::back_inserter(out), format, ##args);         \
        HAL_LOG_WARN() << ara::core::StringView(out.data(), out.size()); \
    }

#define HAL_LOG_INFO_FMT(format, args...)                                \
    {                                                                    \
        fmt::memory_buffer out;                                          \
        fmt::format_to(std::back_inserter(out), format, ##args);         \
        HAL_LOG_INFO() << ara::core::StringView(out.data(), out.size()); \
    }

#define HAL_LOG_DEBUG_FMT(format, args...)                                \
    {                                                                     \
        fmt::memory_buffer out;                                           \
        fmt::format_to(std::back_inserter(out), format, ##args);          \
        HAL_LOG_DEBUG() << ara::core::StringView(out.data(), out.size()); \
    }

#define HAL_LOG_VERBOSE_FMT(format, args...)                                \
    {                                                                       \
        fmt::memory_buffer out;                                             \
        fmt::format_to(std::back_inserter(out), format, ##args);            \
        HAL_LOG_VERBOSE() << ara::core::StringView(out.data(), out.size()); \
    }

#endif /* __cplusplus */

#define HAL_PRINT_FATAL(format, args...)                \
    {                                                   \
        HAL_PRINT(HAL_LOG_LEVEL_FATAL, format, ##args); \
    }

#define HAL_PRINT_ERROR(format, args...)                \
    {                                                   \
        HAL_PRINT(HAL_LOG_LEVEL_ERROR, format, ##args); \
    }

#define HAL_PRINT_WARN(format, args...)                \
    {                                                  \
        HAL_PRINT(HAL_LOG_LEVEL_WARN, format, ##args); \
    }

#define HAL_PRINT_INFO(format, args...)                \
    {                                                  \
        HAL_PRINT(HAL_LOG_LEVEL_INFO, format, ##args); \
    }

#define HAL_PRINT_DEBUG(format, args...)                \
    {                                                   \
        HAL_PRINT(HAL_LOG_LEVEL_DEBUG, format, ##args); \
    }

#define HAL_PRINT_VERBOSE(format, args...)                \
    {                                                     \
        HAL_PRINT(HAL_LOG_LEVEL_VERBOSE, format, ##args); \
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _HAL_LOG_H */

/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-05-31 18:13:03
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-11-24 18:50:16
 * @FilePath: /aeg-adaptive-autosar/ara-api/log/src/log_stream.cpp
 * @Description: logstream
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#include "ara/log/log_stream.h"
#include "ara/log/logger.h"

namespace ara
{
    namespace log
    {

        LogStream::LogStream(const LogLevel logLevel, Logger &logger) noexcept
            : logRet_(DltReturnValue::DLT_RETURN_OK), logLocalData_(std::make_unique<DltContextData>(DltContextData()))
        {
            DltLogLevelType log_level = DltLogLevelType::DLT_LOG_WARN;
            switch (logLevel)
            {
            case ara::log::LogLevel::kOff:
                log_level = DltLogLevelType::DLT_LOG_OFF;
                break;
            case ara::log::LogLevel::kFatal:
                log_level = DltLogLevelType::DLT_LOG_FATAL;
                break;
            case ara::log::LogLevel::kError:
                log_level = DltLogLevelType::DLT_LOG_ERROR;
                break;
            case ara::log::LogLevel::kWarn:
                log_level = DltLogLevelType::DLT_LOG_WARN;
                break;
            case ara::log::LogLevel::kInfo:
                log_level = DltLogLevelType::DLT_LOG_INFO;
                break;
            case ara::log::LogLevel::kDebug:
                log_level = DltLogLevelType::DLT_LOG_DEBUG;
                break;
            case ara::log::LogLevel::kVerbose:
                log_level = DltLogLevelType::DLT_LOG_VERBOSE;
                break;
            default:
                break;
            }
            if (logger.IsEnabled(logLevel))
            {
                logRet_ = dlt_user_log_write_start(static_cast<DltContext *>(logger.getContext()), logLocalData_.get(), log_level);
            }
            else
            {
                logRet_ = DltReturnValue::DLT_RETURN_LOGGING_DISABLED;
            }
        }

        LogStream::~LogStream()
        {
            if (logLocalData_ != nullptr)
            {
                if (logRet_ > DltReturnValue::DLT_RETURN_OK)
                {
                    if (logLocalData_->size == 0)
                    {
                        if (logLocalData_->buffer)
                        {
                            free(logLocalData_->buffer);
                            logLocalData_->buffer = NULL;
                        }
                    }
                    else
                    {
                        (void)dlt_user_log_write_finish(logLocalData_.get());
                    }
                }
            }
        }

        void LogStream::Flush() noexcept
        {
            if (logLocalData_ != nullptr)
            {
                if (logRet_ > DltReturnValue::DLT_RETURN_OK)
                {
                    if (logLocalData_->size == 0)
                    {
                        if (logLocalData_->buffer)
                        {
                            free(logLocalData_->buffer);
                            logLocalData_->buffer = NULL;
                        }
                    }
                    else
                    {
                        (void)dlt_user_log_write_finish(logLocalData_.get());
                    }
                }
                logRet_ = dlt_user_log_write_start(
                    logLocalData_->handle, logLocalData_.get(), logLocalData_->log_level);
            }
        }

        LogStream &LogStream::operator<<(bool value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_bool(logLocalData_.get(), value);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(uint8_t value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_uint8(logLocalData_.get(), value);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(uint16_t value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_uint16(logLocalData_.get(), value);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(uint32_t value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_uint32(logLocalData_.get(), value);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(uint64_t value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_uint64(logLocalData_.get(), value);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(int8_t value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_int8(logLocalData_.get(), value);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(int16_t value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_int16(logLocalData_.get(), value);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(int32_t value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_int32(logLocalData_.get(), value);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(int64_t value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_int64(logLocalData_.get(), value);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(float value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_float32(logLocalData_.get(), value);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(double value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_float64(logLocalData_.get(), value);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(const LogHex8 &value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_uint8_formatted(logLocalData_.get(), value.value, DLT_FORMAT_HEX8);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(const LogHex16 &value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_uint16_formatted(logLocalData_.get(), value.value, DLT_FORMAT_HEX16);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(const LogHex32 &value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_uint32_formatted(logLocalData_.get(), value.value, DLT_FORMAT_HEX32);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(const LogHex64 &value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_uint64_formatted(logLocalData_.get(), value.value, DLT_FORMAT_HEX64);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(const LogBin8 &value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_uint8_formatted(logLocalData_.get(), value.value, DLT_FORMAT_BIN8);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(const LogBin16 &value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_uint16_formatted(logLocalData_.get(), value.value, DLT_FORMAT_BIN16);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(const LogBin32 &value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_uint32_formatted(logLocalData_.get(), value.value, DLT_FORMAT_BIN32);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(const LogBin64 &value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_uint64_formatted(logLocalData_.get(), value.value, DLT_FORMAT_BIN64);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(const ara::core::StringView value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_utf8_string(logLocalData_.get(), ara::core::String(value).c_str());
            }
            return *this;
        }

        LogStream &LogStream::operator<<(const char *const value) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_utf8_string(logLocalData_.get(), value);
            }
            return *this;
        }

        LogStream &LogStream::operator<<(
            ara::core::Span<const ara::core::Byte> data) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_raw_formatted(
                    logLocalData_.get(),
                    static_cast<void *>(const_cast<ara::core::Byte *>(data.data())),
                    static_cast<uint16_t>(data.size()), DLT_FORMAT_DEFAULT);
            }
            return *this;
        }

        LogStream &LogStream::LogArgument(const ara::log::Argument<ara::core::String> &arg) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_string_attr(logLocalData_.get(), arg.arg_value.c_str(), arg.arg_name.c_str());
            }
            return *this;
        }

        LogStream &LogStream::LogArgument(const ara::log::Argument<bool> &arg) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_bool_attr(logLocalData_.get(), arg.arg_value, arg.arg_name.c_str());
            }
            return *this;
        }

        LogStream &LogStream::LogArgument(const ara::log::Argument<uint8_t> &arg) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_uint8_attr(logLocalData_.get(), arg.arg_value, arg.arg_name.c_str(), arg.arg_unit.c_str());
            }
            return *this;
        }

        LogStream &LogStream::LogArgument(const ara::log::Argument<uint16_t> &arg) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_uint16_attr(
                    logLocalData_.get(), arg.arg_value, arg.arg_name.c_str(), arg.arg_unit.c_str());
            }
            return *this;
        }

        LogStream &LogStream::LogArgument(const ara::log::Argument<uint32_t> &arg) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_uint32_attr(
                    logLocalData_.get(), arg.arg_value, arg.arg_name.c_str(), arg.arg_unit.c_str());
            }
            return *this;
        }

        LogStream &LogStream::LogArgument(const ara::log::Argument<uint64_t> &arg) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_uint64_attr(
                    logLocalData_.get(), arg.arg_value, arg.arg_name.c_str(), arg.arg_unit.c_str());
            }
            return *this;
        }

        LogStream &LogStream::LogArgument(const ara::log::Argument<int8_t> &arg) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_int8_attr(logLocalData_.get(), arg.arg_value, arg.arg_name.c_str(), arg.arg_unit.c_str());
            }
            return *this;
        }

        LogStream &LogStream::LogArgument(const ara::log::Argument<int16_t> &arg) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_int16_attr(logLocalData_.get(), arg.arg_value, arg.arg_name.c_str(), arg.arg_unit.c_str());
            }
            return *this;
        }

        LogStream &LogStream::LogArgument(const ara::log::Argument<int32_t> &arg) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_int32_attr(logLocalData_.get(), arg.arg_value, arg.arg_name.c_str(), arg.arg_unit.c_str());
            }
            return *this;
        }

        LogStream &LogStream::LogArgument(const ara::log::Argument<int64_t> &arg) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_int64_attr(logLocalData_.get(), arg.arg_value, arg.arg_name.c_str(), arg.arg_unit.c_str());
            }
            return *this;
        }

        LogStream &LogStream::LogArgument(const ara::log::Argument<float32_t> &arg) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_float32_attr(
                    logLocalData_.get(), arg.arg_value, arg.arg_name.c_str(), arg.arg_unit.c_str());
            }
            return *this;
        }

        LogStream &LogStream::LogArgument(const ara::log::Argument<float64_t> &arg) noexcept
        {
            if (logRet_ > DltReturnValue::DLT_RETURN_OK)
            {
                (void)dlt_user_log_write_float64_attr(
                    logLocalData_.get(), arg.arg_value, arg.arg_name.c_str(), arg.arg_unit.c_str());
            }
            return *this;
        }

        LogStream &LogStream::WithLocation(ara::core::StringView file, int line) noexcept
        {
            if (logRet_ < DltReturnValue::DLT_RETURN_OK || logLocalData_->size > 0)
            {
                return *this;
            }
            return *this << ara::log::Argument<int32_t>{line, file.data()};
        }

        LogStream &operator<<(LogStream &out, LogMode logMode) noexcept
        {
            if (logMode == ara::log::LogMode::kConsole)
            {
                return out << "kConsole";
            }
            else if (logMode == ara::log::LogMode::kFile)
            {
                return out << "kFile";
            }
            else
            {
                return out << "kRemote";
            }
        }

        LogStream &operator<<(LogStream &out, MessageMode messageMode) noexcept
        {
            if (messageMode == ara::log::MessageMode::kModeled)
            {
                return out << "kModeled";
            }
            else
            {
                return out << "kNonModeled";
            }
        }

        LogStream &operator<<(LogStream &out, ara::log::internal::FileEncode fileEncode) noexcept
        {
            if (fileEncode == ara::log::internal::FileEncode::kAscii)
            {
                return out << "kAscii";
            }
            else
            {
                return out << "kBinary";
            }
        }

        LogStream &operator<<(LogStream &out, ara::log::internal::FileSaveMode fileSaveMode) noexcept
        {
            if (fileSaveMode == ara::log::internal::FileSaveMode::kSize)
            {
                return out << "kSize";
            }
            else if (fileSaveMode == ara::log::internal::FileSaveMode::kMinutely)
            {
                return out << "kMinutely";
            }
            else if (fileSaveMode == ara::log::internal::FileSaveMode::kHourly)
            {
                return out << "kHourly";
            }
            else if (fileSaveMode == ara::log::internal::FileSaveMode::kDaily)
            {
                return out << "kDaily";
            }
            else
            {
                return out;
            }
        }

        LogStream &operator<<(LogStream &out, const ara::core::ErrorCode &ec) noexcept
        {
            return out << ara::core::String(ec.Domain().Name()) + ":" + std::to_string(ec.Value());
        }

        /* Support for extra types that transforms into native types */
        LogStream &operator<<(LogStream &out, LogLevel value) noexcept
        {
            switch (value)
            {
            case LogLevel::kOff:
                return (out << "Off");
                break;
            case LogLevel::kFatal:
                return (out << "Fatal");
                break;
            case LogLevel::kError:
                return (out << "Error");
                break;
            case LogLevel::kWarn:
                return (out << "Warn");
                break;
            case LogLevel::kInfo:
                return (out << "Info");
                break;
            case LogLevel::kDebug:
                return (out << "Debug");
                break;
            case LogLevel::kVerbose:
                return (out << "Verbose");
                break;
            default:
                break;
            }
            return (out << static_cast<typename std::underlying_type<LogLevel>::type>(value));
        }

    } // namespace log
} // namespace ara

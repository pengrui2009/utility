/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-05-31 17:28:40
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-08-08 12:20:45
 * @FilePath: /aeg-adaptive-autosar/ara-api/log/include/public/ara/log/log_stream.h
 * @Description: This file contains the stream based message handler.
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#ifndef AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_LOG_LOGSTREAM_H
#define AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_LOG_LOGSTREAM_H

#include <ara/core/error_code.h>
#include <ara/core/span.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include "ara/log/common.h"
#include "dlt/dlt_user.h"

#include <chrono>
#include <sstream>
#include <string>
#include <type_traits>

namespace ara
{
    namespace log
    {

        /*forward declare Logger class*/
        class Logger;

        /**
 * Helper struct that is utilized as custom type.
 * Holds an integer value that will be logged with a special format.
 * @uptrace{SWS_LOG_00108}
 */
        struct LogHex8
        {
            uint8_t value;
        };
        /**
 * @uptrace{SWS_LOG_00109}
 */
        struct LogHex16
        {
            uint16_t value;
        };
        /**
 * @uptrace{SWS_LOG_00110}
 */
        struct LogHex32
        {
            uint32_t value;
        };
        /**
 * @uptrace{SWS_LOG_00111}
 */
        struct LogHex64
        {
            uint64_t value;
        };
        /**
 * @uptrace{SWS_LOG_00112}
 */
        struct LogBin8
        {
            uint8_t value;
        };
        /**
 * @uptrace{SWS_LOG_00113}
 */
        struct LogBin16
        {
            uint16_t value;
        };
        /**
 * @uptrace{SWS_LOG_00114}
 */
        struct LogBin32
        {
            uint32_t value;
        };
        /**
 * @uptrace{SWS_LOG_00115}
 */
        struct LogBin64
        {
            uint64_t value;
        };

        template <typename T>
        class Argument final
        {
        public:
            T arg_value;
            ara::core::String arg_name;
            ara::core::String arg_unit;
            Argument() {}
            Argument(const T &value, const ara::core::String &name, const ara::core::String &unit)
                : arg_value(value), arg_name(name), arg_unit(unit) {}
            Argument(const T &value, const ara::core::String &name) : Argument(value, name, "") {}
            ~Argument() = default;

            Argument(const Argument<T> &argu) noexcept = default;
            Argument(Argument<T> &&argu) noexcept = default;
            Argument &operator=(const Argument<T> &argu) noexcept = default;
            Argument &operator=(Argument<T> &&argu) noexcept = default;
        };

        class LogStream final
        {
        public:
            LogStream() = delete;

            /**
   * @brief Creates the message stream object.
   *
   * Initiates it with the given log level directly on the back-end.
   *
   * @param[in] logLevel  The severity level of this message
   * @param[in] logger    The associated logger context
   */
            LogStream(const LogLevel logLevel, Logger &logger) noexcept;

            template <typename T = uint32_t>
            LogStream(const T &msgId, const LogLevel logLevel, Logger &logger) noexcept;

            /**
   * @brief Flushes the log message before destroying the stream object.
   *
   * Unless it was not explicitly flushed before, by calling @c Flush().
   */
            ~LogStream();

            /*
   * The big five.
   *
   * We actually don't want this class movable or copyable, but enforce RVO instead.
   * Since "Guaranteed copy elision" will be supported up from C++17, declaring the move ctor
   * without definition is a workaround and is necessary because we have a custom dtor.
   * With the missing move ctor implementation, explicit calls of std::move() on objects
   * of this class, will be prevented by a compile error.
   */
            LogStream(const LogStream &log_stream) = delete;
            LogStream &operator=(const LogStream &log_stream) = delete;
            LogStream(LogStream &&log_stream) noexcept = default;
            LogStream &operator=(LogStream &&log_stream) = delete;

            /**
   * @brief Processes the current log buffer.
   *
   * And prepares the object for new message in one go. Further usage of the @c LogStream object
   * after calling this method will end up in a new message. Use this method in case you want to
   * build your message distributed over multiple code lines.
   *
   * @note Calling @c Flush() is only necessary if the @c LogStream object is going to be re-used within
   * the same scope. Otherwise, if the object goes out of scope (e.g. end of function block), than
   * flush operation will be anyway done internally by the @c dtor.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00039}
   */
            void Flush() noexcept;

            /// @name DLT primitive types
            /// Build-in support for DLT native types.
            /// @{
            /**
   * @brief Writes boolean parameter into message.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00040}
   */
            LogStream &operator<<(bool value) noexcept;
            /**
   * @brief Writes unsigned int 8 bit parameter into message.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00041}
   */
            LogStream &operator<<(uint8_t value) noexcept;
            /**
   * @brief Writes unsigned int 16 bit parameter into message.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00042}
   */
            LogStream &operator<<(uint16_t value) noexcept;
            /**
   * @brief Writes unsigned int 32 bit parameter into message.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00043}
   */
            LogStream &operator<<(uint32_t value) noexcept;
            /**
   * @brief Writes unsigned int 64 bit parameter into message.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00044}
   */
            LogStream &operator<<(uint64_t value) noexcept;
            /**
   * @brief Writes signed int 8 bit parameter into message.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00045}
   */
            LogStream &operator<<(int8_t value) noexcept;
            /**
   * @brief Writes signed int 16 bit parameter into message.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00046}
   * @uptrace{SWS_LOG_00052}
   */
            LogStream &operator<<(int16_t value) noexcept;
            /**
   * @brief Writes signed int 32 bit parameter into message.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00047}
   */
            LogStream &operator<<(int32_t value) noexcept;
            /**
   * @brief Writes signed int 64 bit parameter into message.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00048}
   */
            LogStream &operator<<(int64_t value) noexcept;
            /**
   * @brief Writes float 32 bit parameter into message.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00049}
   */
            LogStream &operator<<(float value) noexcept;
            /**
   * @brief Writes float 64 bit parameter into message.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00050}
   */
            LogStream &operator<<(double value) noexcept;

            /**
   * @brief Writes unsigned int parameter into message, formatted as hexadecimal 8 digits.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00053}
   */
            LogStream &operator<<(const LogHex8 &value) noexcept;

            /**
   * @brief Writes unsigned int parameter into message, formatted as hexadecimal 16 digits.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00054}
   */
            LogStream &operator<<(const LogHex16 &value) noexcept;

            /**
   * @brief Writes unsigned int parameter into message, formatted as hexadecimal 32 digits.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00055}
   */
            LogStream &operator<<(const LogHex32 &value) noexcept;

            /**
   * @brief Writes unsigned int parameter into message, formatted as hexadecimal 64 digits.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00056}
   */
            LogStream &operator<<(const LogHex64 &value) noexcept;

            /**
   * @brief Writes unsigned int parameter into message, formatted as binary 8 digits.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00057}
   */
            LogStream &operator<<(const LogBin8 &value) noexcept;

            /**
   *  @brief Writes unsigned int parameter into message, formatted as binary 16 digits.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00058}
   */
            LogStream &operator<<(const LogBin16 &value) noexcept;

            /**
   *  @brief Writes unsigned int parameter into message, formatted as binary 32 digits.
   *  Only for kConsole or kFile with ASCII, do not use in kRemote or kFile
   *  @uptrace{SWS_LOG_00002}
   *  @uptrace{SWS_LOG_00059}
   */
            LogStream &operator<<(const LogBin32 &value) noexcept;

            /**
   *  @brief Writes unsigned int parameter into message, formatted as binary 64 digits.
   *  Only for kConsole or kFile with ASCII, do not use in kRemote or kFile
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00060}
   */
            LogStream &operator<<(const LogBin64 &value) noexcept;

            /**
   *  @brief Writes ara::core::StringView into message.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00062}
   */
            LogStream &operator<<(const ara::core::StringView value) noexcept;

            /**
   *  @brief Writes null terminated UTF8 string into message. (NOT sPECIFIED. WILL BE REMOVED IN FUTURE!)
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00051}
   */
            LogStream &operator<<(const char *const value) noexcept;

            /**
   *  @brief Write a byte sequence into message.
   *
   * @author renwen.wang
   * @note output format:"48'65'ff'22"
   *
   * @uptrace{SWS_LOG_00128}
   */
            LogStream &operator<<(ara::core::Span<const ara::core::Byte> data) noexcept;

            /**
   * @brief Log an argument with attributes.
   * @uptrace{SWS_LOG_00128}
   */
            template <typename T>
            LogStream &operator<<(const ara::log::Argument<T> &arg) noexcept
            {
                return LogArgument(arg);
            }
            /**
   *  @brief Add source file location into the message.
   * @uptrace{SWS_LOG_00129}
   */
            LogStream &WithLocation(ara::core::StringView file, int line) noexcept;
            /// @}

        private:
            DltReturnValue logRet_;
            std::unique_ptr<DltContextData> logLocalData_;

            LogStream &LogArgument(const ara::log::Argument<ara::core::String> &arg) noexcept;

            LogStream &LogArgument(const ara::log::Argument<bool> &arg) noexcept;

            LogStream &LogArgument(const ara::log::Argument<uint8_t> &arg) noexcept;

            LogStream &LogArgument(const ara::log::Argument<uint16_t> &arg) noexcept;

            LogStream &LogArgument(const ara::log::Argument<uint32_t> &arg) noexcept;

            LogStream &LogArgument(const ara::log::Argument<uint64_t> &arg) noexcept;

            LogStream &LogArgument(const ara::log::Argument<int8_t> &arg) noexcept;

            LogStream &LogArgument(const ara::log::Argument<int16_t> &arg) noexcept;

            LogStream &LogArgument(const ara::log::Argument<int32_t> &arg) noexcept;

            LogStream &LogArgument(const ara::log::Argument<int64_t> &arg) noexcept;

            LogStream &LogArgument(const ara::log::Argument<float32_t> &arg) noexcept;

            LogStream &LogArgument(const ara::log::Argument<float64_t> &arg) noexcept;
        };

        // Support for LogStream objects as rvalue reference. Enables custom type <<operator overload as first call
        template <typename T>
        LogStream &operator<<(LogStream &&out, T &&value) noexcept
        {
            return out << std::forward<T>(value);
        }

        /**
 * @brief Writes LogLevel enum parameter as text into message.
 *
 * @param out
 * @param value
 * @return LogStream&
 * @uptrace{SWS_LOG_00063}
 */
        LogStream &operator<<(LogStream &out, LogLevel value) noexcept;

        /**
 * @brief Write a core::ErrorCode instance into the message.
 * @uptrace{SWS_LOG_00124}
 */
        LogStream &operator<<(LogStream &out, const ara::core::ErrorCode &ec) noexcept;

        /**
 * @brief Write a LogMode into the message.
 */
        LogStream &operator<<(LogStream &out, LogMode logMode) noexcept;

        /**
 * @brief Write a MessageMode into the message.
 */
        LogStream &operator<<(LogStream &out, MessageMode logMode) noexcept;

        LogStream &operator<<(LogStream &out, ara::log::internal::FileEncode fileEncode) noexcept;

        LogStream &operator<<(LogStream &out, ara::log::internal::FileSaveMode fileSaveMode) noexcept;

        /**
 * @brief Write a std::chrono::duration instance into the message.
 * @uptrace{SWS_LOG_00125}
 */
        template <typename Rep, typename Period>
        LogStream &operator<<(LogStream &out, const std::chrono::duration<Rep, Period> &value) noexcept
        {
            return out << Argument<Rep>(value.count(), "Duration");
        }

        template <typename Rep>
        LogStream &operator<<(LogStream &out, const std::chrono::duration<Rep, std::nano> &value) noexcept
        {
            return out << Argument<Rep>(value.count(), "Duration", "ns");
        }

        template <typename Rep>
        LogStream &operator<<(LogStream &out, const std::chrono::duration<Rep, std::micro> &value) noexcept
        {
            return out << Argument<Rep>(value.count(), "Duration", "us");
        }

        template <typename Rep>
        LogStream &operator<<(LogStream &out, const std::chrono::duration<Rep, std::milli> &value) noexcept
        {
            return out << Argument<Rep>(value.count(), "Duration", "ms");
        }

        template <typename Rep>
        LogStream &operator<<(LogStream &out, const std::chrono::duration<Rep, std::centi> &value) noexcept
        {
            constexpr uint8_t coefficient = 10U;
            return out << Argument<Rep>(value.count() * coefficient, "Duration", "ms");
        }

        template <typename Rep>
        LogStream &operator<<(LogStream &out, const std::chrono::duration<Rep, std::deci> &value) noexcept
        {
            constexpr uint8_t coefficient = 100U;
            return out << Argument<Rep>(value.count() * coefficient, "Duration", "ms");
        }

        template <typename Rep>
        LogStream &operator<<(LogStream &out, const std::chrono::duration<Rep, std::ratio<1>> &value) noexcept
        {
            return out << Argument<Rep>(value.count(), "Duration", "s");
        }

        /// @brief Writes std::string into message.
        inline LogStream &operator<<(LogStream &out, const std::string &value) noexcept { return out << value.c_str(); }

        /**
 * @brief Writes pointer address into message, formatted as hexadecimal.
 * @uptrace{SWS_LOG_00127}
 */
        inline LogStream &operator<<(LogStream &out, const void *const value) noexcept
        {
            /* uintptr_t is generic for both 32/64 bit system*/
            /* _const void* const value_ SHALL NOT be converted to uintptr_t by reinterupt_cast.
  So the following style is used. */
            std::ostringstream s1;
            (void)(s1 << value);
            return out << s1.str();
        }

    } // namespace log
} // namespace ara

#endif // AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_LOG_LOGSTREAM_H

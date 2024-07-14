/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-05-31 13:47:26
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-08-02 14:07:48
 * @FilePath: /aeg-adaptive-autosar/ara-api/log/include/public/ara/log/logger.h
 * @Description: This file is the main public header of the logging API
 * This file must be used ONLY for test.
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#ifndef AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_LOGGER_H
#define AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_LOGGER_H

#include <cstring>

#include "log_stream.h"

// #include <ara/core/result.h>
// #include <ara/core/string_view.h>

namespace ara
{
    namespace log
    {
        class Logger final
        {
        private:
            class Impl;
            std::unique_ptr<Impl> _pImpl;

        public:
            /**
   * @brief  @c Logger ctor
   *
   * Holds the DLT context which is registered in the DLT back-end.
   *
   * @warning Create @c Logger with ctor is forbiden. CreateLogger should be used to generate Logger.
   *
   * @param[in] ctxId             The _up to four-character_ ID
   * @param[in] ctxDescription    Some description
   * @param[in] use_app_default_level    Whether use the configuration app default level
   * @param[in] ctxDefLogLevel    The context's default log reporting level
   */
            Logger(const ara::core::StringView &ctxId,
                   const ara::core::StringView &ctxDescription,
                   const int8_t &use_app_default_level, LogLevel ctxDefLogLevel) noexcept;

            /**
   * @brief Deregister the DLT context from the DLT back-end.
   *
   * If not already done explicitly by the LogManager.
   */
            ~Logger() noexcept;

            Logger &operator=(const Logger &logger) = delete;
            Logger &operator=(Logger &&logger) = delete;
            Logger(const Logger &logger) = delete;
            Logger(Logger &&logger) = delete;

            /**
   * @brief Creates a @c LogStream object.
   *
   * Returned object will accept arguments via the _insert stream operator_ "@c <<".
   *
   * @note In the normal usage scenario, the object's life time of the created @c LogStream is scoped
   * within one statement (ends with @c ; after last passed argument). If one wants to extend the
   * @c LogStream object's life time, the object might be assigned to a named variable.
   * @see Refer to the @c LogStream class documentation for further details.
   *
   * @return  @c LogStream object of Fatal severity.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00008}
   * @uptrace{SWS_LOG_00064}
   */
            LogStream LogFatal() const noexcept;
            /**
   * @brief Same as @c Logger::LogFatal().
   * @return  @c LogStream object of Error severity.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00009}
   * @uptrace{SWS_LOG_00065}
   */
            LogStream LogError() const noexcept;
            /**
   * @brief Same as @c Logger::LogFatal().
   * @return  @c LogStream object of Warn severity.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00010}
   * @uptrace{SWS_LOG_00066}
   */
            LogStream LogWarn() const noexcept;
            /**
   * @brief Same as @c Logger::LogFatal().
   * @return  @c LogStream object of Info severity.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00011}
   * @uptrace{SWS_LOG_00067}
   */
            LogStream LogInfo() const noexcept;
            /**
   * @brief Same as @c Logger::LogFatal().
   * @return  @c LogStream object of Debug severity.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00012}
   * @uptrace{SWS_LOG_00068}
   */
            LogStream LogDebug() const noexcept;
            /**
   * @brief Same as @c Logger::LogFatal().
   * @return  @c LogStream object of Verbose severity.
   *
   * @uptrace{SWS_LOG_00002}
   * @uptrace{SWS_LOG_00013}
   * @uptrace{SWS_LOG_00069}
   */
            LogStream LogVerbose() const noexcept;
            /**
   * @brief Check current configured log reporting level.
   *
   * Applications may want to check the actual configured reporting log level of certain loggers
   * before doing log data preparation that is runtime intensive.
   *
   * @param[in] logLevel  The to be checked log level.
   * @return              True if desired log level satisfies the configured reporting level.
   *
   * @uptrace{SWS_LOG_00007}
   * @uptrace{SWS_LOG_00070}
   */
            bool IsEnabled(LogLevel logLevel) const noexcept;
            /**
   * @brief Hide direct call and unregister any relevant backend;
   *
   * This public call unregister the current active backend
   *
   * @note The only current supported backend is DLT, but the direct access and implementation
   * should be hidden from the class consumer.
   */
            void unregisterBackends() noexcept;
            /**
   * @brief Return the current active backend context
   *
   * @note The only current supported backend is DLT, but the direct access and implementation
   * should be hidden from the class consumer.
   */
            DltContext *getContext();

            /*
   * Returns the context ID.
   * @return  The _up to four-character_ ID
   */
            std::string getId() const noexcept;

            /**
   * @brief Create a LogStream with LogLevel
   *
   * @param logLevel logLevel
   * @return LogStream
   */
            LogStream WithLevel(LogLevel logLevel) const noexcept;
        };

        /*************Function API***************/

        /**
 *
 * @param[in] appId             application ID,max 4bytes
 * @param[in] appDescription    application description
 * @param[in] appDefLogLevel    application default loglevel
 * @param[in] logMode           logmode, default is kConsole
 * @param[in] messageMode       When set to kModeled,support for both kModeled and
 * kNonModeled,when set to kNonModeled,only support for kNonModeled. Default is kModeled
 * @param[in] fileModeDescription     File save details. Used in kFile mode.
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00003}
 * @uptrace{SWS_LOG_00004} 21-11 kConsole
 * @uptrace{SWS_LOG_00020}
 * @uptrace{LOG_CST_00004}
 */
        void InitLogging(
            const std::string &appId, const std::string &appDescription, const LogLevel appDefLogLevel, const uint8_t &logMode,
            const MessageMode messageMode = MessageMode::kModeled,
            const ara::log::internal::FileModeDescription &fileModeDescription =
                ara::log::internal::FileModeDescription("", "")) noexcept;

        void InitLogging(
            const std::string &appId, const std::string &appDescription, const LogLevel appDefLogLevel,
            const LogMode logMode = LogMode::kConsole, const MessageMode messageMode = MessageMode::kModeled,
            const ara::log::internal::FileModeDescription &fileModeDescription =
                ara::log::internal::FileModeDescription("", "")) noexcept;

        ara::core::Result<void> Initialize();
        /**
 * @brief Create and return @c Logger ,register context into dlt back-end.
 * Remove the defalut loglevel according to AA-561
 *
 * @param[in] ctxId             The _up to four-character_ ID
 * @param[in] ctxDescription    Some description
 * @param[in] ctxDefLogLevel    The context log level
 * @return                      return @c Logger
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00005}
 * @uptrace{SWS_LOG_00006}
 * @uptrace{SWS_LOG_00021}  21-11 kWarn
 */
        Logger &CreateLogger(ara::core::StringView ctxId,
                             ara::core::StringView ctxDescription,
                             LogLevel ctxDefLogLevel) noexcept;

        /**
 * @brief reate and return @c Logger ,register context into dlt back-end.
 * 
 * If no default level configured in manifest.json, kWarn will be used.
 *
 * This API is newly added according to AA-561.
 *
 * @param[in] ctxId             The _up to four-character_ ID
 * @param[in] ctxDescription    Some description
 * @return                      return @c Logger
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00005}
 * @uptrace{SWS_LOG_00006}
 *
 */
        Logger &CreateLogger(ara::core::StringView ctxId,
                             ara::core::StringView ctxDescription) noexcept;

        template <typename T>
        Argument<T> Arg(T &&arg, const char *name = nullptr, const char *unit = nullptr) noexcept
        {
            return Argument<T>{std::forward<T>(arg), name, unit};
        }

        void RegisterConnectionStateHandler(std::function<void(ClientState)> callback) noexcept;

        /**
 * @brief  Logs decimal numbers in hexadecimal format.
 *
 * Negatives are represented in 2's complement. The number of represented digits depends on the
 * overloaded parameter type length.
 *
 * @param[in] value Decimal number to be represented in 8 digits
 * @return          @c LogHex8 type that has a built-in stream handler
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00015}
 * @uptrace{SWS_LOG_00016}
 * @uptrace{SWS_LOG_00022}
 */
        constexpr LogHex8 HexFormat(uint8_t value) noexcept { return LogHex8{value}; }

        /**
 * @brief Same as @c HexFormat(uint8_t value).
 *
 * @param[in] value Decimal number to be represented in 8 digits
 * @return          @c LogHex16 type that has a built-in stream handler
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00015}
 * @uptrace{SWS_LOG_00016}
 * @uptrace{SWS_LOG_00023}
 */
        constexpr LogHex8 HexFormat(int8_t value) noexcept { return LogHex8{static_cast<uint8_t>(value)}; }

        /**
 * @brief Same as @c HexFormat(uint8_t value).
 *
 * @param[in] value Decimal number to be represented in 16 digits
 * @return          @c LogHex16 type that has a built-in stream handler
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00015}
 * @uptrace{SWS_LOG_00016}
 * @uptrace{SWS_LOG_00024}
 */
        constexpr LogHex16 HexFormat(uint16_t value) noexcept { return LogHex16{value}; }

        /**
 * @brief Same as @c HexFormat(uint8_t value).
 *
 * @param[in] value Decimal number to be represented in 16 digits
 * @return          @c LogHex16 type that has a built-in stream handler
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00015}
 * @uptrace{SWS_LOG_00016}
 * @uptrace{SWS_LOG_00025}
 */
        constexpr LogHex16 HexFormat(int16_t value) noexcept { return LogHex16{static_cast<uint16_t>(value)}; }

        /**
 * @brief Same as @c HexFormat(uint8_t value).
 *
 * @param[in] value Decimal number to be represented in 32 digits
 * @return          @c LogHex32 type that has a built-in stream handler
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00015}
 * @uptrace{SWS_LOG_00016}
 * @uptrace{SWS_LOG_00026}
 */
        constexpr LogHex32 HexFormat(uint32_t value) noexcept { return LogHex32{value}; }

        /**
 * @brief Same as @c HexFormat(uint8_t value).
 *
 * @param[in] value Decimal number to be represented in 32 digits
 * @return          @c LogHex32 type that has a built-in stream handler
 *
 * @uptrace{SWS_LOG_00015}
 * @uptrace{SWS_LOG_00016}
 * @uptrace{SWS_LOG_00027}
 */
        constexpr LogHex32 HexFormat(int32_t value) noexcept { return LogHex32{static_cast<uint32_t>(value)}; }

        /**
 * @brief Same as @c HexFormat(uint8_t value).
 *
 * @param[in] value Decimal number to be represented in 64 digits
 * @return          @c LogHex64 type that has a built-in stream handler
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00015}
 * @uptrace{SWS_LOG_00016}
 * @uptrace{SWS_LOG_00028}
 */
        constexpr LogHex64 HexFormat(uint64_t value) noexcept { return LogHex64{value}; }

        /**
 * @brief Same as @c HexFormat(uint8_t value).
 *
 * @param[in] value Decimal number to be represented in 64 digits
 * @return          @c LogHex64 type that has a built-in stream handler
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00015}
 * @uptrace{SWS_LOG_00016}
 * @uptrace{SWS_LOG_00029}
 */
        constexpr LogHex64 HexFormat(int64_t value) noexcept { return LogHex64{static_cast<uint64_t>(value)}; }

        /**
 * @brief Logs decimal numbers in binary format.
 *
 * Negatives are represented in 2's complement. The number of represented digits depends on the
 * overloaded parameter type length.
 *
 * @param[in] value Decimal number to be represented in 8 digits
 * @return          @c LogBin8 type that has a built-in stream handler
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00017}
 * @uptrace{SWS_LOG_00030}
 */
        constexpr LogBin8 BinFormat(uint8_t value) noexcept { return LogBin8{value}; }

        /**
 * @brief Same as @c BinFormat(uint8_t value).
 *
 * @param[in] value Decimal number to be represented in 8 digits
 * @return          @c LogBin8 type that has a built-in stream handler
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00017}
 * @uptrace{SWS_LOG_00031}
 */
        constexpr LogBin8 BinFormat(int8_t value) noexcept { return LogBin8{static_cast<uint8_t>(value)}; }

        /**
 * @brief Same as @c BinFormat(uint8_t value).
 *
 * @param[in] value Decimal number to be represented in 16 digits
 * @return          @c LogBin16 type that has a built-in stream handler
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00017}
 * @uptrace{SWS_LOG_00032}
 */
        constexpr LogBin16 BinFormat(uint16_t value) noexcept { return LogBin16{value}; }

        /**
 * @brief Same as @c BinFormat(uint8_t value).
 *
 * @param[in] value Decimal number to be represented in 16 digits
 * @return          @c LogBin16 type that has a built-in stream handler
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00017}
 * @uptrace{SWS_LOG_00033}
 */
        constexpr LogBin16 BinFormat(int16_t value) noexcept { return LogBin16{static_cast<uint16_t>(value)}; }

        /**
 * @brief Same as @c BinFormat(uint8_t value).
 *
 * @param[in] value Decimal number to be represented in 32 digits
 * @return          @c LogBin32 type that has a built-in stream handler
 *
 * @uptrace{SWS_LOG_00017}
 * @uptrace{SWS_LOG_00034}
 */
        constexpr LogBin32 BinFormat(uint32_t value) noexcept { return LogBin32{value}; }

        /**
 * @brief Same as @c BinFormat(uint8_t value).
 *
 * @param[in] value Decimal number to be represented in 32 digits
 * @return          @c LogBin32 type that has a built-in stream handler
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00017}
 * @uptrace{SWS_LOG_00035}
 */
        constexpr LogBin32 BinFormat(int32_t value) noexcept { return LogBin32{static_cast<uint32_t>(value)}; }

        /**
 * @brief Same as @c BinFormat(uint8_t value).
 *
 * @param[in] value Decimal number to be represented in 64 digits
 * @return          @c LogBin64 type that has a built-in stream handler
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00017}
 * @uptrace{SWS_LOG_00036}
 */
        constexpr LogBin64 BinFormat(uint64_t value) noexcept { return LogBin64{value}; }

        /**
 * @brief Same as @c BinFormat(uint8_t value).
 *
 * @param[in] value Decimal number to be represented in 64 digits
 * @return          @c LogBin64 type that has a built-in stream handler
 *
 * @uptrace{SWS_LOG_00002}
 * @uptrace{SWS_LOG_00017}
 * @uptrace{SWS_LOG_00037}
 */
        constexpr LogBin64 BinFormat(int64_t value) noexcept { return LogBin64{static_cast<uint64_t>(value)}; }

        /* -------- END of API -------- */
        /// @}

    } /* namespace log */
} /* namespace ara */

#endif // AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_LOGGER_H

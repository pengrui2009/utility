/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-06-01 10:50:32
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-11-16 11:27:40
 * @FilePath:
 * /aeg-adaptive-autosar/ara-api/log/include/private/ara/log/logmanager.h
 * @Description: class logmanager
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#ifndef AEG_ADAPTIVE_AUTOSAR_PRIVATE_ARA_LOG_LOGMANAGER_H_
#define AEG_ADAPTIVE_AUTOSAR_PRIVATE_ARA_LOG_LOGMANAGER_H_

#include <ara/core/string_view.h>

#include <atomic>
#include <mutex>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include "ara/log/common.h"
#include "ara/log/logger.h"
#include <nlohmann/json.hpp>

#define AP_LOG_CONFIG_FILE "AP_LOG_CONFIG_FILE"
#define AP_LOG_FILE_SAVE_MODE "AP_LOG_FILE_SAVE_MODE"
#define AP_LOG_MAX_FILES "AP_LOG_MAX_FILES"
#define AP_LOG_FILE_SIZE "AP_LOG_FILE_SIZE"
#define AP_LOG_CONFIG_DFT_FILE "/usr/local/etc/ap_log/Log_configure.json"

namespace ara {
namespace log {

class LogStream;

/**
 * Class holding the main logic of the logging API.
 * It handles the de-/registration of the application against the DLT back-end
 * and is responsible for the logger contexts object life time.
 * * @uptrace{SWS_LOG_00097}
 */
class LogManager final {
 public:
  /**
   * Creates a single instance of this class.
   * @param[in] appId         The _up to four-character_ ID
   * @param[in] description   Some description
   * @param[in] logLevel      The application's default log reporting level
   * @param[in] logMode       The log mode to be used
   * @param[in] modeDescription  The description for file mode
   * @return                  Reference to the LogManager singleton instance
   */
  static LogManager& instance() noexcept;

  /**
   * @brief load log configuration file
   *
   */
  void LoadConfigurationFileAndInitLogging() noexcept;

  /**
   * Creates a logger, representing a DLT context.
   *
   * Does internally track all created contexts and cares for their
   * deregistration. It also checks weather requested ID isn't created yet, and
   * if so it returns the already available logger as reference.
   *
   * Implementation is multi-thread safe via serialization.
   *
   * @param[in] ctxId             The _up to four-character_ ID
   * @param[in] ctxDescription    Some description
   * @param[in] use_app_default_level Whether use the configuration app_default_level
   * @param[in] ctxDefLogLevel    The specified context's reporting level
   * @return                      Reference to the logger object
   * @uptrace{SWS_LOG_00100}
   */
  Logger& createLogContext(ara::core::StringView ctxId,
                           const ara::core::StringView& ctxDescription,
                           const int8_t& use_app_default_level,
                           LogLevel ctxDefLogLevel) noexcept;
  /**
   * Creates a logger with the application default log level
   *
   *
   * Implementation is multi-thread safe via serialization.
   *
   * @param[in] ctxId             The _up to four-character_ ID
   * @param[in] ctxDescription    Some description
   * @return                      Reference to the logger object
   * @uptrace{SWS_LOG_00100}
   */
  Logger& createLogContext(ara::core::StringView ctxId,
                           const ara::core::StringView& ctxDescription) noexcept;

  /**
   * Sets the application ID, description, default log level and mode.
   * In case of an empty string for @param[in] ID, it will be set to "XXXX".
   *
   * @param[in] appId               The application ID of maximum four
   * characters
   * @param[in] description      The application description
   * @param[in] logLevel         The default application reporting log level
   * @param[in] logMode          The application log mode
   * @param[in] fileModeDescription
   */
  void LogManagerInitialize(const ara::core::StringView& appId,
                            const ara::core::StringView& description,
                            const LogLevel logLevel, const uint8_t logMode,
                            const MessageMode messageMode,
                            const ara::log::internal::FileModeDescription&
                                fileModeDescription) noexcept;

 private:
  /**
   * Fetches the connection state from the DLT back-end of a possibly available
   * remote client.
   *
   * @returns The current client state
   * @uptrace{SWS_LOG_00101}
   * @note doesn't need. remove by wrw depend on 21-11
   */
  // ClientState remoteClientState() const noexcept;

  std::mutex g_mutex_logContexts;
  std::mutex g_mutex_initialize;
  std::atomic<bool> is_initialized{false};
  std::unordered_map<std::string, std::unique_ptr<Logger>> g_logContexts;
  const ara::core::StringView internCtxId = "INTM";
  const ara::core::StringView internCtxDesc = "logging API internal context";
  ara::log::LogLevel default_app_level = ara::log::LogLevel::kWarn;
  std::unique_ptr<Logger> g_logINT;
  nlohmann::json LoadConfigurations() noexcept;

  LogManager();

  // Will unregister remaining log contexts and the application before shutting
  // down.
  ~LogManager();

  // Deleted copy/move & assignment operators due to this object's singleton
  // nature.
  LogManager(const LogManager& log_manager) = delete;
  LogManager(LogManager&& log_manager) = default;
  LogManager& operator=(const LogManager& log_manager) = delete;
  LogManager& operator=(LogManager&& log_manager) = delete;
};

}  // namespace log
}  // namespace ara

#endif  // LOGMANAGER_H

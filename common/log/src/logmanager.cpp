/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-06-01 10:52:46
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-11-16 11:28:49
 * @FilePath: /aeg-adaptive-autosar/ara-api/log/src/logmanager.cpp
 * @Description: logmanager
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#include "ara/log/logmanager.h"
#include "ara/log/utility.h"

#include <sys/stat.h>
#include <climits>
#include <fstream>
#include <nlohmann/json.hpp>

#include "ara/log/logger.h"

namespace ara {
namespace log {

void LogManager::LogManagerInitialize(
    const ara::core::StringView& appId, const ara::core::StringView& description, const LogLevel logLevel,
    const uint8_t logMode, const MessageMode messageMode,
    const ara::log::internal::FileModeDescription& fileModeDescription) noexcept {
  if (is_initialized) {
    g_logINT->LogInfo() << "Logging Framework has already been initialized.";
    return;
  }
  const ara::core::String id{appId.data(), appId.size()};
  const ara::core::String desc{description.data(), description.size()};

  DltReturnValue ret{DltReturnValue::DLT_RETURN_OK};
  uint8_t log_mode = logMode;
  DltLogLevelType log_level = DltLogLevelType::DLT_LOG_WARN;
  constexpr uint8_t lower = 1U;
  constexpr uint32_t exit_time_ms = 1000U;
  constexpr uint8_t upper = 7U;
  default_app_level = logLevel;

  if (!((logMode >= lower) && (logMode <= upper))) {
    log_mode = static_cast<uint8_t>(ara::log::LogMode::kConsole);
  }

  dlt_set_resend_timeout_atexit(exit_time_ms);

  if ((log_mode & static_cast<uint8_t>(LogMode::kRemote)) > 0U) {
    (void)dlt_enable_remote();
  } else {
    (void)dlt_disable_remote();
  }

  if ((log_mode & static_cast<uint8_t>(LogMode::kConsole)) > 0U) {
    ret = dlt_enable_local_print();
    if (ret < DltReturnValue::DLT_RETURN_OK) {
      g_logINT->LogWarn() << "Unable to setup console output in DLT back-end. Error-code:" << static_cast<uint8_t>(ret);
    } else {
      if (!((logMode >= lower) && (logMode <= upper))) {
        g_logINT->LogWarn() << "LogMode illegal. Set to default:kConsole. Illegal LogMode:" << logMode;
      }
      g_logINT->LogInfo() << "Enabled console output.";
    }
  }

  constexpr uint32_t r_shift = 3U;
  if ((log_mode & static_cast<uint8_t>(LogMode::kFile)) > 0U) {
    const ara::core::String path{fileModeDescription.getSavePath().c_str(), fileModeDescription.getSavePath().size()};
    /* all static casts are for static checks */
    if (path.empty()) {
      g_logINT->LogError() << "logging: [" << id << "] unable to init file log mode. Given path is empty.";
      ret = DltReturnValue::DLT_RETURN_ERROR;
    } else if (ara::log::internal::pathExist(path)) {
      struct stat st {};
      /* path exists and id folder and is writable */
      if (((access(path.c_str(), W_OK)) < 0) || (stat(path.c_str(), &st) != 0) ||
          ((st.st_mode & static_cast<uint32_t>(S_IFDIR)) == 0U)) {
        g_logINT->LogError() << "logging: [" << id
                             << "] unable to write file path. Given path is not an accessible directory: " << path;
        ret = DltReturnValue::DLT_RETURN_ERROR;
      } else {
        ret = DltReturnValue::DLT_RETURN_OK;
      }
    } else if (
        fileModeDescription.getCreateDirs() &&
        ara::log::internal::createDir(
            path, static_cast<uint32_t>(
                      static_cast<uint32_t>(S_IRUSR + S_IWUSR + S_IXUSR) + (static_cast<uint32_t>(S_IRUSR) >> r_shift) +
                      (static_cast<uint32_t>(S_IWUSR) >> r_shift) + (static_cast<uint32_t>(S_IXUSR) >> r_shift)))) {
      ret = DltReturnValue::DLT_RETURN_OK;
    } else {
      g_logINT->LogError() << "logging: [" << id << "] unable to write file. Given path is not created: " << path;
      ret = DltReturnValue::DLT_RETURN_ERROR;
    }
    if (ret == DltReturnValue::DLT_RETURN_OK) {
      ara::core::String file_name = fileModeDescription.getFileName();
      if (fileModeDescription.getFileEncode() == ara::log::internal::FileEncode::kAscii) {
        if (file_name.find(".txt") == std::string::npos) {
          /* for InitLogging() */
          file_name = fileModeDescription.getFileName() == "" ? path + "/" + id + ".txt"
                                                              : path + "/" + fileModeDescription.getFileName() + ".txt";
        } else {
          /* for configuration */
          file_name = fileModeDescription.getSavePath() + "/" + fileModeDescription.getFileName();
        }
      } else {
        if (file_name.find(".dlt") == std::string::npos) {
          /* for InitLogging() */
          file_name = fileModeDescription.getFileName() == "" ? path + "/" + id + ".dlt"
                                                              : path + "/" + fileModeDescription.getFileName() + ".dlt";
        } else {
          /* for configuration */
          file_name = fileModeDescription.getSavePath() + "/" + fileModeDescription.getFileName();
        }
      }
      ret = dlt_init_file(
          file_name.c_str(), static_cast<int8_t>(fileModeDescription.getFileEncode()),
          static_cast<int8_t>(fileModeDescription.getFileSaveMode()), fileModeDescription.getMaxFile());
      if (ret < DltReturnValue::DLT_RETURN_OK) {
        g_logINT->LogError() << "logging: [" << id << "] unable to init file log mode in DLT back-end!";
      } else {
        g_logINT->LogInfo() << "Init file success. Path:" << file_name;
      }
      if (fileModeDescription.getFileSaveMode() == ara::log::internal::FileSaveMode::kSize) {
        ret = dlt_set_filesize_max(fileModeDescription.getFileSize());
      }
    }
  }

  if (dlt_check_library_version(
          ara::core::String(_DLT_PACKAGE_MAJOR_VERSION).c_str(),
          ara::core::String(_DLT_PACKAGE_MINOR_VERSION).c_str()) < DltReturnValue::DLT_RETURN_OK) {
    g_logINT->LogError() << "logging: mismatch of DLT back-end version. Further behavior is undefined!";
  }

  ret = dlt_register_app(id.c_str(), desc.c_str());
  if (ret < DltReturnValue::DLT_RETURN_OK) {
    g_logINT->LogError() << "logging: [" << id << "] unable to register application to DLT back-end. Error-code: "
                         << static_cast<uint8_t>(ret);
  }

  switch (logLevel) {
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
  // set application wide default log level for all already registered contexts
  ret = dlt_set_application_ll_ts_limit(log_level, DLT_TRACE_STATUS_OFF);
  if (ret < DltReturnValue::DLT_RETURN_OK) {
    g_logINT->LogError() << "Unable to finalize init of DLT back-end. DLT daemon is probably not running.";
  } else {
    g_logINT->LogInfo() << "Init DLT back-end done, all known contexts set to default log level: " << logLevel;
  }

  if (messageMode == MessageMode::kModeled) {
    if (dlt_nonverbose_mode() < DltReturnValue::DLT_RETURN_OK) {
      g_logINT->LogError() << "Unable set non verbose mode";
    }
  } else {
    if (dlt_verbose_mode() < DltReturnValue::DLT_RETURN_OK) {
      g_logINT->LogError() << "Unable set verbose mode";
    }
  }
  (void)(is_initialized = true);
}

LogManager& LogManager::instance() noexcept {
  static LogManager inst{};
  return inst;
}

LogManager::LogManager()
    : g_logINT(std::make_unique<Logger>(internCtxId, internCtxDesc, 0,
                                        LogLevel::kVerbose)) {}

LogManager::~LogManager() {
  try {
    const std::lock_guard<std::mutex> guard(g_mutex_logContexts);
    for (const std::pair<const std::string, std::unique_ptr<Logger>>& context : g_logContexts) {
      context.second->unregisterBackends();
    }

  } catch (std::logic_error&) {
  }
  g_logINT->unregisterBackends();
  (void)dlt_unregister_app_flush_buffered_logs();
}

Logger& LogManager::createLogContext(
    ara::core::StringView ctxId,
    const ara::core::StringView& ctxDescription) noexcept {
  return createLogContext(ctxId, ctxDescription, 1, default_app_level);
}

Logger& LogManager::createLogContext(
    ara::core::StringView ctxId, const ara::core::StringView& ctxDescription,
    const int8_t& use_app_default_level, LogLevel ctxDefLogLevel) noexcept {
  if (ctxId.compare(internCtxId) == 0) {
    g_logINT->LogWarn() << "Tried to register an internally reserved context ID [" << ctxId
                        << "]. Requested ID is changed to [XXXX]. Correct this in your code!";

    ctxId = "XXXX";
  }

  try {
    const std::lock_guard<std::mutex> guard(g_mutex_logContexts);

    const std::unordered_map<std::string, std::unique_ptr<Logger>>::const_iterator ctxIter =
        g_logContexts.find({ctxId.data(), ctxId.size()});

    if (ctxIter == g_logContexts.cend()) {
      (void)g_logContexts.emplace(
          ctxId.data(),
          std::make_unique<Logger>(ctxId, ctxDescription, use_app_default_level,
                                   ctxDefLogLevel));
      (*g_logContexts[{ctxId.data(), ctxId.size()}]).LogDebug()
          << "local time base used";
      return *g_logContexts[{ctxId.data(), ctxId.size()}].get();
    } else {
      g_logINT->LogDebug() << "Requested an already registered context ID - returning: " << ctxId;
      return *ctxIter->second.get();
    }
  } catch (const std::system_error& e) {
    g_logINT->LogFatal() << "Caught system_error exception: " << e.what() << ", " << e.code().value();

    // We don't want the application to break, so returning the internal context as emergency fallback
    g_logINT->LogFatal() << "Something went wrong in trying to create a logger context for the ID " << ctxId
                         << ". As fallback the logging API internal context will be used.";
    return *g_logINT;
  } catch (const std::bad_alloc& e) {
    g_logINT->LogError() << "Caught bad_alloc exception: " << e.what();

    // We don't want the application to break, so returning the internal context as emergency fallback
    g_logINT->LogFatal() << "Something went wrong in trying to create a logger context for the ID " << ctxId
                         << ". As fallback the logging API internal context will be used.";
    return *g_logINT;
  }
}

nlohmann::json LogManager::LoadConfigurations() noexcept {
  const char* env = getenv(AP_LOG_CONFIG_FILE);
  if (env == NULL) {
    env = AP_LOG_CONFIG_DFT_FILE;
  }

  struct stat st {};
  /* path exists and id folder and is writable */
  if (((access(env, R_OK)) < 0) || (stat(env, &st) != 0) || ((st.st_mode & static_cast<uint32_t>(S_IFDIR)) != 0U)) {
    g_logINT->LogWarn() << "Path:" << env << "is not a readable configuration file.";
    return NULL;
  }
  std::ifstream ifs(env);
  if (ifs.is_open()) {
    g_logINT->LogInfo() << env << "path in using.";
    nlohmann::json json = nlohmann::json::parse(ifs, nullptr, false, true);
    return json;
  } else {
    g_logINT->LogWarn() << "Error path: " << env;
  }
  return NULL;
}

void LogManager::LoadConfigurationFileAndInitLogging() noexcept {
  ara::core::String app_id = "XXXX";
  ara::core::String app_desc = "";
  ara::log::LogLevel log_level = ara::log::LogLevel::kWarn;
  uint8_t log_mode = static_cast<uint8_t>(ara::log::LogMode::kConsole);
  ara::log::MessageMode message_mode = ara::log::MessageMode::kNonModeled;
  std::string file_path = "./";
  std::string file_name = ara::core::String(app_id);
  bool create_dir = false;
  ara::log::internal::FileEncode file_encode = ara::log::internal::FileEncode::kAscii;
  ara::log::internal::FileSaveMode file_save_mode = ara::log::internal::FileSaveMode::kSize;
  uint8_t max_files = 5u;
  uint32_t file_size = 67108864U;
  std::lock_guard<std::mutex> lock(g_mutex_initialize);
  if (!is_initialized) {
    const nlohmann::json& js = LoadConfigurations();
    try {
      if (js == NULL) {
        g_logINT->LogWarn() << "Log Configuration parses failure. All parameters are set to default.";
      } else {
        // nlohmann::detail::iter_impl<const nlohmann::json> it;
        nlohmann::json tmp;
        /* Parsing appId */
        if (!js.contains("log_app_id")) {
          g_logINT->LogWarn() << "Can not find appid. Set to default:" + app_id;
        } else {
          tmp = js.at("log_app_id");
          if (!tmp.is_string() || tmp.get<std::string>() == "") {
            g_logINT->LogWarn() << "appId is invalid. Set to default:" + app_id;
          } else {
            app_id = tmp.get<std::string>().c_str();
          }
        }
        /* Parsing appDesc */
        if (!js.contains("log_app_description")) {
          g_logINT->LogWarn() << "Can not find app description. Set to default:" + ara::core::String(app_desc);
        } else {
          tmp = js.at("log_app_description");
          if (!tmp.is_string()) {
            g_logINT->LogWarn() << "appDesc is invalid. Set to default:" + ara::core::String(app_desc);
          } else {
            app_desc = tmp.get<std::string>();
          }
        }
        /* Parsing logLevel */
        if (!js.contains("log_default_level")) {
          g_logINT->LogWarn() << "Can not find app default log level. Set to default:" << log_level;
        } else {
          tmp = js.at("log_default_level");
          if (!tmp.is_string() || tmp.get<std::string>() == "") {
            g_logINT->LogWarn() << "logLevel is invalid. Set to default:" << log_level;
          } else {
            std::string level = tmp.get<std::string>();
            std::transform(level.begin(), level.end(), level.begin(), ::tolower);
            if (level.find("off") != std::string::npos) {
              log_level = ara::log::LogLevel::kOff;
            } else if (level.find("fatal") != std::string::npos) {
              log_level = ara::log::LogLevel::kFatal;
            } else if (level.find("error") != std::string::npos) {
              log_level = ara::log::LogLevel::kError;
            } else if (level.find("warn") != std::string::npos) {
              log_level = ara::log::LogLevel::kWarn;
            } else if (level.find("info") != std::string::npos) {
              log_level = ara::log::LogLevel::kInfo;
            } else if (level.find("debug") != std::string::npos) {
              log_level = ara::log::LogLevel::kDebug;
            } else if (level.find("verbose") != std::string::npos) {
              log_level = ara::log::LogLevel::kVerbose;
            } else {
              g_logINT->LogWarn() << "Unknown logLevel. Set to default:" << log_level;
            }
          }
        }
        /* Parsing logMode */
        if (!js.contains("log_mode")) {
          log_mode = static_cast<uint8_t>(ara::log::LogMode::kConsole);
          g_logINT->LogWarn() << "Can not find app log mode. Set to default:" << ara::log::LogMode::kConsole;
        } else {
          tmp = js.at("log_mode");
          if (!tmp.is_array() || tmp.size() == 0U) {
            log_mode = static_cast<uint8_t>(ara::log::LogMode::kConsole);
            g_logINT->LogWarn() << "logMode is invalid. Set to default:" << ara::log::LogMode::kConsole;
          } else {
            log_mode = 0U;
            for (auto& mode : tmp) {
              if (mode.is_string()) {
                if (mode.get<std::string>() == "kRemote") {
                  log_mode = log_mode | ara::log::LogMode::kRemote;
                } else if (mode.get<std::string>() == "kConsole") {
                  log_mode = log_mode | ara::log::LogMode::kConsole;
                } else if (mode.get<std::string>() == "kFile") {
                  log_mode = log_mode | ara::log::LogMode::kFile;
                } else {
                  log_mode = static_cast<uint8_t>(ara::log::LogMode::kConsole);
                  g_logINT->LogWarn() << "Unknown logLevel:" << mode.get<std::string>()
                                      << "Set to default:" << ara::log::LogMode::kConsole;
                  break;
                }
              } else {
                log_mode = static_cast<uint8_t>(ara::log::LogMode::kConsole);
                g_logINT->LogWarn() << "logMode format error. Set to default:" << ara::log::LogMode::kConsole;
                break;
              }
            }
          }
        }
        /* Parsing messageMode */
        if (!js.contains("log_message_mode")) {
          g_logINT->LogWarn() << "Can not find app log message mode. Set to default:" << message_mode;
        } else {
          tmp = js.at("log_message_mode");
          if (!tmp.is_string() || tmp.get<std::string>() == "") {
            g_logINT->LogWarn() << "messageMode is invalid. Set to default:" << message_mode;
          } else {
            if (tmp.get<std::string>() == "kModeled") {
              message_mode = ara::log::MessageMode::kModeled;
            } else if (tmp.get<std::string>() == "kNonModeled") {
              message_mode = ara::log::MessageMode::kNonModeled;
            } else {
              g_logINT->LogWarn() << "Unknown messageMode:" << tmp.get<std::string>()
                                  << "Set to default : " << message_mode;
            }
          }
        }
        /* Parsing FileDescription */
        if ((log_mode & static_cast<uint8_t>(ara::log::LogMode::kFile)) ==
            static_cast<uint8_t>(ara::log::LogMode::kFile)) {
          /* Parsing log file path */
          if (!js.contains("log_file_path")) {
            g_logINT->LogWarn() << "Can not find app log file path. Set to default:" << file_path;
          } else {
            tmp = js.at("log_file_path");
            if (!tmp.is_string() || tmp.get<std::string>() == "") {
              g_logINT->LogWarn() << "filePath is invalid. Set to default:" << file_path;
            } else {
              file_path = tmp.get<std::string>();
            }
          }
          /* Parsing file name */
          auto patterns = ara::log::internal::substring(file_path, "/");
          if (patterns[patterns.size() - 1].find(".txt") != std::string::npos ||
              patterns[patterns.size() - 1].find(".dlt") != std::string::npos) {
            /* file path contains file name */
            file_name = patterns[patterns.size() - 1];
            /* update file path */
            auto id = file_path.find(file_name);
            if (id == 0) {
              file_path = "./";
            } else {
              file_path = file_path.substr(0, id);
            }
          } else {
            /* file path doesn't contain file name */
            file_name = app_id;
          }
          /* Parsing create dir, default true */
          create_dir = true;
          /* Parsing file encode, default ascii */
          if (file_name.find(".txt") != std::string::npos) {
            file_encode = ara::log::internal::FileEncode::kAscii;
          } else if (file_name.find(".dlt") != std::string::npos) {
            file_encode = ara::log::internal::FileEncode::kBinary;
          } else { /* use appid as file name */
            if (!js.contains("log_file_encode")) {
              g_logINT->LogDebug() << "app log file encode is not specified. Set to default:" << file_encode;
            } else {
              tmp = js.at("log_file_encode");
              if (!tmp.is_string() || tmp.get<std::string>() == "") {
                g_logINT->LogWarn() << "fileEncode is invalid. Set to default:" << file_encode;
              } else {
                if (tmp.get<std::string>() == "kAscii") {
                  file_encode = ara::log::internal::FileEncode::kAscii;
                } else if (tmp.get<std::string>() == "kBinary") {
                  file_encode = ara::log::internal::FileEncode::kBinary;
                } else {
                  g_logINT->LogWarn() << "Unknown file_encode:" << tmp.get<std::string>()
                                      << "Set to default : " << file_encode;
                }
              }
            }
            /* add sufix for filename */
            if (file_encode == ara::log::internal::FileEncode::kBinary) {
              file_name = file_name + ".dlt";
            } else {
              file_name = file_name + ".txt";
            }
          }
          /* Parsing file encode, default ascii */
          char* env = getenv(AP_LOG_FILE_SAVE_MODE);
          if (env == NULL) { /* use configure file */
            if (!js.contains("log_file_savemode")) {
              g_logINT->LogDebug() << "app log file savemode is not specified. Set to default:" << file_save_mode;
            } else {
              tmp = js.at("log_file_savemode");
              if (!tmp.is_string() || tmp.get<std::string>() == "") {
                g_logINT->LogWarn() << "fileSaveMode is invalid. Set to default:" << file_save_mode;
              } else {
                if (tmp.get<std::string>() == "kSize") {
                  file_save_mode = ara::log::internal::FileSaveMode::kSize;
                } else if (tmp.get<std::string>() == "kMinutely") {
                  file_save_mode = ara::log::internal::FileSaveMode::kMinutely;
                } else if (tmp.get<std::string>() == "kHourly") {
                  file_save_mode = ara::log::internal::FileSaveMode::kHourly;
                } else if (tmp.get<std::string>() == "kDaily") {
                  file_save_mode = ara::log::internal::FileSaveMode::kDaily;
                } else {
                  g_logINT->LogWarn() << "Unknown file_save_mode:" << tmp.get<std::string>()
                                      << "Set to default : " << file_save_mode;
                }
              }
            }
          } else { /* use environment */
            auto file_save_mode_s = ara::core::String(env);
            if (file_save_mode_s == "kSize") {
              file_save_mode = ara::log::internal::FileSaveMode::kSize;
            } else if (file_save_mode_s == "kMinutely") {
              file_save_mode = ara::log::internal::FileSaveMode::kMinutely;
            } else if (file_save_mode_s == "kHourly") {
              file_save_mode = ara::log::internal::FileSaveMode::kHourly;
            } else if (file_save_mode_s == "kDaily") {
              file_save_mode = ara::log::internal::FileSaveMode::kDaily;
            } else {
              g_logINT->LogWarn() << "Unknown file_save_mode:" << file_save_mode_s
                                  << "Set to default : " << file_save_mode;
            }
          }
          /* Parsing file max files */
          env = getenv(AP_LOG_MAX_FILES);
          if (env == NULL) { /* use configure file */
            if (!js.contains("log_file_maxfiles")) {
              g_logINT->LogDebug() << "app log file maxfiles is not specified. Set to default:" << max_files;
            } else {
              tmp = js.at("log_file_maxfiles");
              if (!tmp.is_number()) {
                g_logINT->LogWarn() << "maxFiles is invalid. Set to default:" << max_files;
              } else {
                max_files = tmp.get<uint8_t>();
              }
            }
          } else {
            try {
              max_files = std::stoi(env);
              if (max_files > UCHAR_MAX) {
                max_files = UCHAR_MAX;
                g_logINT->LogWarn() << "maxFiles is too big. Set to max:" << max_files;
              }
            } catch (const std::invalid_argument& e) {
              g_logINT->LogWarn() << "maxFiles is invalid. Set to default:" << max_files;
            }
          }
          /* Parsing file size */
          env = getenv(AP_LOG_FILE_SIZE);
          if (env == NULL) { /* use configure file */
            if (!js.contains("log_file_size")) {
              g_logINT->LogDebug() << "app log file size is not specified. Set to default:" << file_size;
            } else {
              tmp = js.at("log_file_size");
              if (!tmp.is_number()) {
                g_logINT->LogWarn() << "fileSize is invalid. Set to default:" << file_size;
              } else {
                file_size = tmp.get<uint32_t>();
              }
            }
          } else {
            try {
              file_size = std::stoi(env);
            } catch (const std::invalid_argument& e) {
              g_logINT->LogWarn() << "fileSize is invalid. Set to default:" << file_size;
            }
          }
        }
      }
    } catch (const std::exception& e) {
      g_logINT->LogError() << "Parse file error:" << e.what();
    }
    {
      LogStream ls = std::move(g_logINT->LogInfo());
      ls << "appId:[" << app_id << "], appDesc:[" << app_desc << "], messageMode:[" << message_mode << "], logLevel:["
         << log_level << "], logMode:[";
      if ((log_mode & static_cast<uint8_t>(ara::log::LogMode::kConsole)) ==
          static_cast<uint8_t>(ara::log::LogMode::kConsole)) {
        ls << "kConsole";
      }
      if ((log_mode & static_cast<uint8_t>(ara::log::LogMode::kRemote)) ==
          static_cast<uint8_t>(ara::log::LogMode::kRemote)) {
        ls << "kRemote";
      }
      if ((log_mode & static_cast<uint8_t>(ara::log::LogMode::kFile)) ==
          static_cast<uint8_t>(ara::log::LogMode::kFile)) {
        ls << "kFile"
           << "], filePath:[" << file_path << "], fileName:[" << file_name << "], createDir:[" << create_dir
           << "], fileEncode:[" << file_encode << "], fileSaveMode:[" << file_save_mode << "], maxFiles:[" << max_files
           << "], fileSize:[" << file_size;
      }
      ls << "]";
    }  // auto release ls
    LogManagerInitialize(
        app_id.c_str(), app_desc.c_str(), log_level, log_mode, message_mode,
        ara::log::internal::FileModeDescription(
            file_path, file_name, create_dir, file_encode, file_save_mode, max_files, file_size));
  } else {
    g_logINT->LogInfo() << "Logging Framework has already been initialized.";
    return;
  }
}

}  // namespace log
}  // namespace ara
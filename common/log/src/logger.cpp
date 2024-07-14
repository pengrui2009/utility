#include "ara/log/logger.h"
#include <atomic>

#include "ara/core/string.h"
#include "ara/log/logmanager.h"
#include <nlohmann/json.hpp>

namespace ara {
namespace log {

namespace {
// To prevent misuse of the API (calling functions on the LogManager::instance() without previous
// InitLogging() call on it), this variable must be checked in every function that calls LogManager::instance().
std::atomic<bool> g_isInitialized{false};
std::mutex init_mutex;
}  // namespace

class Logger::Impl final {
 public:
  Impl(Logger* logger_parent, const ara::core::StringView& ctxId,
       const ara::core::StringView& ctxDescription,
       const int8_t& use_app_default_level,
       const LogLevel ctxDefLogLevel) noexcept
      : parent(logger_parent), isRegistered_(false) {
    const ara::core::String id(ctxId.data(), ctxId.size());
    const ara::core::String desc(ctxDescription.data(), ctxDescription.size());

    DltReturnValue ret = dlt_register_context_ll_ts_use_default_level(
        dltContext_.get(), id.c_str(), desc.c_str(), use_app_default_level,
        static_cast<int32_t>(ctxDefLogLevel), DLT_TRACE_STATUS_OFF);

    if (ret < DltReturnValue::DLT_RETURN_OK) {
      const std::string err_msg = "logging: unable to register context [" + id +
                                  "] to DLT back-end. Error-code: " + std::to_string(static_cast<int8_t>(ret));
      DltContextData log_local;
      DltReturnValue dlt_local = dlt_user_log_write_start(dltContext_.get(), &log_local, DLT_LOG_ERROR);
      if (dlt_local == DLT_RETURN_TRUE) {
        (void)dlt_user_log_write_string(&log_local, err_msg.c_str());
        (void)dlt_user_log_write_finish(&log_local);
      }
    } else {
      (void)(isRegistered_ = true);
    }
  };

  Impl& operator=(Impl& impl) = delete;
  Impl(const Impl& impl) = delete;
  Impl& operator=(Impl&& impl) = delete;
  Impl(const Impl&& impl) = delete;

  ~Impl() { unregisterBackends(); }

  bool IsEnabled(LogLevel logLevel) const noexcept {
    DltLogLevelType log_level = DltLogLevelType::DLT_LOG_WARN;
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
    return dlt_user_is_logLevel_enabled(dltContext_.get(), log_level) == DLT_RETURN_TRUE;
  }

  LogStream LogOff() noexcept { return std::move(LogStream{LogLevel::kOff, *parent}); }

  LogStream LogFatal() noexcept { return std::move(LogStream{LogLevel::kFatal, *parent}); }

  LogStream LogError() noexcept { return std::move(LogStream{LogLevel::kError, *parent}); }

  LogStream LogWarn() noexcept { return std::move(LogStream{LogLevel::kWarn, *parent}); }

  LogStream LogInfo() noexcept { return std::move(LogStream{LogLevel::kInfo, *parent}); }

  LogStream LogDebug() noexcept { return std::move(LogStream{LogLevel::kDebug, *parent}); }

  LogStream LogVerbose() noexcept { return std::move(LogStream{LogLevel::kVerbose, *parent}); }

  void unregisterBackends() noexcept {
    // Single backend supported from now
    unregisterFromDlt();
  }

  void unregisterFromDlt() noexcept {
    if (isRegistered_) {
      if (!(isRegistered_ = false)) {
        (void)dlt_unregister_context(dltContext_.get());
      }
    }
  }

  std::string getId() const noexcept {
    // string ctor usage might not be no-throw guaranteed - implementation not to be used for series SW
    const char* ctx_id = dltContext_->contextID;
    return ara::core::String(ctx_id, static_cast<size_t>(DLT_ID_SIZE)).c_str();
  }

  DltContext* getContext() noexcept { return dltContext_.get(); }

 private:
  Logger* parent;
  std::atomic<bool> isRegistered_{false};
  std::shared_ptr<DltContext> dltContext_ = std::make_shared<DltContext>(DltContext());
  // DltContext dltContext_{};
};

Logger::Logger(const ara::core::StringView& ctxId,
               const ara::core::StringView& ctxDescription,
               const int8_t& use_app_default_level,
               LogLevel ctxDefLogLevel) noexcept
    : _pImpl(std::make_unique<Impl>(this, ctxId, ctxDescription,
                                    use_app_default_level, ctxDefLogLevel)) {}

Logger::~Logger() noexcept { unregisterBackends(); }

void Logger::unregisterBackends() noexcept { _pImpl->unregisterBackends(); }

std::string Logger::getId() const noexcept { return _pImpl->getId(); }

LogStream Logger::LogFatal() const noexcept { return _pImpl->LogFatal(); }

LogStream Logger::LogError() const noexcept { return _pImpl->LogError(); }

LogStream Logger::LogWarn() const noexcept { return _pImpl->LogWarn(); }

LogStream Logger::LogInfo() const noexcept { return _pImpl->LogInfo(); }

LogStream Logger::LogDebug() const noexcept { return _pImpl->LogDebug(); }

LogStream Logger::LogVerbose() const noexcept { return _pImpl->LogVerbose(); }

LogStream Logger::WithLevel(LogLevel logLevel) const noexcept {
  if (logLevel == LogLevel::kFatal) {
    return _pImpl->LogFatal();
  } else if (logLevel == LogLevel::kError) {
    return _pImpl->LogError();
  } else if (logLevel == LogLevel::kWarn) {
    return _pImpl->LogWarn();
  } else if (logLevel == LogLevel::kInfo) {
    return _pImpl->LogInfo();
  } else if (logLevel == LogLevel::kDebug) {
    return _pImpl->LogDebug();
  } else if (logLevel == LogLevel::kVerbose) {
    return _pImpl->LogVerbose();
  } else {
    return _pImpl->LogOff();  // logLevel = kOff
  }
}

bool Logger::IsEnabled(LogLevel logLevel) const noexcept { return _pImpl->IsEnabled(logLevel); }

DltContext* Logger::getContext() { return _pImpl->getContext(); }

/*************Function API***************/
void InitLogging(
    const std::string& appId, const std::string& appDescription, const LogLevel appDefLogLevel, const uint8_t& logMode,
    const MessageMode messageMode, const ara::log::internal::FileModeDescription& fileModeDescription) noexcept {
  std::lock_guard<std::mutex> lock(init_mutex);
  if (!g_isInitialized) {
    LogManager::instance().LogManagerInitialize(
        ara::core::StringView{appId.c_str(), appId.size()},
        ara::core::StringView{appDescription.c_str(), appDescription.size()}, appDefLogLevel, logMode, messageMode,
        fileModeDescription);
    (void)(g_isInitialized = true);
  }
}

void InitLogging(
    const std::string& appId, const std::string& appDescription, const LogLevel appDefLogLevel, const LogMode logMode,
    const MessageMode messageMode, const ara::log::internal::FileModeDescription& fileModeDescription) noexcept {
  std::lock_guard<std::mutex> lock(init_mutex);
  if (!g_isInitialized) {
    LogManager::instance().LogManagerInitialize(
        ara::core::StringView{appId.c_str(), appId.size()},
        ara::core::StringView{appDescription.c_str(), appDescription.size()}, appDefLogLevel,
        static_cast<uint8_t>(logMode), messageMode, fileModeDescription);
    (void)(g_isInitialized = true);
  }
}

ara::core::Result<void> Initialize() {
  LogManager::instance().LoadConfigurationFileAndInitLogging();
  return ara::core::Result<void>::FromValue();
}

Logger& CreateLogger(ara::core::StringView ctxId,
                     ara::core::StringView ctxDescription,
                     LogLevel ctxDefLogLevel) noexcept {
  return LogManager::instance().createLogContext(ctxId, ctxDescription, 0,
                                                 ctxDefLogLevel);
}

Logger& CreateLogger(ara::core::StringView ctxId,
                     ara::core::StringView ctxDescription) noexcept {
  return LogManager::instance().createLogContext(ctxId, ctxDescription);
}

void func1(int8_t cs, std::function<void(ara::log::ClientState)> cb) {
  ara::log::ClientState s;
  switch (cs) {
    case static_cast<int8_t>(ara::log::ClientState::kUnknown):
      s = ara::log::ClientState::kUnknown;
      break;
    case static_cast<int8_t>(ara::log::ClientState::kNotConnected):
      s = ara::log::ClientState::kNotConnected;
      break;
    case static_cast<int8_t>(ara::log::ClientState::kConnected):
      s = ara::log::ClientState::kConnected;
      break;
    default:
      s = ara::log::ClientState::kUnknown;
      break;
  }
  cb(s);
}

static std::function<void(ara::log::ClientState)> cb_func;

static void cb_helper(int8_t cs) {
  ara::log::ClientState s;
  switch (cs) {
    case static_cast<int8_t>(ara::log::ClientState::kUnknown):
      s = ara::log::ClientState::kUnknown;
      break;
    case static_cast<int8_t>(ara::log::ClientState::kNotConnected):
      s = ara::log::ClientState::kNotConnected;
      break;
    case static_cast<int8_t>(ara::log::ClientState::kConnected):
      s = ara::log::ClientState::kConnected;
      break;
    default:
      s = ara::log::ClientState::kUnknown;
      break;
  }
  cb_func(s);
}

void RegisterConnectionStateHandler(std::function<void(ara::log::ClientState)> callback) noexcept {
  cb_func = callback;
  dlt_register_client_state_callback(cb_helper);
}

static_assert(
    static_cast<DltLogLevelType>(LogLevel::kOff) == DLT_LOG_OFF,
    "LogLevel::kOff does not match DltLogLevelType::DLT_LOG_OFF");
static_assert(
    static_cast<DltLogLevelType>(LogLevel::kFatal) == DLT_LOG_FATAL,
    "LogLevel::kFatal does not match DltLogLevelType::DLT_LOG_FATAL");
static_assert(
    static_cast<DltLogLevelType>(LogLevel::kError) == DLT_LOG_ERROR,
    "LogLevel::kError does not match DltLogLevelType::DLT_LOG_ERROR");
static_assert(
    static_cast<DltLogLevelType>(LogLevel::kWarn) == DLT_LOG_WARN,
    "LogLevel::kWarn does not match DltLogLevelType::DLT_LOG_WARN");
static_assert(
    static_cast<DltLogLevelType>(LogLevel::kInfo) == DLT_LOG_INFO,
    "LogLevel::kInfo does not match DltLogLevelType::DLT_LOG_INFO");
static_assert(
    static_cast<DltLogLevelType>(LogLevel::kDebug) == DLT_LOG_DEBUG,
    "LogLevel::kDebug does not match DltLogLevelType::DLT_LOG_DEBUG");
static_assert(
    static_cast<DltLogLevelType>(LogLevel::kVerbose) == DLT_LOG_VERBOSE,
    "LogLevel::kVerbose does not match DltLogLevelType::DLT_LOG_VERBOSE");

static_assert(
    static_cast<DltReturnValue>(internal::LogReturnValue::kReturnLoggingDisabled) == DLT_RETURN_LOGGING_DISABLED,
    "internal::LogReturnValue::kReturnLoggingDisabled does not match DltReturnValue::DLT_RETURN_LOGGING_DISABLED");
static_assert(
    static_cast<DltReturnValue>(internal::LogReturnValue::kReturnUserBufferFull) == DLT_RETURN_USER_BUFFER_FULL,
    "internal::LogReturnValue::kReturnUserBufferFull does not match DltReturnValue::DLT_RETURN_USER_BUFFER_FULL");
static_assert(
    static_cast<DltReturnValue>(internal::LogReturnValue::kReturnWrongParameter) == DLT_RETURN_WRONG_PARAMETER,
    "internal::LogReturnValue::kReturnWrongParameter does not match DltReturnValue::DLT_RETURN_WRONG_PARAMETER");
static_assert(
    static_cast<DltReturnValue>(internal::LogReturnValue::kReturnBufferFull) == DLT_RETURN_BUFFER_FULL,
    "internal::LogReturnValue::kReturnBufferFull does not match DltReturnValue::DLT_RETURN_BUFFER_FULL");
static_assert(
    static_cast<DltReturnValue>(internal::LogReturnValue::kReturnPipeFull) == DLT_RETURN_PIPE_FULL,
    "internal::LogReturnValue::kReturnPipeFUll does not match DltReturnValue::DLT_RETURN_PIPE_FULL");
static_assert(
    static_cast<DltReturnValue>(internal::LogReturnValue::kReturnPipeError) == DLT_RETURN_PIPE_ERROR,
    "internal::LogReturnValue::kReturnPipeError does not match DltReturnValue::DLT_RETURN_PIPE_ERROR");
static_assert(
    static_cast<DltReturnValue>(internal::LogReturnValue::kReturnError) == DLT_RETURN_ERROR,
    "internal::LogReturnValue::kReturnError does not match DltReturnValue::DLT_RETURN_ERROR");
static_assert(
    static_cast<DltReturnValue>(internal::LogReturnValue::kReturnOk) == DLT_RETURN_OK,
    "internal::LogReturnValue::kReturnOk does not match DltReturnValue::DLT_RETURN_OK");
static_assert(
    static_cast<DltReturnValue>(internal::LogReturnValue::kReturnTrue) == DLT_RETURN_TRUE,
    "internal::LogReturnValue::kReturnTrue does not match DltReturnValue::DLT_RETURN_TRUE");

}  // namespace log
}  // namespace ara

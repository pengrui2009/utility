#ifndef ARA_FILESYSTEM_FILESYSTEM_LOGGER_H_
#define ARA_FILESYSTEM_FILESYSTEM_LOGGER_H_

#include <ara/log/logger.h>

namespace ara {
namespace filesystem {

class FileSystemLogger {
 public:
  

  FileSystemLogger(const FileSystemLogger& other) = delete;
  FileSystemLogger& operator=(const FileSystemLogger& other) = delete;

  FileSystemLogger(FileSystemLogger&& other) = delete;
  FileSystemLogger& operator=(FileSystemLogger&& other) = delete;

  ~FileSystemLogger() = default;

  static FileSystemLogger& GetInstance() {
    static FileSystemLogger doip_logger;
    return doip_logger;
  }
  ara::log::Logger& logger() { return logger_; }

 private:
  FileSystemLogger() : logger_(ara::log::CreateLogger("DoIP", "")) {}
  
  ara::log::Logger& logger_;
};

inline auto LOG_VERBOSE() {
  return FileSystemLogger::GetInstance().logger().LogVerbose();
}

inline auto LOG_DEBUG() {
  return FileSystemLogger::GetInstance().logger().LogDebug();
}

inline auto LOG_INFO() {
  return FileSystemLogger::GetInstance().logger().LogInfo();
}

inline auto LOG_WARN() {
  return FileSystemLogger::GetInstance().logger().LogWarn();
}

inline auto LOG_ERROR() {
  return FileSystemLogger::GetInstance().logger().LogError();
}

inline auto LOG_FATAL() {
  return FileSystemLogger::GetInstance().logger().LogFatal();
}

}  // namespace filesystem
}  // namespace ara

#endif  // ARA_FILESYSTEM_FILESYSTEM_LOGGER_H_
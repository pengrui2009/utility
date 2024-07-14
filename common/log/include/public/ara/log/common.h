/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-05-31 10:33:01
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-07-08 11:55:06
 * @FilePath: /aeg-adaptive-autosar/ara-api/log/include/public/ara/log/common.h
 * @Description: This file contains common types definitions for the logging
 * API.
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#ifndef AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_LOG_COMMON_H_
#define AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_LOG_COMMON_H_

#include <iostream>
#include <memory>

namespace ara {
namespace log {
/**
 * @brief LogLevel, one-to-one correspondence with DltLogLevelType
 * @sa <dlt/dlt_types.h>
 *
 * @uptrace{SWS_LOG_00018}
 */
enum class LogLevel : uint8_t {
  kOff = 0x00,     ///< No logging
  kFatal = 0x01,   ///< Fatal error, not recoverable
  kError = 0x02,   ///< Error with impact to correct functionality
  kWarn = 0x03,    ///< Warning if correct behavior cannot be ensured
  kInfo = 0x04,    ///< Informational, providing high level understanding
  kDebug = 0x05,   ///< Detailed information for programmers
  kVerbose = 0x06  ///< Extra-verbose debug messages (highest grade of information)
};

/**
 * @brief LogMode,support multiple modes to output at the same time with & or | operator.
 *
 * @uptrace{SWS_LOG_00019}
 * @uptrace{LOG_CST_00003}
 */
enum class LogMode : uint8_t { kRemote = 0x01, kFile = 0x02, kConsole = 0x04 };

/**
 * @brief
 *
 */
enum class ClientState : int8_t {
  kUnknown = -1,  //< DLT back-end not up and running yet, state cannot be determined
  kNotConnected,  //< No remote client detected
  kConnected      //< Remote client is connected
};

/**
 * @brief loge message mode,kModeled corresponde with non-verbose in DLT protocol,
 * kNonModeled corresponde with verbose in protocol
 *
 */
enum class MessageMode : uint8_t {
  kModeled = 1,  // non-verbose mode
  kNonModeled    // verbose mode
};

namespace internal {
/**
 * @brief File save mode
 *
 */
enum class FileSaveMode : int8_t {
  kSize = 1,  // fix size
  kMinutely,  // every minute
  kHourly,    // every hour
  kDaily      // every day
};

/**
 * @brief File encode
 *
 */
enum class FileEncode : int8_t { kAscii = 1, kBinary };

/**
 * @brief When LogMode is kFile, the behavior of saving is defined by this struct.
 * @param path file path
 * @param file_name file name. Doesn't include suffix.
 * @param createDirs wheather create file path
 * @param saveMode file save mode, default kSize
 * @param fileSize This parameter is valid in kSize Mode, bytes in unit, indicating the maximum number of bytes saved
 * per file, the default value is 64Mb
 * @param max_files Maximum number of saved files. Default value is 5
 *
 */
constexpr uint32_t DEFAULT_SIZE = 67108864U;
constexpr uint8_t MAX_FILES = 5U;

class FileModeDescription final {
 private:
  std::string save_path;
  std::string file_name;
  bool create_dirs;
  FileEncode file_encode;
  FileSaveMode save_mode;
  uint32_t file_size;
  uint8_t max_files;

 public:
  const std::string& getSavePath() const noexcept { return save_path; }
  const std::string& getFileName() const noexcept { return file_name; }

  const bool& getCreateDirs() const noexcept { return create_dirs; }
  const FileEncode& getFileEncode() const noexcept { return file_encode; }
  const FileSaveMode& getFileSaveMode() const noexcept { return save_mode; }
  const uint32_t& getFileSize() const noexcept { return file_size; }
  const uint8_t& getMaxFile() const noexcept { return max_files; }

  FileModeDescription(
      const std::string& path, const std::string& filename, const bool& createDirs = false,
      const FileEncode fileEncode = FileEncode::kAscii, const FileSaveMode saveMode = FileSaveMode::kSize,
      const uint8_t maxFiles = MAX_FILES, const uint32_t fileSize = DEFAULT_SIZE)
      : save_path(path),
        file_name(filename),
        create_dirs(createDirs),
        file_encode(fileEncode),
        save_mode(saveMode),
        file_size(fileSize),
        max_files(maxFiles) {}
};

/**
 * @brief Internal LogReturn.
 * @note One-to-one correspondence with DltReturnValue
 *
 * @sa <dls/dlt_types.h>
 */
enum class LogReturnValue : int8_t {
  kReturnLoggingDisabled = -7,
  kReturnUserBufferFull = -6,
  kReturnWrongParameter = -5,
  kReturnBufferFull = -4,
  kReturnPipeFull = -3,
  kReturnPipeError = -2,
  kReturnError = -1,
  kReturnOk = 0,
  kReturnTrue = 1
};

}  // namespace internal

/**
 * @brief LogMode | operator
 *
 */
inline typename std::underlying_type<LogMode>::type operator|(const LogMode lhs, const LogMode rhs) {
  return static_cast<typename std::underlying_type<LogMode>::type>(
      static_cast<typename std::underlying_type<LogMode>::type>(lhs) |
      static_cast<typename std::underlying_type<LogMode>::type>(rhs));
}

inline typename std::underlying_type<LogMode>::type operator|(
    const typename std::underlying_type<LogMode>::type lhs, const LogMode rhs) {
  return static_cast<typename std::underlying_type<LogMode>::type>(
      lhs | static_cast<typename std::underlying_type<LogMode>::type>(rhs));
}

inline typename std::underlying_type<LogMode>::type operator|(
    const LogMode lhs, const typename std::underlying_type<LogMode>::type rhs) {
  return static_cast<typename std::underlying_type<LogMode>::type>(
      static_cast<typename std::underlying_type<LogMode>::type>(lhs) | rhs);
}

}  // namespace log
}  // namespace ara
#endif  // AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_LOG_COMMON_H_

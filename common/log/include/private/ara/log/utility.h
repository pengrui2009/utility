/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-06-21 14:33:12
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-07-01 18:51:00
 * @FilePath: /aeg-adaptive-autosar/ara-api/log/include/private/ara/log/utility.h
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#ifndef AEG_ADAPTIVE_AUTOSAR_PRIVATE_ARA_LOG_UTLITY_H_
#define AEG_ADAPTIVE_AUTOSAR_PRIVATE_ARA_LOG_UTLITY_H_

#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

namespace ara {
namespace log {
namespace internal {

std::vector<std::string> substring(const std::string& str, const std::string& pattern);

inline bool pathExist(const std::string& path) noexcept {
  if (access(path.c_str(), F_OK) < 0) {
    return false;
  }
  return true;
}

bool createDir(const std::string& path, const mode_t& mode) noexcept;

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif

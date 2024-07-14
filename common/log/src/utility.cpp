/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-06-23 19:43:16
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-06-30 17:40:12
 * @FilePath: /aeg-adaptive-autosar/ara-api/log/src/utility.cpp
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include "ara/log/utility.h"
namespace ara {
namespace log {
namespace internal {

std::vector<std::string> substring(const std::string& str,
                                   const std::string& pattern) {
  std::string::size_type pos;
  std::vector<std::string> result;
  std::string temp = str;
  if (str == "" || pattern == "") {
    result.emplace_back(str);
    return result;
  }
  temp += pattern;
  const size_t str_size = temp.size();
  size_t i = 0U;
  while (i < str_size) {
    pos = temp.find(pattern, i);
    if (pos < str_size) {
      result.push_back(temp.substr(i, pos - i));
      i = pos + pattern.size();
    } else {
      i++;
    }
  }

  return result;
}

bool createDir(const std::string& path, const mode_t& mode) noexcept {
  if (pathExist(path)) {
    return true;
  }
  std::vector<std::string> temp = substring(path, "/");
  std::string::size_type len = temp.size();

  uint32_t i = 0U;
  std::string temp_path = temp[0];
  while (len >= 1U) {
    i++;
    if ((temp_path != "") && (!pathExist(temp_path))) {
      if (-1 == mkdir(temp_path.c_str(), mode)) {
        return false;
      }
    }
    if (i < temp.size()) {
      temp_path += "/" + temp[i];
    }
    len--;
  }

  return true;
}

}  // namespace internal
}  // namespace log
}  // namespace ara
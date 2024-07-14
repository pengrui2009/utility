/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2023-02-07 10:28:37
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-08 17:34:54
 * @Description:
 *
 * Copyright (c) 2023 by Tusimple, All Rights Reserved.
 */
#pragma once

#include <string>
#include <vector>

namespace ara {

namespace socket {

typedef struct InterfaceInfo {
 public:
  // int32_t af;           // address family
  std::string mac;      // mac address
  std::string address;  // interface address
  // std::string mask;               // interface mask
  std::string broadcast_address;  // interface address
  std::string name;               // interface name

  bool operator==(const InterfaceInfo& rhs) const {
    return (mac == rhs.mac) && (address == rhs.address) &&
           (broadcast_address == rhs.broadcast_address) && (name == rhs.name);
  }
} InterfaceInfo;

std::string GetMacFromIp(const std::string& ip);

std::string GetNameFromIp(const std::string& ip);

std::string GetMacFromName(const std::string& name);

InterfaceInfo GetInterfaceFromName(const std::string& name);

std::vector<std::string> GetAllInterfaceName();

std::vector<InterfaceInfo> GetAllInterfaces();

}  // namespace socket
}  // namespace ara
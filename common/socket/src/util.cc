/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2023-01-31 09:52:31
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-06-13 10:35:14
 * @Description:
 *
 * Copyright (c) 2023 by Tusimple, All Rights Reserved.
 */
#include "ara/socket/util.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#ifdef __QNX__
#include <net/if_dl.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "ara/socket/logger.h"

namespace ara {

namespace socket {

extern SocketLogger logger;

std::string GetMacFromIp(const std::string& ip) {
  return GetMacFromName(GetNameFromIp(ip));
};

std::string GetNameFromIp(const std::string& ip) {
  if (ip == "" || ip == "0.0.0.0") return "";

  int32_t sock = ::socket(AF_INET, SOCK_STREAM, 0);

  for (auto& name : GetAllInterfaceName()) {
    struct ifreq ifr;
    strncpy(ifr.ifr_name, name.c_str(), sizeof(ifr.ifr_name) - 1);
    ifr.ifr_name[sizeof(ifr.ifr_name) - 1] = '\0';
    if (-1 == ioctl(sock, SIOCGIFADDR, &ifr)) {
      logger.LogWarn() << "ioctl(SIOCGIFADDR) name:" << name
                       << "Error:" << strerror(errno);
      close(sock);
      return "";
    }
    char* p = inet_ntoa(((struct sockaddr_in*)(&ifr.ifr_addr))->sin_addr);
    // strncpy(addr, p, sizeof(addr) - 1);
    // addr[sizeof(addr) - 1] = '\0';
    if (std::string(p) == ip) {
      close(sock);
      return name;
    }
  }
  close(sock);
  return "";
}

std::string GetMacFromName(const std::string& name) {
  if (name == "") return "";
  struct ifreq ifr;
  char mac[32];
  int32_t sock = ::socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    logger.LogWarn() << "name:" << name << "socket()" << strerror(errno);
    return "";
  }
  // copy name to ifr_name
  strncpy(ifr.ifr_name, name.c_str(), sizeof(ifr.ifr_name) - 1);
  ifr.ifr_name[sizeof(ifr.ifr_name) - 1] = '\0';
#if defined(__linux__)
  int32_t j, k = 0;
  /*Get hardware mac*/
  if (-1 == ioctl(sock, SIOCGIFHWADDR, &ifr)) {
    logger.LogWarn() << "name:" << name << "ioctl(SIOCGIFHWADDR)"
                     << strerror(errno);
    close(sock);
  }
  for (j = 0, k = 0; j < 6; j++) {
    k += snprintf(mac + k, sizeof(mac) - k - 1, j ? ":%02X" : "%02X",
                  (int)(unsigned int)(unsigned char)ifr.ifr_hwaddr.sa_data[j]);
  }
#elif defined(__QNX__)
  int32_t j = 0;
  struct ifaddrs *ifaddr, *ifa;
  if (-1 == getifaddrs(&ifaddr)) {
    logger.LogWarn() << "name:" << name << "getifaddrs" << strerror(errno);
  }
  for (ifa = ifaddr, j = 0; ifa != NULL; ifa = ifa->ifa_next, j++) {
    if (ifa->ifa_addr == NULL) continue;
    if (strcmp(ifa->ifa_name, name.c_str())) {
      struct sockaddr_dl* sdl = (struct sockaddr_dl*)ifa->ifa_addr;
      std::memcpy(mac, LLADDR(sdl), sdl->sdl_alen);
    }
  }
  freeifaddrs(ifa);
#endif
  mac[sizeof(mac) - 1] = '\0';
  close(sock);
  return std::string(mac);
}

InterfaceInfo GetInterfaceFromName(const std::string& name) {
  if (name == "") return InterfaceInfo{"", "", "", ""};

  for (auto& inter : GetAllInterfaces()) {
    if (inter.name == name) {
      return inter;
    }
  }

  return InterfaceInfo{"", "", "", ""};
}

std::vector<InterfaceInfo> GetAllInterfaces() {
  std::vector<InterfaceInfo> res;
  char mac[32], buf[1024];
  struct ifconf ifc;
  struct ifreq* ifr;
  int32_t nInterfaces = 0;

  /* Get a socket handle. */
  int32_t sock = ::socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    logger.LogWarn() << "socket()" << strerror(errno);
    return res;
  }

  /* Query available interfaces. */
  ifc.ifc_len = sizeof(buf);
  ifc.ifc_buf = buf;
  if (ioctl(sock, SIOCGIFCONF, &ifc) < 0) {
    logger.LogWarn() << "ioctl(SIOCGIFCONF)" << strerror(errno);
    close(sock);
    return res;
  }

  /* Iterate through the list of interfaces. */
  ifr = ifc.ifc_req;
  nInterfaces = ifc.ifc_len / sizeof(struct ifreq);
  for (int32_t i = 0; i < nInterfaces; i++) {
    struct ifreq* item = &ifr[i];
    InterfaceInfo info;
    /* Show the device name and IP address */
    info.name = std::string(item->ifr_name);
    info.address = std::string(
        inet_ntoa(((struct sockaddr_in*)&item->ifr_addr)->sin_addr));

#if defined(__linux__)
    int32_t j, k = 0;
    /*Get hardware mac*/
    if (-1 == ioctl(sock, SIOCGIFHWADDR, item)) {
      logger.LogWarn() << "name:" << info.name << "ioctl(SIOCGIFHWADDR)"
                       << strerror(errno);
    }
    for (j = 0, k = 0; j < 6; j++) {
      k += snprintf(
          mac + k, sizeof(mac) - k - 1, j ? ":%02X" : "%02X",
          (int)(unsigned int)(unsigned char)item->ifr_hwaddr.sa_data[j]);
    }
#elif defined(__QNX__)
    int32_t j = 0;
    struct ifaddrs *ifaddr, *ifa;
    if (-1 == getifaddrs(&ifaddr)) {
      logger.LogWarn() << "name:" << info.name << "getifaddrs"
                       << strerror(errno);
    }
    for (ifa = ifaddr, j = 0; ifa != NULL; ifa = ifa->ifa_next, j++) {
      if (ifa->ifa_addr == NULL) continue;
      if (strcmp(ifa->ifa_name, info.name.c_str())) {
        struct sockaddr_dl* sdl = (struct sockaddr_dl*)ifa->ifa_addr;
        std::memcpy(mac, LLADDR(sdl), sdl->sdl_alen);
      }
    }
    freeifaddrs(ifa);
#endif
    mac[sizeof(mac) - 1] = '\0';
    info.mac = std::string(mac);

    /* Get the broadcast address */
    if (ioctl(sock, SIOCGIFBRDADDR, item) >= 0) {
      // printf(", BROADCAST %s",
      //        inet_ntoa(((struct
      //        sockaddr_in*)&item->ifr_broadaddr)->sin_addr));
      info.broadcast_address = std::string(
          inet_ntoa(((struct sockaddr_in*)&item->ifr_broadaddr)->sin_addr));
    }

    res.emplace_back(info);
  }
  close(sock);
  return res;
}

std::vector<std::string> GetAllInterfaceName() {
  std::vector<std::string> res;
  char buf[1024];
  struct ifconf ifc;
  struct ifreq* ifr;
  int32_t nInterfaces = 0;

  /* Get a socket handle. */
  int32_t sock = ::socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    logger.LogWarn() << "socket()" << strerror(errno);
    return res;
  }

  /* Query available interfaces. */
  ifc.ifc_len = sizeof(buf);
  ifc.ifc_buf = buf;
  if (ioctl(sock, SIOCGIFCONF, &ifc) < 0) {
    logger.LogWarn() << "ioctl(SIOCGIFCONF)" << strerror(errno);
    close(sock);
    return res;
  }

  /* Iterate through the list of interfaces. */
  ifr = ifc.ifc_req;
  nInterfaces = ifc.ifc_len / sizeof(struct ifreq);
  for (int32_t i = 0; i < nInterfaces; i++) {
    struct ifreq* item = &ifr[i];

    res.emplace_back(std::string(item->ifr_name));

    /* Show the device name and IP address */
    // printf("%s: IP %s", item->ifr_name,
    //        inet_ntoa(((struct sockaddr_in*)&item->ifr_addr)->sin_addr));

    /* Get the broadcast address (added by Eric) */
    // if (ioctl(sock, SIOCGIFBRDADDR, item) >= 0)
    //   printf(", BROADCAST %s",
    //          inet_ntoa(((struct
    //          sockaddr_in*)&item->ifr_broadaddr)->sin_addr));
    // printf("\n");
  }
  close(sock);
  return res;
}

}  // namespace socket
}  // namespace ara
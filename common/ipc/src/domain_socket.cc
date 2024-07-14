/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-17 15:15:57
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-05-06 15:09:48
 * @FilePath: /aeg-adaptive-autosar/ara-api/common/ipc/src/domain_socket.cc
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include "ara/ipc/domain_socket.h"

#include <pthread.h>

#include "ara/ipc/ipc_logger.h"
#include "boost/bind/bind.hpp"
#ifdef __QNX__
#include <sys/socket.h>
#endif

namespace ara {
namespace ipc {
extern IpcLogger logger;

BaseDomainSocket::BaseDomainSocket(uint32_t n_threads)
    : thread_nums(n_threads), pool(n_threads){};

const char* BaseDomainSocket::getDomainSocketFilePrefixPath() {
  char* environment_socket_file_prefix_path =
      getenv("AP_DOMAIN_SOCKET_FILE_PREFIX_PATH");
  if (environment_socket_file_prefix_path) {
    logger.LogWarn() << "IPC client domain socket file prefix path:"
                     << environment_socket_file_prefix_path;
    return environment_socket_file_prefix_path;
  } else {
    return "/tmp";
  }
}

void BaseDomainSocket::Run() {
  for (uint32_t i = 0; i < thread_nums; i++) {
    boost::asio::post(pool, [this, i]() {
      pthread_setname_np(pthread_self(),
                         std::string("ara-ipc-" + std::to_string(i)).c_str());
#ifdef __QNX__
      char* environment_sock_prefix = getenv("AP_DOMAIN_SOCKET_SOCK");
      if (environment_sock_prefix) {
        if (setsockprefix(environment_sock_prefix) == NULL) {
          // set socket prefix error
          logger.LogWarn() << "IPC inner thread:" << gettid()
                           << "set domain socket sock prefix:"
                           << environment_sock_prefix
                           << "error. Use default sock";
        } else {
          logger.LogInfo() << "IPC inner thread:" << gettid()
                           << "set domain socket sock prefix:"
                           << environment_sock_prefix;
        }
      }
#endif
      io_context_.run();
    });
  }
}

void BaseDomainSocket::Stop() {
  io_context_.stop();  // stop all pending handlers
  pool.stop();
  pool.join();
}

BaseDomainSocket::~BaseDomainSocket() {
  io_context_.stop();
  // wait all pending handlers
  pool.join();
}

}  // namespace ipc
}  // namespace ara
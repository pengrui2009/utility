/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-15 17:17:39
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-05-06 15:10:08
 * @FilePath:
 * /aeg-adaptive-autosar/ara-api/common/ipc/include/public/ara/ipc/domain_socket.h
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#ifndef AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_DOMAIN_SOCKET_H_
#define AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_DOMAIN_SOCKET_H_

#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>

#include "ara/ipc/process_handler.h"

namespace ara {
namespace ipc {

using boost::asio::io_context;
using boost::asio::thread_pool;
using boost::asio::local::stream_protocol;

class BaseDomainSocket {
 protected:
  io_context io_context_;
  /* number of threads for running io_text */
  uint32_t thread_nums;
  /* thread pool used for io_text.run */
  thread_pool pool;

 public:
  /**
   * @brief Run io_context.run in multi-threads
   *
   */
  void Run();
  /* Stop all io_text.run */
  void Stop();
  /**
   * @brief Get the Domain Socket File Prefix Path
   *
   * @return const char*
   */
  const char* getDomainSocketFilePrefixPath();
  BaseDomainSocket(uint32_t n_threads);
  virtual ~BaseDomainSocket();
};

}  // namespace ipc
}  // namespace ara

#endif  // AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_DOMAIN_SOCKET_H_
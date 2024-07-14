/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-16 10:05:31
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-10-08 12:11:46
 * @FilePath:
 * /aeg-adaptive-autosar/ara-api/common/ipc/include/public/ara/ipc/server_domain_socket.h
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#ifndef AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_SERVER_DOMAIN_SOCKET_H_
#define AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_SERVER_DOMAIN_SOCKET_H_

#include <memory>

namespace ara {
namespace ipc {
class IPCMessage;
template <typename T>
class ProcessHandler;
template <typename T>
using ProcessHandlerPtr = std::shared_ptr<ProcessHandler<T>>;

template <typename T = ara::ipc::IPCMessage>
class ServerDomainSocket {
 private:
  class pImpl;
  std::unique_ptr<pImpl> pImpl_;

 public:
  /* return number of connections */
  uint32_t Connetions();
  void Run();
  void Stop();
  /**
   * @brief Construct a new Server Domain Socket object
   *
   * @param n_threads
   * @param socket_file
   * @param handler
   */
  ServerDomainSocket(uint32_t n_threads, const std::string socket_file,
                     ProcessHandlerPtr<T> handler);
  ~ServerDomainSocket();
};

}  // namespace ipc
}  // namespace ara

#endif
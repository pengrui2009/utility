/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-16 10:05:31
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-10-05 22:07:40
 * @FilePath:
 * /aeg-adaptive-autosar/ara-api/common/ipc/include/public/ara/ipc/client_domain_socket.h
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#ifndef AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_CLIENT_DOMAIN_SOCKET_H_
#define AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_CLIENT_DOMAIN_SOCKET_H_

#include <ara/core/future.h>
#include <ara/core/promise.h>
#include <future>

#include <iostream>
#include <map>
#include <memory>

namespace ara {
namespace ipc {

class IPCMessage;

template <typename T>
using future_type = ara::core::Future<T>;

template <typename T = ara::ipc::IPCMessage>
class ClientDomainSocket {
 private:
  class pImpl;
  std::unique_ptr<pImpl> pImpl_;

 public:
  /**
   * @brief send ipc message for client
   *
   * @param msg
   * @return future_type<T>
   */
  future_type<T> Send(T& msg);
  /**
   * @brief return true or false when the client connection is connected or not
   *
   * @return true connected
   * @return false not connected
   */
  bool Connected() const noexcept;
  // ara::core::Future<bool> connect();
  /**
   * @brief
   *
   * @return std::future<bool>
   */
  std::future<bool> connect();
  /**
   * @brief 停止当前所有的未完成的异步操作，移动所有未完成的request，停止io
   * context，关闭client端socket
   *
   */
  void disconnect();
  ClientDomainSocket(uint32_t n_threads, const std::string socket_file);
  ~ClientDomainSocket();
};

}  // namespace ipc
}  // namespace ara

#endif

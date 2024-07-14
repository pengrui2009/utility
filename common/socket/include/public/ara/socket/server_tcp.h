/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2023-01-12 15:30:43
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-15 10:53:46
 * @FilePath:
 * /adas-monorepo/modules/adaptive_autosar/ara-api/common/socket/include/public/ara/socket/server_tcp_socket.h
 * @Description:
 *
 * Copyright (c) 2023 by Tusimple, All Rights Reserved.
 */

#pragma once

#include <ara/core/future.h>
#include <ara/core/promise.h>

#include <memory>

namespace ara {
namespace socket {
namespace tcp {

class Connection;
using ConnectionPtr = std::shared_ptr<Connection>;

class ServerTcp {
 private:
  class pImpl;
  std::unique_ptr<pImpl> pImpl_;

  template <typename T>
  using future_type = ara::core::Future<T>;

 public:
  /**
   * @brief Construct a new Server Tcp object
   *
   * @param ip Local ip
   * @param port Local port
   * @param thread_nums Thread numbers in inner threadpoll
   */
  ServerTcp(const std::string& ip, const uint32_t& port,
            const uint32_t& thread_nums);

  /**
   * @brief Construct a new Server Tcp object with ip 0.0.0.0
   *
   * @param port Local port
   * @param thread_nums Thread numbers in inner threadpoll
   */
  ServerTcp(const uint32_t& port, const uint32_t& thread_nums);

  /**
   * @brief Get the mac address of the bind interface
   *
   * @return std::string Return "" if no interface is specificed.
   */
  std::string Mac();

  /**
   * @brief Set the Acceptor Handler object. Any new connection comes, this
   * handler will be invoked.
   *
   * @param func
   */
  void SetAcceptorHandler(std::function<void(const ConnectionPtr& conn)> func);
  ~ServerTcp();

  /**
   * @brief Start listening on local port and local address
   *
   */
  void Run();

  /**
   * @brief Close server
   *
   */
  void Close();
};

}  // namespace tcp
}  // namespace socket
}  // namespace ara

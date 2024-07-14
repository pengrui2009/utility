/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2023-01-12 15:30:43
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-03-06 16:04:32
 * @FilePath:
 * /adas-monorepo/modules/adaptive_autosar/ara-api/common/socket/include/public/ara/socket/server_tcp_socket.h
 * @Description:
 *
 * Copyright (c) 2023 by Tusimple, All Rights Reserved.
 */

#pragma once

#include <ara/core/future.h>
#include <ara/socket/data_buffer.h>

#include <memory>

namespace ara {
namespace socket {
namespace udp {

template <typename T>
using future_type = ara::core::Future<T>;
class SocketUdp {
 private:
  class pImpl;
  std::unique_ptr<pImpl> pImpl_;

  template <typename T>
  using future_type = ara::core::Future<T>;

 public:
  /**
   * @brief Construct a new Socket Udp object
   *
   * @param local_address Local ip adress/Multicast address/Direct broadcast
   * address. Network interface will be bound according to local ip.
   * @param local_port Local port
   * @param thread_nums Thread numbers in inner threadpoll
   */
  SocketUdp(const std::string& local_address, const uint32_t& local_port,
            const uint32_t& thread_nums);

  /**
   * @brief Construct a new Socket Udp object with ip 0.0.0.0
   *
   * @param local_port Local port
   * @param thread_nums Thread numbers in inner threadpoll
   */
  SocketUdp(const uint32_t& local_port, const uint32_t& thread_nums);

  /**
   * @brief Construct a new Socket Udp object
   *
   * @param local_address Local ip adress/Multicast address/Direct broadcast
   * address. Network interface will be bound according to local ip.
   *
   * @param thread_nums Thread numbers in inner threadpoll
   */
  SocketUdp(const std::string& local_address, const uint32_t& thread_nums);

  ~SocketUdp();

  /**
   * @brief Get the mac address of the bind interface
   *
   * @return std::string Return "" if no interface is specificed.
   */
  std::string Mac();

  /**
   * @brief Close udp socket
   *
   */
  void Close();

  /**
   * @brief Start listening on local address
   *
   */
  void StartReceiving();

  /**
   * @brief Join the multicast group. This functions can be called multi-times
   * to set more than one multicast addresses. It is possible to join the same
   * group on more than one interface.
   *
   * @param multicast_ip Multicast group to join
   * @param local_interface Interface to join on. Set to "" to use INADDR_ANY
   */
  void JoinGroup(const std::string& multicast_ip,
                 const std::string& local_interface = "");

  /**
   * @brief Set the Receive Callback object. Any data comes, this callback will
   * be invoked.
   *
   * @param func
   */
  void SetReceiveCallback(
      std::function<void(SocketBufferType&& data, const std::string& ip,
                         const uint32_t& port)>
          func);

  /**
   * @brief Get the broadcast address with the bound interface
   *
   * @return std::string
   */
  std::string GetBroadcastIP();

  /**
   * @brief Send data to remote endpoint.
   *
   * @param ip Remote ip
   * @param port Remote port
   * @param data Send data
   * @return future_type<std::size_t> Bytes were sent.
   */
  future_type<std::size_t> SendTo(const std::string& ip,
                                  const std::uint32_t& port,
                                  SocketBufferType& data);

  /**
   * @brief Send data to remote endpoint.
   *
   * @param ip Remote ip
   * @param port Remote port
   * @param data Send data
   * @param size Send data size
   * @return future_type<std::size_t> Bytes were sent.
   */
  future_type<std::size_t> SendTo(const std::string& ip,
                                  const std::uint32_t& port, const void* data,
                                  std::size_t size);
};

}  // namespace udp
}  // namespace socket
}  // namespace ara

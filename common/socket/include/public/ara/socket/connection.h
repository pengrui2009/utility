/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-15 21:58:10
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-15 10:49:10
 * @FilePath:
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#pragma once

#include <ara/core/future.h>
#include <ara/socket/data_buffer.h>

#include <future>

namespace ara {
namespace socket {
namespace tcp {
template <typename T>
using future_type = ara::core::Future<T>;
// using future_type = std::future<T>;

/// Represents a single connection from a client.
class Connection : public std::enable_shared_from_this<Connection> {
 private:
  friend class ConnectionManager;
  class pImpl;
  std::unique_ptr<pImpl> pImpl_;

  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;

 protected:
  /// Construct a connection.
  Connection();

 public:
  /// Start the first asynchronous operation for the connection.
  // void start();

  /**
   * @brief Close the current connection
   *
   */
  void Stop();

  /**
   * @brief Send reference data in Buffer to remote endpoint
   *
   * @param data
   * @return future_type<std::size_t>
   */
  future_type<std::size_t> Write(SocketBufferType& data);

  /**
   * @brief Send data in Buffer to remote point and release Buffer
   *
   * @param data
   * @return future_type<std::size_t>
   */
  future_type<std::size_t> Write(SocketBufferType&& data);

  /**
   * @brief Send data to remote point
   *
   * @param data Data will be sent
   * @param size Data size
   * @return future_type<std::size_t>
   */
  future_type<std::size_t> Write(const void* data, std::size_t size);

  /**
   * @brief Read some bytes to a new Buffer with received size
   *
   * @note The Readsome operation may not read all of the requested number of
   * bytes.
   *
   * @return future_type<SocketBufferType> The new buffer
   */
  future_type<SocketBufferType> Readsome();

  /**
   * @brief Read some bytes to the data with size no more than max_size
   *
   * @note The Readsome operation may not read all of the requested number of
   * bytes.
   *
   * @return future_type<std::size_t> Received data size
   */
  future_type<std::size_t> Readsome(void* data, std::size_t max_size);

  /**
   * @brief Read bytes with the specific size to a new Buffer
   *
   * @param size bytes size
   * @return future_type<SocketBufferType> The new buffer
   */
  future_type<SocketBufferType> Read(std::size_t size);

  /**
   * @brief Read bytes with the specific size to data
   *
   * @param data data buffer
   * @param size bytes size
   * @return future_type<std::size_t> Received size
   */
  future_type<std::size_t> Read(void* data, std::size_t size);

  /**
   * @brief Get the remote ip
   *
   * @return const std::string
   */
  const std::string Ip();

  /**
   * @brief Get the local ip
   *
   * @return const std::string Return "" if no interface is specificed.
   */
  const std::string LocalIp();

  /**
   * @brief Get the protocol
   *
   * @return const int32_t
   */
  const int32_t Protocol();

  /**
   * @brief Get the remote port
   *
   * @return const int32_t
   */
  const uint16_t Port();

  /**
   * @brief Get the local port
   *
   * @return const int32_t
   */
  const uint16_t LocalPort();

  /**
   * @brief Get the connection socket fd
   *
   * @return const int32_t
   */
  const int32_t UniqueId();
};

using ConnectionPtr = std::shared_ptr<Connection>;

}  // namespace tcp
}  // namespace socket
}  // namespace ara

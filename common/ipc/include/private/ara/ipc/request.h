/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-15 21:58:10
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-24 19:07:06
 * @FilePath:
 * /aeg-adaptive-autosar/ara-api/common/ipc/include/public/ara/ipc/request.h
 * /aeg-adaptive-autosar/ara-api/common/ipc/include/public/ara/ipc/request.h
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#ifndef AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_REQUEST_H_
#define AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_REQUEST_H_
#include <ara/core/promise.h>

#include <boost/asio/strand.hpp>

#include "ara/ipc/handle.h"

namespace ara {
namespace ipc {

template <typename T>
using promise_type = ara::core::Promise<T>;
template <typename T>
using future_type = ara::core::Future<T>;

/// Represents a single request from client.
template <typename T>
class request : public Handler<T>,
                public std::enable_shared_from_this<request<T>> {
 public:
  request(const request&) = delete;
  request& operator=(const request&) = delete;

  /// Construct a request with the given socket.
  request(uint32_t id, stream_protocol::socket& socket,
          std::atomic_bool& connected,
          boost::asio::io_service::strand& strand_write,
          boost::asio::io_service::strand& strand_read, promise_type<T>&& p,
          T& req_msg, handler_manager<T>& manager);
  virtual ~request();

  /// Start the first asynchronous operation for the connection.
  virtual void start() override;
  virtual void stop(){};

 private:
  uint32_t request_id;
  stream_protocol::socket& socket_;
  std::atomic_bool& connected_;
  boost::asio::io_service::strand& strand_write_;
  boost::asio::io_service::strand& strand_read_;
  /// Perform an asynchronous read operation.
  virtual void do_read() override;

  /// Perform an asynchronous write operation.
  virtual void do_write(T& msg) override;

  void read_header(const boost::system::error_code& ec, std::size_t t);
  void read_payload(const boost::system::error_code& ec, std::size_t t);
  void write_msg(T& msg);
  void write_msg_without_response(T& msg);
  boost::system::error_code write_header(T& msg);
  boost::system::error_code write_payload(T& msg);

  promise_type<T> promise_;

  T& req_msg_;
  T rec_msg_;

  using std::enable_shared_from_this<request<T>>::shared_from_this;
};

template <typename T>
using request_ptr = std::shared_ptr<request<T>>;

}  // namespace ipc
}  // namespace ara

#endif  // AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_REQUEST_H_

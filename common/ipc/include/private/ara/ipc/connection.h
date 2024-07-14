/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-15 21:58:10
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-08-26 11:34:18
 * @FilePath: /aeg-adaptive-autosar/ara-api/common/ipc/include/public/ara/ipc/connection.h
 * /aeg-adaptive-autosar/ara-api/common/ipc/include/public/ara/ipc/connection.h
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#ifndef AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_CONNECTION_H_
#define AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_CONNECTION_H_

#include "ara/ipc/handle.h"
#include "ara/ipc/process_handler.h"

namespace ara {
namespace ipc {

/// Represents a single connection from a client.
template <typename T>
class connection : public Handler<T>, public std::enable_shared_from_this<connection<T>> {
 public:
  connection(const connection&) = delete;
  connection& operator=(const connection&) = delete;

  /// Construct a connection with the given socket.
  explicit connection(stream_protocol::socket socket, handler_manager<T>& manager, ProcessHandlerPtr<T>& handler);

  /// Start the first asynchronous operation for the connection.
  virtual void start() override;

  /// Stop all asynchronous operations associated with the connection.
  virtual void stop() override;

  stream_protocol::socket& socket() { return socket_; }

 private:
  /// Perform an asynchronous read operation.
  virtual void do_read() override;

  /// Perform an asynchronous write operation.
  virtual void do_write(T& msg) override;

  /// Socket for the connection.
  stream_protocol::socket socket_;

  /// The handler used to process the incoming request.
  ProcessHandlerPtr<T>& process_handler_;

  void read_header(const boost::system::error_code& ec);
  void read_payload(const boost::system::error_code& ec);

  /// IpcMessage
  T buffer_;

  using std::enable_shared_from_this<connection<T>>::shared_from_this;
};

}  // namespace ipc
}  // namespace ara

#endif  // AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_CONNECTION_H_

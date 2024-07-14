/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-15 21:58:10
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-08-17 14:37:03
 * @FilePath:
 * /aeg-adaptive-autosar/ara-api/common/ipc/include/public/ara/ipc/handle.h
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#ifndef AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_HANDLE_H_
#define AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_HANDLE_H_

#include <boost/asio.hpp>

namespace ara {
namespace ipc {

using boost::asio::local::stream_protocol;

template <typename T>
class handler_manager;

template <typename T>
class Handler {
 public:
  Handler(const Handler&) = delete;
  Handler& operator=(const Handler&) = delete;
  Handler(handler_manager<T>& manager) : handler_manager_(manager){};
  virtual ~Handler(){};
  virtual void start(){};
  virtual void stop(){};

 protected:
  /// Perform an asynchronous read operation.
  virtual void do_read() = 0;

  /// Perform an asynchronous write operation.
  virtual void do_write(T& msg) = 0;

  /// The manager for this connection.
  handler_manager<T>& handler_manager_;
};

template <typename T>
using handler_ptr = std::shared_ptr<Handler<T>>;

}  // namespace ipc
}  // namespace ara

#endif  // AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_HANDLE_H_

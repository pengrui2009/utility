/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-16 10:03:54
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-10-05 22:50:21
 * @FilePath:
 * /aeg-adaptive-autosar/ara-api/common/ipc/include/public/ara/ipc/connection_manager.h
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#ifndef AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_CONNECTION_MANAGER_H_
#define AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_CONNECTION_MANAGER_H_

#include <set>

#include "ara/ipc/handle.h"

namespace ara {
namespace ipc {

/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
template <typename T>
class handler_manager {
 public:
  handler_manager(const handler_manager&) = delete;
  handler_manager& operator=(const handler_manager&) = delete;

  /// Construct a connection manager.
  handler_manager();

  /// Add the specified connection to the manager and start it.
  void start(handler_ptr<T> c);

  /// Stop the specified connection.
  void stop(handler_ptr<T> c);

  /// Stop all connections.
  void stop_all();

  uint32_t size();

 private:
  /// The managed connections.
  std::set<handler_ptr<T>> connections_;
  std::mutex mtx_;
};

}  // namespace ipc
}  // namespace ara

#endif  // HTTP_CONNECTION_MANAGER_HPP

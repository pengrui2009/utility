/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-16 10:03:54
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-01-30 14:08:10
 * @FilePath:
 * /aeg-adaptive-autosar/ara-api/common/ipc/include/public/ara/ipc/connection_manager.h
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <set>
#include <unordered_map>

#include "ara/socket/connection.h"

namespace ara {
namespace socket {
class Handler;
namespace tcp {
/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
class ConnectionManager {
 public:
  ConnectionManager(const ConnectionManager&) = delete;
  ConnectionManager& operator=(const ConnectionManager&) = delete;

  /// Construct a connection manager.
  ConnectionManager();

  /// Add the specified connection to the manager and start it.
  void start(boost::asio::ip::tcp::socket&& socket,
             std::function<void(const ConnectionPtr& conn)>& handler);

  /// Stop the specified connection.
  void stop(ConnectionPtr c);

  /// Stop all connections.
  void stop_all();

 private:
  /// The managed connections.
  std::set<ConnectionPtr> connections_;

  std::mutex mtx_;
};

}  // namespace tcp
}  // namespace socket
}  // namespace ara
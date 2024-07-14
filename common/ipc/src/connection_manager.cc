/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-16 19:09:38
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-09-22 13:39:11
 * @FilePath: /aeg-adaptive-autosar/ara-api/common/ipc/src/connection_manager.cc
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include "ara/ipc/connection_manager.h"
#include "ara/ipc/ipc_message.h"
#include "ara/ipc/new_ipc_message.h"

namespace ara {
namespace ipc {
template <typename T>
handler_manager<T>::handler_manager() {}

template <typename T>
void handler_manager<T>::start(handler_ptr<T> c) {
  std::lock_guard<std::mutex> lk(mtx_);
  connections_.insert(c);
  c->start();
}

template <typename T>
void handler_manager<T>::stop(handler_ptr<T> c) {
  std::lock_guard<std::mutex> lk(mtx_);
  connections_.erase(c);
  c->stop();
}

template <typename T>
void handler_manager<T>::stop_all() {
  std::lock_guard<std::mutex> lk(mtx_);
  for (auto c : connections_) c->stop();
  connections_.clear();
}

template <typename T>
uint32_t handler_manager<T>::size() {
  std::lock_guard<std::mutex> lk(mtx_);
  return connections_.size();
}

template class handler_manager<IPCMessage>;
template class handler_manager<NewIpcMessage>;

}  // namespace ipc
}  // namespace ara
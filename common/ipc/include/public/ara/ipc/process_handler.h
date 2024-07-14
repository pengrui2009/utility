/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-16 10:04:29
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-11-08 15:43:34
 * @FilePath:
 * /aeg-adaptive-autosar/ara-api/common/ipc/include/public/ara/ipc/process_handler.h
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#ifndef AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_PROCESS_HANDLER_H_
#define AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_PROCESS_HANDLER_H_

#include <memory>
#include <string>

namespace ara {
namespace ipc {

/// The common handler for all incoming requests.
template <typename T>
class ProcessHandler {
 public:
  ProcessHandler(const ProcessHandler&) = delete;
  ProcessHandler& operator=(const ProcessHandler&) = delete;

  ProcessHandler(){};
  virtual ~ProcessHandler(){};

  /// Async read call back.
  virtual T OnReadCallBack(T& msg) { return T(); };
};

template <typename T>
using ProcessHandlerPtr = std::shared_ptr<ProcessHandler<T>>;

}  // namespace ipc
}  // namespace ara

#endif  // AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_PROCESS_HANDLER_H_

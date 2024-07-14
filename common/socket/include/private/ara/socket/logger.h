/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-15 21:58:10
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-01-13 18:49:30
 * @FilePath:
 * /aeg-adaptive-autosar/ara-api/common/ipc/include/public/ara/ipc/ipc_logger.h
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#ifndef AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_IPC_LOGGER_H_
#define AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_IPC_LOGGER_H_

#include <ara/log/logger.h>

namespace ara {
namespace socket {

class SocketLogger {
 public:
  SocketLogger() : ap_logger_(InitApLogger()) {}

  const ara::log::Logger &ap_logger() { return ap_logger_; }

  ara::log::LogStream LogFatal() { return ap_logger_.LogFatal(); }

  ara::log::LogStream LogError() { return ap_logger_.LogError(); }

  ara::log::LogStream LogWarn() { return ap_logger_.LogWarn(); }

  ara::log::LogStream LogInfo() { return ap_logger_.LogInfo(); }

  ara::log::LogStream LogDebug() { return ap_logger_.LogDebug(); }

  ara::log::LogStream LogVerbose() { return ap_logger_.LogVerbose(); }

 private:
  const ara::log::Logger &ap_logger_;

  const ara::log::Logger &InitApLogger() {
    return ara::log::CreateLogger("sock", "");
  };
};

}  // namespace socket
}  // namespace ara

#endif  // AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_IPC_LOGGER_H_

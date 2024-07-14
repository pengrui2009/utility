/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-15 15:01:54
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-28 13:54:33
 * @FilePath:
 * /aeg-adaptive-autosar/ara-api/common/ipc/include/public/ara/ipc/new_ipc_message.h
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#pragma once

#include <ara/ipc/ipc_base_message.h>

namespace ara {
namespace ipc {

class NewIpcMessage : BaseIPCMessage {
 private:
  class pImpl;
  std::unique_ptr<pImpl> pImpl_;

 public:
  static constexpr std::size_t header_length = 17U;
  /**
   * @brief Construct a null New Ipc Message object with size 0
   *
   */
  NewIpcMessage(IPCMessageModeType type = IPCMessageMode::RPC);
  NewIpcMessage(const std::size_t& size, const void* data,
                IPCMessageModeType type = IPCMessageMode::RPC);
  NewIpcMessage(const NewIpcMessage& rhs);
  NewIpcMessage(NewIpcMessage&& rhs);
  NewIpcMessage& operator=(const NewIpcMessage& rhs);
  NewIpcMessage& operator=(NewIpcMessage&& rhs);
  ~NewIpcMessage();

  bool decode_header() const;
  std::size_t payload_length() const;
  /**
   * @brief resize will earse all the original payload data.
   *
   * @param new_size new payload size
   */
  void resize(std::size_t new_size);
  /*override fucntions*/
  virtual char* header() override;
  virtual char* payload() override;
  virtual const char* header() const override;
  virtual const char* payload() const override;
  virtual IPCMessageModeType ipc_mode() const override;
  virtual uint64_t timestamp() const override;
  virtual String timestamp_format() override;
};

}  // namespace ipc
}  // namespace ara

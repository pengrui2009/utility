/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-15 15:01:54
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-07-31 12:57:13
 * @FilePath:
 * /aeg-adaptive-autosar/ara-api/common/ipc/include/public/ara/ipc/ipc_message.h
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#ifndef AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_IPC_MESSAGE_H_
#define AEG_ADAPTIVE_AUTOSAR_PUBLIC_ARA_IPC_IPC_MESSAGE_H_

#include <ara/ipc/ipc_base_message.h>

namespace ara {
namespace ipc {

/**
 * @brief IPCMessage is the transport protocol between server and client which
 * contains header part and payload part. IPCMessage header consists of 13
 * bytes. First 4 bytes is payload size, last 8 bytes is timestamp of IPCMessage
 * was made, final 1 byte denotes the ipc message mode, default is RPC.
 *
 */
class IPCMessage : BaseIPCMessage {
 public:
  using IPCMessageModeType = IPCMessageMode;

  static constexpr std::size_t payload_size_length = 4;
  static constexpr std::size_t timestamp_length = 8;
  static constexpr std::size_t header_length = 13;
  static constexpr std::size_t max_body_length = 1400;
  // A null ipc message
  explicit IPCMessage(IPCMessageModeType type = IPCMessageMode::RPC);
  // Copy data. Timestamp will be generated in this.
  explicit IPCMessage(const uint32_t& size, const void* data,
                      IPCMessageModeType type = IPCMessageMode::RPC);

  IPCMessage(const IPCMessage&);
  IPCMessage(IPCMessage&& msg);
  IPCMessage& operator=(const IPCMessage& msg);
  IPCMessage& operator=(IPCMessage&& msg);

  ~IPCMessage();

  uint32_t payload_length() const;
  /*override fucntions*/
  virtual char* header() override;
  virtual char* payload() override;
  virtual const char* header() const override;
  virtual const char* payload() const override;
  virtual IPCMessageModeType ipc_mode() const override;
  virtual uint64_t timestamp() const override;
  virtual String timestamp_format() override;

 private:
  char data_[header_length + max_body_length];
  void* payload_size_;  // first 0-3 bytes
  void* timestamp_;     // 4-11 bytes
  void* ipc_mode_;      // 12 bytes
  void* payload_;
  void Initialize();
  void Deinitialize();
  bool encode_header(const uint32_t& size, const uint64_t& ts,
                     const IPCMessageModeType& type);
  bool decode_header(uint32_t& size, uint64_t& ts,
                     IPCMessageModeType& mode) const;
};
}  // namespace ipc
}  // namespace ara
#endif  // IPC_MESSAGE_HPP

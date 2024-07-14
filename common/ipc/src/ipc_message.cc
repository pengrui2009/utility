/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-16 15:35:28
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-07-31 13:09:41
 * @FilePath: /aeg-adaptive-autosar/ara-api/common/ipc/src/ipc_message.cc
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include "ara/ipc/ipc_message.h"

#include <string.h>
#include <sys/time.h>

#include "ara/ipc/ipc_logger.h"
#include "ara/ipc/ipc_utilites.h"

namespace ara {
namespace ipc {
extern IpcLogger logger;

IPCMessage::IPCMessage(IPCMessageModeType mode) : data_{0} {
  Initialize();
  (void)encode_header(0U, ara::ipc::utilites::get_timestamp(), mode);
  logger.LogVerbose() << "IPCMessage()";
}
IPCMessage::~IPCMessage() {
  logger.LogVerbose() << "~IPCMessage(): delete data";
  Deinitialize();
};

void IPCMessage::Initialize() {
  payload_size_ = data_;
  timestamp_ = data_ + payload_size_length;
  ipc_mode_ = data_ + payload_size_length + timestamp_length;
  payload_ = data_ + header_length;
}

void IPCMessage::Deinitialize() {
  payload_size_ = nullptr;
  timestamp_ = nullptr;
  ipc_mode_ = nullptr;
  payload_ = nullptr;
}

// copy constructor
IPCMessage::IPCMessage(const IPCMessage& msg) {
  if (msg.payload_size_ == nullptr) {
    // data_ = nullptr;
    memset(data_, 0U, header_length + max_body_length);
    Deinitialize();
  } else {
    Initialize();
    memcpy(data_, msg.data_, header_length + msg.payload_length());
  }
  logger.LogVerbose() << "IPCMessage(const IPCMessage& msg): data_:" << data_;
}

// move constructor
IPCMessage::IPCMessage(IPCMessage&& msg) {
  if (&msg != this) {
    if (msg.payload_size_ == nullptr) {
      memset(data_, 0U, header_length + max_body_length);
      Deinitialize();
    } else {
      Initialize();
      memcpy(data_, msg.data_, header_length + msg.payload_length());
    }
    msg.Deinitialize();
  }
  logger.LogVerbose() << "IPCMessage(IPCMessage&& msg) data_:" << data_;
}

IPCMessage& IPCMessage::operator=(const IPCMessage& msg) {
  if (msg.payload_size_ == nullptr) {
    // data_ = nullptr;
    memset(data_, 0U, header_length + max_body_length);
    Deinitialize();
  } else {
    Initialize();
    memcpy(data_, msg.data_, header_length + msg.payload_length());
  }
  logger.LogVerbose() << "IPCMessage::operator=(const IPCMessage& msg) data_:"
                      << data_;
  return *this;
}

IPCMessage& IPCMessage::operator=(IPCMessage&& msg) {
  if (&msg != this) {
    if (msg.payload_size_ == nullptr) {
      // data_ = nullptr;
      memset(data_, 0U, header_length + max_body_length);
      Deinitialize();
    } else {
      Initialize();
      memcpy(data_, msg.data_, header_length + msg.payload_length());
    }
    // msg.data_ = nullptr;
    msg.Deinitialize();
  }
  logger.LogVerbose() << "IPCMessage::operator=(IPCMessage&& msg) data_:"
                      << data_;
  return *this;
}

bool IPCMessage::encode_header(const uint32_t& size, const uint64_t& ts,
                               const IPCMessageModeType& mode) {
  if (payload_size_ == nullptr) {
    return false;
  }
  memcpy(data_, &size, sizeof(size));  // copy payload size
  memcpy(static_cast<void*>(static_cast<char*>(data_) + payload_size_length),
         &ts,
         timestamp_length);  // copy timestamp
  memcpy(static_cast<void*>(static_cast<char*>(data_) + payload_size_length +
                            timestamp_length),
         &mode,
         sizeof(mode));  // copy mode
  return true;
}

bool IPCMessage::decode_header(uint32_t& size, uint64_t& ts,
                               IPCMessageModeType& mode) const {
  if (payload_size_ == nullptr) {
    return false;
  }
  size = ara::ipc::utilites::bytes_to_uint(data_);
  ts = ara::ipc::utilites::bytes_to_ulong(data_ + payload_size_length);
  mode = static_cast<IPCMessageModeType>(
      *(data_ + payload_size_length + timestamp_length));

  return true;
}

IPCMessage::IPCMessage(const uint32_t& size, const void* data,
                       IPCMessageModeType mode)
    : data_{0} {
  Initialize();
  if (size > max_body_length) {
    logger.LogWarn() << "Message size is too long. Bytes over"
                     << max_body_length << "will be discarded.";
    (void)encode_header(max_body_length, ara::ipc::utilites::get_timestamp(),
                        mode);
    memcpy(payload_, data, max_body_length);
  } else {
    (void)encode_header(size, ara::ipc::utilites::get_timestamp(), mode);
    memcpy(payload_, data, size);
  }
  logger.LogVerbose() << "IPCMessage(const uint32_t& size, const void* data)";
}

const char* IPCMessage::header() const {
  return static_cast<const char*>(data_);
}
char* IPCMessage::header() { return static_cast<char*>(data_); }

char* IPCMessage::payload() { return static_cast<char*>(payload_); }
uint32_t IPCMessage::payload_length() const {
  uint32_t size = 0U;
  uint64_t ts = 0U;
  IPCMessageModeType mode = IPCMessageMode::UNKNOWN;
  decode_header(size, ts, mode);
  return size;
}
const char* IPCMessage::payload() const {
  return static_cast<const char*>(payload_);
}
uint64_t IPCMessage::timestamp() const {
  uint32_t size = 0U;
  uint64_t ts = 0U;
  IPCMessageModeType mode = IPCMessageMode::UNKNOWN;
  decode_header(size, ts, mode);
  return ts;
}

IPCMessageModeType IPCMessage::ipc_mode() const {
  uint32_t size = 0U;
  uint64_t ts = 0U;
  IPCMessageModeType mode = IPCMessageMode::UNKNOWN;
  decode_header(size, ts, mode);
  return mode;
}

String IPCMessage::timestamp_format() {
  return ara::ipc::utilites::get_Datetime(timestamp());
}

}  // namespace ipc
}  // namespace ara
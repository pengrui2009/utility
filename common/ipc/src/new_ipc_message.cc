/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2023-02-27 11:27:01
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-28 14:04:21
 * @Description:
 *
 * Copyright (c) 2023 by Tusimple, All Rights Reserved.
 */
#include "ara/ipc/new_ipc_message.h"

#include <sys/time.h>

#include "ara/ipc/ipc_logger.h"
#include "ara/ipc/ipc_utilites.h"

namespace ara {
namespace ipc {
extern IpcLogger logger;

typedef struct {
  uint64_t payload_size;
  uint64_t timestamp;
  uint8_t ipc_mode;

  void Deserialize(const char* header_buf) {
    if (header_buf != nullptr) {
      payload_size = ara::ipc::utilites::bytes_to_ulong(
          static_cast<const char*>(header_buf));
      timestamp = ara::ipc::utilites::bytes_to_ulong(
          static_cast<const char*>(header_buf + sizeof(payload_size)));
      ipc_mode = static_cast<uint8_t>(
          *(header_buf + sizeof(payload_size) + sizeof(timestamp)));
    }
  };

  void Serialize(char* header_buf) {
    if (header_buf != nullptr) {
      std::memcpy(static_cast<void*>(header_buf), &payload_size,
                  sizeof(payload_size));  // copy payload size
      std::memcpy(static_cast<void*>(header_buf + sizeof(payload_size)),
                  &timestamp,
                  sizeof(timestamp));  // copy timestamp
      std::memcpy(static_cast<void*>(header_buf + sizeof(payload_size) +
                                     sizeof(timestamp)),
                  &ipc_mode,
                  sizeof(ipc_mode));  // copy ipc_mode
    }
  };

} Header;

class NewIpcMessage::pImpl {
 private:
  char header_buf_[header_length] = {0};
  std::allocator<char> alloc_;
  void* payload_ = nullptr;
  uint64_t memory_buffer_size_ = 0U;
  Header header_{0, 0, static_cast<uint8_t>(IPCMessageMode::RPC)};

 public:
  pImpl(const std::size_t& size, const void* data,
        const IPCMessageModeType& type)
      : alloc_(std::allocator<char>()),
        payload_(static_cast<void*>(alloc_.allocate(size))),
        memory_buffer_size_(0U),
        header_{size, ara::ipc::utilites::get_timestamp(),
                static_cast<uint8_t>(type)} {
    encode_header();
    std::memcpy(payload_, data, size);
  }

  pImpl(const IPCMessageModeType& type)
      : alloc_(std::allocator<char>()),
        payload_(nullptr),
        memory_buffer_size_(0U),
        header_{0, ara::ipc::utilites::get_timestamp(),
                static_cast<uint8_t>(type)} {
    encode_header();
  }

  pImpl() : pImpl(IPCMessageMode::RPC) {}

  pImpl(const pImpl& rhs) {
    if (this == &rhs) return;
    if (this->payload_ != nullptr) {
      alloc_.deallocate(static_cast<char*>(this->payload_),
                        header_.payload_size);
    }
    this->header_ = rhs.header_;
    std::memcpy(this->header_buf_, rhs.header_buf_, header_length);
    this->alloc_ = rhs.alloc_;
    this->payload_ = static_cast<void*>(alloc_.allocate(header_.payload_size));
    std::memcpy(this->payload_, rhs.payload_, header_.payload_size);
  }

  pImpl(pImpl&& rhs) = delete;

  pImpl& operator=(const pImpl& rhs) {
    if (this == &rhs) return *this;
    if (this->payload_ != nullptr) {
      alloc_.deallocate(static_cast<char*>(this->payload_),
                        header_.payload_size);
    }
    this->header_ = rhs.header_;
    std::memcpy(this->header_buf_, rhs.header_buf_, header_length);
    this->alloc_ = rhs.alloc_;
    this->payload_ = static_cast<void*>(alloc_.allocate(header_.payload_size));
    std::memcpy(this->payload_, rhs.payload_, header_.payload_size);
    return *this;
  }

  pImpl& operator=(pImpl&& rhs) = delete;

  ~pImpl() {
    if (payload_ != nullptr) {
      alloc_.deallocate(static_cast<char*>(payload_), header_.payload_size);
      payload_ = nullptr;
    }
  }

  void encode_header() { header_.Serialize(header_buf_); }

  void decode_header(std::size_t& size, uint64_t& ts, uint8_t& mode) {
    decode_header();
    size = header_.payload_size;
    ts = header_.timestamp;
    mode = header_.ipc_mode;
  }

  void decode_header() {
    header_.Deserialize(header_buf_);
    resize(header_.payload_size);
  }

  bool realloc(const std::size_t& new_size) {
    if (new_size > memory_buffer_size_) {
      return true;
    } else if (new_size < memory_buffer_size_) {
      // NOTE: Here, we easily don't reallocate memory when the new size is
      // almost same with memory_buffer_size_ to improve performence.
      if (memory_buffer_size_ - new_size >= 1024U) {
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  }

  void resize(std::size_t new_size) {
    if (realloc(new_size)) {
      auto new_data = static_cast<void*>(alloc_.allocate(new_size));
      if (payload_ != nullptr) {
        alloc_.deallocate(static_cast<char*>(payload_), memory_buffer_size_);
      }
      payload_ = new_data;
      memory_buffer_size_ = new_size;
    }
  };

  char* payload() { return static_cast<char*>(payload_); }

  const char* payload() const { return static_cast<const char*>(payload_); }

  uint64_t timestamp() const { return header_.timestamp; }

  std::size_t payload_length() { return header_.payload_size; }

  uint8_t ipc_mode() { return header_.ipc_mode; }

  char* header() { return header_buf_; }

  const char* header() const { return static_cast<const char*>(header_buf_); }
};

NewIpcMessage::NewIpcMessage(const std::size_t& size, const void* data,
                             IPCMessageModeType type)
    : pImpl_(std::make_unique<pImpl>(size, data, type)) {}

NewIpcMessage::NewIpcMessage(IPCMessageModeType type)
    : pImpl_(std::make_unique<pImpl>(type)) {}

NewIpcMessage::NewIpcMessage(const NewIpcMessage& rhs)
    : pImpl_(std::make_unique<pImpl>()) {
  if (this == &rhs) return;
  *pImpl_ = *rhs.pImpl_;
}

NewIpcMessage::NewIpcMessage(NewIpcMessage&& rhs)
    : pImpl_(std::make_unique<pImpl>()) {
  if (this == &rhs) return;
  pImpl_ = std::move(rhs.pImpl_);
}

NewIpcMessage& NewIpcMessage::operator=(const NewIpcMessage& rhs) {
  if (this == &rhs) return *this;
  *pImpl_ = *rhs.pImpl_;
  return *this;
}

NewIpcMessage& NewIpcMessage::operator=(NewIpcMessage&& rhs) {
  if (this == &rhs) return *this;
  pImpl_ = std::move(rhs.pImpl_);
  return *this;
}

NewIpcMessage::~NewIpcMessage() {}

bool NewIpcMessage::decode_header() const {
  pImpl_->decode_header();
  return true;
}

void NewIpcMessage::resize(std::size_t new_size) {
  if (pImpl_ != nullptr) {
    pImpl_->resize(new_size);
  }
}

char* NewIpcMessage::header() {
  return pImpl_ != nullptr ? pImpl_->header() : nullptr;
}

const char* NewIpcMessage::header() const {
  return pImpl_ != nullptr ? pImpl_->header() : nullptr;
}

char* NewIpcMessage::payload() {
  return pImpl_ != nullptr ? pImpl_->payload() : nullptr;
}

const char* NewIpcMessage::payload() const {
  return pImpl_ != nullptr ? pImpl_->payload() : nullptr;
}

std::size_t NewIpcMessage::payload_length() const {
  return pImpl_ != nullptr ? pImpl_->payload_length() : 0;
}

IPCMessageModeType NewIpcMessage::ipc_mode() const {
  return pImpl_ != nullptr ? static_cast<IPCMessageModeType>(pImpl_->ipc_mode())
                           : IPCMessageModeType::UNKNOWN;
}

uint64_t NewIpcMessage::timestamp() const {
  return pImpl_ != nullptr ? pImpl_->timestamp() : 0;
}

String NewIpcMessage::timestamp_format() {
  return pImpl_ != nullptr ? ara::ipc::utilites::get_Datetime(timestamp()) : "";
}

}  // namespace ipc
}  // namespace ara
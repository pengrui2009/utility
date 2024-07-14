#pragma once

#include <ara/core/string.h>

#include <memory>
#include <string>

namespace ara {
namespace ipc {

enum class IPCMessageMode : std::uint8_t {
  UNKNOWN = 0U,
  FIRE_AND_FORGET = 1U,
  RPC = 2U
};

using String = ara::core::String;
using IPCMessageModeType = IPCMessageMode;

class BaseIPCMessage {
 public:
  BaseIPCMessage() = default;
  virtual ~BaseIPCMessage() = default;
  virtual const char* header() const = 0;
  virtual char* header() = 0;
  virtual char* payload() = 0;
  virtual const char* payload() const = 0;
  virtual uint64_t timestamp() const = 0;
  virtual IPCMessageModeType ipc_mode() const = 0;
  virtual String timestamp_format() = 0;
};

}  // namespace ipc
}  // namespace ara
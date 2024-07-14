#ifndef TUSIMPLEAP_ARA_CORE_INITIALIZATION_H_
#define TUSIMPLEAP_ARA_CORE_INITIALIZATION_H_

#include "ara/core/result.h"
namespace ara {
namespace com {
ara::core::Result<void> apComInit();
ara::core::Result<void> apComDeInit();
namespace shutdown {
const bool apGetShutdown();
}  // namespace shutdown
}  // namespace com
}  // namespace ara

namespace ara {
namespace log {
ara::core::Result<void> Initialize();
}
}

namespace ara {
namespace core {

inline Result<void> Initialize() noexcept {
  {
    auto res = ara::log::Initialize();
    if (!res) { return res; }
  }
  {
    auto res = ara::com::apComInit();
    if (!res) { return res; }
  }
  return Result<void>();
}

inline Result<void> Deinitialize() noexcept {
  {
    auto res = ara::com::apComDeInit();
    if (!res) { return res; }
  }
  return Result<void>();
}

}  // namespace core
}  // namespace ara

#endif

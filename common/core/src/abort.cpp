#include "ara/core/abort.h"

namespace ara {
namespace core {

namespace internal {
AbortHandler g_abort_handler = nullptr;
}

// @uptrace{SWS_CORE_00051}
AbortHandler SetAbortHandler(AbortHandler handler) noexcept {
  ara::core::internal::g_abort_handler = handler;
  return ara::core::internal::g_abort_handler;
}

// @uptrace{SWS_CORE_00052}
void Abort(const char* text) noexcept {
  std::cerr << text << std::endl;
  if (ara::core::internal::g_abort_handler != 0) {
    ara::core::internal::g_abort_handler();
  }
  std::abort();
}

}  // namespace core
}  // namespace ara

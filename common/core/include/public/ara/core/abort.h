/// @file
/// @brief Interface to ara::core::abort

#ifndef TUSIMPLEAP_ARA_CORE_ABORT_H_
#define TUSIMPLEAP_ARA_CORE_ABORT_H_
#include <cstdlib>
#include <functional>
#include <iostream>

namespace ara {
namespace core {

using AbortHandler = std::function<void(void)>;
namespace internal {
extern AbortHandler g_abort_handler;
}

// @uptrace{SWS_CORE_00051}
AbortHandler SetAbortHandler(AbortHandler handler) noexcept;

// @uptrace{SWS_CORE_00052}
void Abort(const char* text) noexcept;

}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_ABORT_H_

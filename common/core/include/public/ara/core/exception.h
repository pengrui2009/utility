

#ifndef TUSIMPLEAP_ARA_CORE_EXCEPTIONS_H
#define TUSIMPLEAP_ARA_CORE_EXCEPTIONS_H

#include <exception>
#include <ostream>
#include <system_error>
#include <utility>

#include "ara/core/error_code.h"

namespace ara {
namespace core {

/// @uptrace{SWS_CORE_00601}
class Exception : public std::exception {
  ErrorCode mErrorCode;

public:
  /// @uptrace{SWS_CORE_00611}
  explicit Exception(ErrorCode err) noexcept : std::exception(), mErrorCode(std::move(err)) {}

  Exception(const Exception& other) noexcept = default;

  Exception() = default;

  virtual ~Exception() = default;

  /// @uptrace{SWS_CORE_00613}
  const ErrorCode& Error() const noexcept { return mErrorCode; }

  /// @uptrace{SWS_CORE_00612}
  const char* what() const noexcept final { return std::exception::what(); }

  /// @uptrace{SWS_CORE_00614}
  Exception& operator=(const Exception& other) & = default;
};

}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_EXCEPTIONS_H

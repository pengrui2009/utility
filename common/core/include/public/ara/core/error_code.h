

#ifndef TUSIMPLEAP_ARA_CORE_ERROR_CODE_H_
#define TUSIMPLEAP_ARA_CORE_ERROR_CODE_H_

#include <cstdint>
#include <ostream>

#include "ara/core/error_domain.h"
#include "ara/core/string_view.h"

namespace ara {
namespace core {

/// @uptrace{SWS_CORE_00501}
class ErrorCode final {
public:
  /// @uptrace{SWS_CORE_00512}
  template <typename EnumT, typename = typename std::enable_if<std::is_enum<EnumT>::value>::type>
  constexpr ErrorCode(EnumT e, ErrorDomain::SupportDataType data = ErrorDomain::SupportDataType()) noexcept
      // Call MakeErrorCode() unqualified, so the correct overload is found
      // via ADL.
      :
      ErrorCode(MakeErrorCode(e, data)) {}

  /// @uptrace{SWS_CORE_00513}
  constexpr ErrorCode(ErrorDomain::CodeType value,
                      const ErrorDomain& domain,
                      ErrorDomain::SupportDataType data = ErrorDomain::SupportDataType()) noexcept :
      mValue(value), mDomain(&domain), mSupportData(data) {}

  /// @uptrace{SWS_CORE_00514}
  constexpr ErrorDomain::CodeType Value() const noexcept { return mValue; }

  /// @uptrace{SWS_CORE_00516}
  constexpr ErrorDomain::SupportDataType SupportData() const noexcept { return mSupportData; }

  /// @uptrace{SWS_CORE_00515}
  constexpr const ErrorDomain& Domain() const noexcept { return *mDomain; }

  /// @uptrace{SWS_CORE_00518}
  StringView Message() const noexcept { return Domain().Message(Value()); }

  /// @uptrace{SWS_CORE_00519}
  void ThrowAsException() const { Domain().ThrowAsException(*this); }

  ErrorCode() = default;

private:
  ErrorDomain::CodeType mValue;
  const ErrorDomain* mDomain;
  ErrorDomain::SupportDataType mSupportData;
  // non-owning pointer to the associated ErrorDomain
};
// @uptrace{SWS_CORE_00571}
constexpr inline bool operator==(const ErrorCode& lhs, const ErrorCode& rhs) {
  return ((lhs.Domain() == rhs.Domain()) && (lhs.Value() == rhs.Value()));
}
// @uptrace{SWS_CORE_00572}
constexpr inline bool operator!=(const ErrorCode& lhs, const ErrorCode& rhs) {
  return !(lhs == rhs);
}

template <typename ExceptionType>
void ThrowOrTerminate(ErrorCode errorCode) {
#ifndef ARA_NO_EXCEPTIONS
  throw ExceptionType(std::move(errorCode));
#else
  (void)errorCode;
  std::terminate();
#endif
}
}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_ERROR_CODE_H_



#ifndef TUSIMPLEAP_ARA_CORE_ERROR_DOMAIN_H_
#define TUSIMPLEAP_ARA_CORE_ERROR_DOMAIN_H_

#include <cstdint>
#include <exception>
#include <iostream>

namespace ara {
namespace core {

// forward declaration
class ErrorCode;

/// @uptrace{SWS_CORE_00110}
class ErrorDomain {
public:
  /// @uptrace{SWS_CORE_00121}
  using IdType = std::uint64_t;
  /// @uptrace{SWS_CORE_00122}
  using CodeType = std::int32_t;
  /// @uptrace{SWS_CORE_00123}
  using SupportDataType = std::int32_t;

  /// @uptrace{SWS_CORE_00131}
  ErrorDomain(const ErrorDomain& err) = delete;
  /// @uptrace{SWS_CORE_00132}
  ErrorDomain(ErrorDomain&& err) = delete;
  /// @uptrace{SWS_CORE_00133}
  ErrorDomain& operator=(const ErrorDomain& err) & = delete;
  /// @uptrace{SWS_CORE_00134}
  ErrorDomain& operator=(ErrorDomain&& err) & = delete;

  /// @uptrace{SWS_CORE_00152}
  virtual const char* Name() const noexcept = 0;

  /// @uptrace{SWS_CORE_00153}
  virtual const char* Message(CodeType errorCode) const noexcept = 0;

  /**
   * @brief Throws the given errorCode as Exception
   * @param errorCode error code to be thrown
   * @remark if ARA_NO_EXCEPTIONS is defined, this function call will
   * terminate.
   * @uptrace{SWS_CORE_00154}
   */
  /// @uptrace{SWS_CORE_00151}
  constexpr IdType Id() const noexcept { return mId; }

  virtual void ThrowAsException(const ErrorCode& errorCode) const noexcept(false) = 0;

protected:
  /// @uptrace{SWS_CORE_00135}
  constexpr explicit ErrorDomain(IdType id) noexcept : mId(id) {}

  /// @brief Destructor
  ///
  /// This dtor is non-virtual (and trivial) so that this class is a literal
  /// type. While we do have virtual functions, we never need polymorphic
  /// destruction.
  ///
  /// @uptrace{SWS_CORE_00136}
  ~ErrorDomain() = default;

private:
  const IdType mId;
};

/// @uptrace{SWS_CORE_00137}
constexpr bool operator==(const ErrorDomain& lhs, const ErrorDomain& rhs) {
  return lhs.Id() == rhs.Id();
}

/// @uptrace{SWS_CORE_00138}
constexpr bool operator!=(const ErrorDomain& lhs, const ErrorDomain& rhs) {
  return lhs.Id() != rhs.Id();
}

}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_ERROR_DOMAIN_H_

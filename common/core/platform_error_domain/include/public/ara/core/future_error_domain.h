

#ifndef TUSIMPLEAP_ARA_CORE_FUTURE_ERROR_DOMAIN_H_
#define TUSIMPLEAP_ARA_CORE_FUTURE_ERROR_DOMAIN_H_

#include <cstdint>

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"

namespace ara {
namespace core {

/**
 * @brief Specifies the types of internal errors that can occur upon calling
 * Future::get or Future::GetResult.
 *
 * These definitions are equivalent to the ones from std::future_errc.
 *
 * @uptrace{SWS_CORE_00400}
 */
enum class future_errc : int32_t {
  broken_promise = 101,             ///< the asynchronous task abandoned its shared state
  future_already_retrieved = 102,   ///< the contents of the shared state were already accessed
  promise_already_satisfied = 103,  ///< attempt to store a value into the shared state twice
  no_state = 104,                   ///< attempt to access Promise or Future without an
                                    ///< associated state
};

/**
 * @brief Exception type thrown by Future and Promise classes.
 *
 * @uptrace{SWS_CORE_00411}
 */
class FutureException final : public Exception {
public:
  /**
   * @brief Construct a new FutureException from an ErrorCode.
   * @param err  the ErrorCode
   *
   * @uptrace{SWS_CORE_00412}
   */
  explicit FutureException(ErrorCode err) noexcept : Exception(std::move(err)) {}
};

/**
 * @brief Error domain for errors originating from classes Future and Promise.
 *
 * @uptrace{SWS_CORE_00421}
 */
class FutureErrorDomain final : public ErrorDomain {
  constexpr static ErrorDomain::IdType kId = 0x8000000000000013;

public:
  /// @brief Alias for the error code value enumeration
  ///
  /// @uptrace{SWS_CORE_00431}
  using Errc = future_errc;

  /// @brief Alias for the exception base class
  ///
  /// @uptrace{SWS_CORE_00432}
  using Exception = FutureException;

  /// @brief Default constructor
  ///
  /// @uptrace{SWS_CORE_00441}
  /// @uptrace{SWS_CORE_00013}
  constexpr FutureErrorDomain() noexcept : ErrorDomain(kId) {}

  /// @brief Return the "shortname" ApApplicationErrorDomain.SN of this error
  /// domain.
  /// @returns "Future"
  ///
  /// @uptrace{SWS_CORE_00442}
  /// @uptrace{SWS_CORE_00013}
  const char* Name() const noexcept final { return "Future"; }

  /// @brief Translate an error code value into a text message.
  /// @param errorCode  the error code value
  /// @returns the text message, never nullptr
  ///
  /// @uptrace{SWS_CORE_00443}
  const char* Message(ErrorDomain::CodeType errorCode) const noexcept final {
    switch (static_cast<Errc>(errorCode)) {
      case Errc::broken_promise: {
        return "broken promise";
        break;
      }
      case Errc::future_already_retrieved: {
        return "future already retrieved";
        break;
      }
      case Errc::promise_already_satisfied: {
        return "promise already satisfied";
        break;
      }
      case Errc::no_state: {
        return "no state associated with this future";
        break;
      }
      default: {
        return "unknown future error";
        break;
      }
    }
  }

  /// @brief Throw the exception type corresponding to the given ErrorCode.
  /// @param errorCode  the ErrorCode instance
  ///
  /// @uptrace{SWS_CORE_00444}
  void ThrowAsException(const ErrorCode& errorCode) const final { ThrowOrTerminate<Exception>(errorCode); }
};

namespace internal {
constexpr FutureErrorDomain g_futureErrorDomain;
}

/**
 * @brief Obtain the reference to the single global FutureErrorDomain instance.
 * @returns reference to the FutureErrorDomain instance
 *
 * @uptrace{SWS_CORE_00480}
 */
inline constexpr const ErrorDomain& GetFutureErrorDomain() noexcept {
  return internal::g_futureErrorDomain;
}

/**
 * @brief Create a new ErrorCode for FutureErrorDomain with the given support
 * data type and message.
 *
 * The lifetime of the text pointed to by @a message must exceed that
 * of all uses of ErrorCode::UserMessage() on the new instance.
 *
 * @param code  an enumeration value from future_errc
 * @param data  a vendor-defined supplementary value
 * @param message  a user-defined context message (can be nullptr)
 * @returns the new ErrorCode instance
 *
 * @uptrace{SWS_CORE_00490}
 */
inline constexpr ErrorCode MakeErrorCode(future_errc code, ErrorDomain::SupportDataType data) noexcept {
  return ErrorCode(static_cast<ErrorDomain::CodeType>(code), GetFutureErrorDomain(), data);
}

enum class future_status : std::uint8_t {
  ready,
  timeout,
};

}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_FUTURE_ERROR_DOMAIN_H_

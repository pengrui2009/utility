
#ifndef TUSIMPLEAP_ARA_SM__SM_ERROR_DOMAIN_H_
#define TUSIMPLEAP_ARA_SM__SM_ERROR_DOMAIN_H_

#include <ara/core/error_code.h>
#include <ara/core/error_domain.h>
#include <ara/core/exception.h>

namespace ara {
namespace sm {

enum class SMErrc : ara::core::ErrorDomain::CodeType {
  kUndefineFunctionGroup = 0,
  kUndefineFunctionGroupState = 1,
  kCancelled = 2,
  kServiceNotAvailable = 3,
  kInvalidParameters = 4,
  kRejected = 5,
  kVerifyFailed = 6, 
  kPrepareFailed = 7, 
  kRollbackFailed = 8,
  kNotAllowedMultipleUpdateSessions = 9,
  kPrepareFailedBecauseFgNonOff = 0xFF01, 
};

class SMException final : public ara::core::Exception {
 public:
  explicit SMException(ara::core::ErrorCode code) noexcept
      : ara::core::Exception(std::move(code)) {}
};

class SMErrorDomain final : public ara::core::ErrorDomain {
 public:
  constexpr static ErrorDomain::IdType kId = 0x8000000000000203;

  constexpr SMErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}

  const char* Name() const noexcept override {
    return "State";
  }

  const char* Message(ara::core::ErrorDomain::CodeType code) const noexcept override {
    switch (static_cast<SMErrc>(code)) {
      case SMErrc::kUndefineFunctionGroup: {
        return "UndefineFunctionGroup";
      }

      case SMErrc::kUndefineFunctionGroupState: {
        return "UndefineFunctionGroupState";
      }

      case SMErrc::kCancelled: {
        return "Cancelled";
      }

      case SMErrc::kServiceNotAvailable: {
        return "ServiceNotAvailable";
      }

      case SMErrc::kInvalidParameters: {
        return "InvalidParameters";
      }

      case SMErrc::kRejected: {
        return "Requested operation was rejected";
      }

      case SMErrc::kVerifyFailed: {
        return "Verification step of update failed";
      }

      case SMErrc::kPrepareFailed: {
        return "Preparation step of update failed";
      }

      case SMErrc::kRollbackFailed: {
        return "Rollback step of update failed";
      }

      case SMErrc::kNotAllowedMultipleUpdateSessions: {
        return "Only single update session is allowed";
      }

      case SMErrc::kPrepareFailedBecauseFgNonOff: {
        return "Pre update failed because the functional group is not in Off state";
      }

      default: {
        return "UnknownError";
      }
    };
  }

  void ThrowAsException(const ara::core::ErrorCode &code) const noexcept(false) override {
    ara::core::ThrowOrTerminate<ara::core::Exception>(code);
  }
};

namespace internal {
constexpr SMErrorDomain g_sm_error_domain;
} // namespace internal

inline constexpr const ara::core::ErrorDomain& GetSMErrorDomain() noexcept {
  return internal::g_sm_error_domain;
}

inline ara::core::ErrorCode MakeErrorCode(SMErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode(static_cast<ara::core::ErrorDomain::CodeType>(code),
                              GetSMErrorDomain(),
                              data);
}

} // namespace sm
} // namespace ara

#endif // TUSIMPLEAP_ARA_SM__SM_ERROR_DOMAIN_H_
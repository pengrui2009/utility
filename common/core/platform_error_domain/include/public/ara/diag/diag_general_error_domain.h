/*
 * @Author: Dongdong Sun dongdong.sun@tusen.ai
 * @Date: 2023-02-26 07:17:15
 * @LastEditors: Sun Dongdong dongdong.sun@tusen.ai
 * @LastEditTime: 2023-03-15 14:55:40
 * @FilePath: /adas-monorepo/modules/adaptive_autosar/ara-api/diag/api/include/private/ara/diag/diag_general_error_domain.h
 * @Description:
 */
#ifndef TUSIMPLEAP_ARA_DIAG_DIAG_GENERAL_ERROR_DOMAIN_H_
#define TUSIMPLEAP_ARA_DIAG_DIAG_GENERAL_ERROR_DOMAIN_H_

#include <ara/core/error_code.h>
#include <ara/core/error_domain.h>
#include <ara/core/exception.h>

#include "ara/diag/diag_error_domain.h"

namespace ara {
namespace diag {

enum class DiagGeneralErrc : ara::core::ErrorDomain::CodeType {
  kNoServiceInstance = 0xA0,
  kNoPendingPromise,
  kNoMethod,
  kNoConversation,
  kSockIoError,
  kManifestError,
  kResponseError,
  kAlreadyRegistered
};

class DiagGeneralErrorDomain final : public ara::core::ErrorDomain {
 public:
  using Errc = ara::diag::DiagGeneralErrc;
  using Exception = ara::diag::DiagException;

  constexpr static ErrorDomain::IdType kId = 0x8000000000000404;
  constexpr DiagGeneralErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}

  const char* Name() const noexcept override {
    return "DiagGeneral";
  }

  const char* Message(ara::core::ErrorDomain::CodeType code) const noexcept override {
    switch (static_cast<Errc>(code)) {
      case Errc::kNoServiceInstance: {
        return "NoServiceInstance";
      }

      case Errc::kNoPendingPromise: {
        return "NoPendingPromise";
      }

      case Errc::kNoMethod: {
        return "NoMethod";
      }

      case Errc::kNoConversation: {
        return "NoConversation";
      }

      case Errc::kSockIoError: {
        return "SockIoError";
      }

      case Errc::kManifestError: {
        return "ManifestError";
      }

      case Errc::kResponseError: {
        return "ResponseError";
      }

      case Errc::kAlreadyRegistered: {
        return "AlreadyRegistered";
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
constexpr DiagGeneralErrorDomain g_diag_general_error_domain;
} // namespace internal

inline constexpr const ara::core::ErrorDomain& GetDiagGeneralDomain() noexcept {
  return internal::g_diag_general_error_domain;
}

inline constexpr ara::core::ErrorCode MakeErrorCode(DiagGeneralErrc code,
                                                    ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode(static_cast<ara::core::ErrorDomain::CodeType>(code),
                              GetDiagGeneralDomain(),
                              data);
}

} // namespace diag
} // namespace ara

#endif // TUSIMPLEAP_ARA_DIAG_DIAG_GENERAL_ERROR_DOMAIN_H_
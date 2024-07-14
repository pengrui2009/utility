/*** 
 * @Author: Sun Dongdong dongdong.sun@tusen.ai
 * @Date: 2022-12-05 18:34:00
 * @LastEditors: Sun Dongdong dongdong.sun@tusen.ai
 * @LastEditTime: 2022-12-17 22:54:09
 * @FilePath: /adas-monorepo/modules/adaptive_autosar/ara-api/phm/ara-api-phm/include/public/ara/phm/phm_error_domain.h
 * @Description: 
 * @
 * @Copyright (c) 2022 by Tusen, All Rights Reserved. 
 */

#ifndef TUSIMPLEAP_ARA_PHM_PHM_ERROR_DOMAIN_H_
#define TUSIMPLEAP_ARA_PHM_PHM_ERROR_DOMAIN_H_

#include <ara/core/error_code.h>
#include <ara/core/error_domain.h>
#include <ara/core/exception.h>

namespace ara {
namespace phm {

enum class PhmErrc : ara::core::ErrorDomain::CodeType {
  kServiceNotAvailable = 1,
  kOfferFailed = 2,
  kCallbackRegistered = 3,
  kInstanceNoAvailable = 4,
  kConnectedFailed = 5,
  kPendingPreviousRequest = 6,
  kExpired,
};

class PhmException final : public ara::core::Exception {
 public:
  explicit PhmException(ara::core::ErrorCode code) noexcept
      : ara::core::Exception(std::move(code)) {}
};

class PhmErrorDomain final : public ara::core::ErrorDomain {
 public:
  constexpr static ErrorDomain::IdType kId = 0x8000000000000602;

  constexpr PhmErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}

  const char* Name() const noexcept override {
    return "Phm";
  }

  const char* Message(ara::core::ErrorDomain::CodeType code) const noexcept override {
    switch (static_cast<PhmErrc>(code)) {
      case PhmErrc::kServiceNotAvailable: {
        return "ServiceNotAvailable";
      }

      case PhmErrc::kOfferFailed: {
        return "OfferFailed";
      }

      case PhmErrc::kCallbackRegistered: {
        return "CallbackRegistered";
      }

      case PhmErrc::kInstanceNoAvailable: {
        return "InstanceNoAvailable";
      }

      case PhmErrc::kConnectedFailed: {
        return "ConnectedFailed";
      }

      case PhmErrc::kPendingPreviousRequest: {
        return "PendingPreviousRequest";
      }

      case PhmErrc::kExpired: {
        return "kExpired";
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
constexpr PhmErrorDomain g_phm_error_domain;
} // namespace internal

inline constexpr const ara::core::ErrorDomain& GetPHMErrorDomain() noexcept {
  return internal::g_phm_error_domain;
}

inline ara::core::ErrorCode MakeErrorCode(PhmErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode(static_cast<ara::core::ErrorDomain::CodeType>(code),
                              GetPHMErrorDomain(),
                              data);
}

} // namespace phm
} // namespace ara

#endif // TUSIMPLEAP_ARA_PHM_PHM_ERROR_DOMAIN_H_
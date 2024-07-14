/*
 * @Author: Dongdong Sun dongdong.sun@tusen.ai
 * @Date: 2023-02-17 03:01:31
 * @LastEditors: Dongdong Sun dongdong.sun@tusen.ai
 * @LastEditTime: 2023-02-24 15:45:49
 * @FilePath: /adas-monorepo/modules/adaptive_autosar/ara-api/diag/api/include/public/ara/diag/diag_error_domain.h
 * @Description:
 */
#ifndef TUSIMPLEAP_ARA_DIAG_DIAG_ERROR_DOMAIN_H_
#define TUSIMPLEAP_ARA_DIAG_DIAG_ERROR_DOMAIN_H_

#include <ara/core/error_code.h>
#include <ara/core/error_domain.h>
#include <ara/core/exception.h>

namespace ara {
namespace diag {

enum class DiagErrc : ara::core::ErrorDomain::CodeType {
  kAlreadyOffered = 101,
  kConfigurationMismatch = 102,
  kDebouncingConfigurationInconsistent = 103,
  kReportIgnored = 104,
  kInvalidArgument = 105,
  kNotOffered = 106,
  kGenericError = 107,
  kNoSuchDTC = 108,
  kBusy = 109,
  kFailed = 110,
  kMemoryError = 111,
  kWrongDtc = 112,
  kRejected = 113,
  kResetTypeNotSupport = 114,
  kRequestFailed = 115,
  kCustomResetTypeNotSupport = 116
};

class DiagException : public ara::core::Exception {
 public:
  explicit DiagException(ara::core::ErrorCode err) noexcept
      : ara::core::Exception(std::move(err)) {}
};

class DiagErrorDomain final : public ara::core::ErrorDomain {
 public:
  using Errc = ara::diag::DiagErrc;
  using Exception = ara::diag::DiagException;

  constexpr static ErrorDomain::IdType kId = 0x8000000000000401;
  constexpr DiagErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}

  const char* Name() const noexcept override {
    return "Diag";
  }

  const char* Message(ara::core::ErrorDomain::CodeType code) const noexcept override {
    switch (static_cast<Errc>(code)) {
      case Errc::kAlreadyOffered: {
        return "AlreadyOffered";
      }

      case Errc::kConfigurationMismatch: {
        return "ConfigurationMismatch";
      }

      case Errc::kDebouncingConfigurationInconsistent: {
        return "DebouncingConfigurationInconsistent";
      }

      case Errc::kReportIgnored: {
        return "ReportIgnored";
      }

      case Errc::kInvalidArgument: {
        return "InvalidArgument";
      }

      case Errc::kNotOffered: {
        return "NotOffered";
      }

      case Errc::kGenericError: {
        return "GenericError";
      }

      case Errc::kNoSuchDTC: {
        return "kNoSuchDTC";
      }

      case Errc::kBusy: {
        return "Busy";
      }

      case Errc::kFailed: {
        return "Failed";
      }

      case Errc::kMemoryError: {
        return "MemoryError";
      }

      case Errc::kWrongDtc: {
        return "kWrongDtc";
      }

      case Errc::kRejected: {
        return "Rejected";
      }

      case Errc::kResetTypeNotSupport: {
        return "ResetTypeNotSupport";
      }

      case Errc::kRequestFailed: {
        return "RequestFailed";
      }

      case Errc::kCustomResetTypeNotSupport: {
        return "CustomResetTypeNotSupport";
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

enum class DiagOfferErrc : ara::core::ErrorDomain::CodeType {
  kAlreadyOffered= 101,
  kConfigurationMismatch= 102,
  kDebouncingConfigurationInconsistent= 103,
};

class DiagOfferErrorDomain final : public ara::core::ErrorDomain {
 public:
  using Errc = ara::diag::DiagOfferErrc;
  using Exception = ara::diag::DiagException;

  constexpr static ErrorDomain::IdType kId = 0x8000000000000403;
  constexpr DiagOfferErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}

  const char* Name() const noexcept override {
    return "DiagOffer";
  }

  const char* Message(ara::core::ErrorDomain::CodeType code) const noexcept override {
    switch (static_cast<Errc>(code)) {
      case Errc::kAlreadyOffered: {
        return "AlreadyOffered";
      }

      case Errc::kConfigurationMismatch: {
        return "ConfigurationMismatch";
      }

      case Errc::kDebouncingConfigurationInconsistent: {
        return "DebouncingConfigurationInconsistent";
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

enum class DiagReportingErrc : ara::core::ErrorDomain::CodeType {
  kAlreadyOffered= 101,
  kConfigurationMismatch= 102,
  kDebouncingConfigurationInconsistent= 103,
  kReportIgnored = 104,
  kInvalidArgument = 105,
  kNotOffered = 106,
  kGenericError = 107,
};

class DiagReportingErrorDomain final : public ara::core::ErrorDomain {
 public:
  using Errc = ara::diag::DiagReportingErrc;
  using Exception = ara::diag::DiagException;

  constexpr static ErrorDomain::IdType kId = 0x8000000000000402;
  constexpr DiagReportingErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}

  const char* Name() const noexcept override {
    return "DiagReporting";
  }

  const char* Message(ara::core::ErrorDomain::CodeType code) const noexcept override {
    switch (static_cast<Errc>(code)) {
      case Errc::kAlreadyOffered: {
        return "AlreadyOffered";
      }

      case Errc::kConfigurationMismatch: {
        return "ConfigurationMismatch";
      }

      case Errc::kDebouncingConfigurationInconsistent: {
        return "DebouncingConfigurationInconsistent";
      }

      case Errc::kReportIgnored: {
        return "ReportIgnored";
      }

      case Errc::kInvalidArgument: {
        return "InvalidArgument";
      }

      case Errc::kNotOffered: {
        return "NotOffered";
      }

      case Errc::kGenericError: {
        return "GenericError";
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
constexpr DiagErrorDomain g_diag_error_domain;
constexpr DiagOfferErrorDomain g_diag_offer_error_domain;
constexpr DiagReportingErrorDomain g_diag_reporting_error_domain;
} // namespace internal

inline constexpr const ara::core::ErrorDomain& GetDiagDomain() noexcept {
  return internal::g_diag_error_domain;
}

inline constexpr const ara::core::ErrorDomain& GetDiagOfferDomain() noexcept {
  return internal::g_diag_offer_error_domain;
}

inline constexpr const ara::core::ErrorDomain& GetDiagReportingDomain() noexcept {
  return internal::g_diag_reporting_error_domain;
}

inline constexpr ara::core::ErrorCode MakeErrorCode(DiagErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode(static_cast<ara::core::ErrorDomain::CodeType>(code),
                              GetDiagDomain(),
                              data);
}

inline constexpr ara::core::ErrorCode MakeErrorCode(DiagOfferErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode(static_cast<ara::core::ErrorDomain::CodeType>(code),
                              GetDiagOfferDomain(),
                              data);
}

inline constexpr ara::core::ErrorCode MakeErrorCode(DiagReportingErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode(static_cast<ara::core::ErrorDomain::CodeType>(code),
                              GetDiagReportingDomain(),
                              data);
}

} // namespace diag
} // namespace ara

#endif // TUSIMPLEAP_ARA_DIAG_DIAG_ERROR_DOMAIN_H_
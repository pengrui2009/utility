/*
 * @Author: Dongdong Sun dongdong.sun@tusen.ai
 * @Date: 2023-02-17 03:01:31
 * @LastEditors: Sun Dongdong dongdong.sun@tusen.ai
 * @LastEditTime: 2023-03-13 17:22:30
 * @FilePath: /adas-monorepo/modules/adaptive_autosar/ara-api/diag/api/include/public/ara/diag/diag_error_domain.h
 * @Description:
 */
#ifndef TUSIMPLEAP_ARA_DIAG_DIAG_UDS_NRC_ERROR_DOMAIN_H_
#define TUSIMPLEAP_ARA_DIAG_DIAG_UDS_NRC_ERROR_DOMAIN_H_

#include <ara/core/error_code.h>
#include <ara/core/error_domain.h>
#include <ara/core/exception.h>

namespace ara {
namespace diag {

enum class DiagUdsNrcErrc : ara::core::ErrorDomain::CodeType {
  kGeneralReject = 0x10,
  kServiceNotSupported = 0x11,
  kSubfunctionNotSupported = 0x12,
  kIncorrectMessageLengthOrInvalid = 0x13,
  kResponseTooLong = 0x14,
  kBusyRepeatRequest = 0x21,
  kConditionsNotCorrect = 0x22,
  kRequestSequenceError = 0x24,
  kNoResponseFromSubnetComponent = 0x25,
  kFailurePreventsExecutionOfRequestedAction = 0x26,
  kRequestOutOfRange = 0x31,
  kSecurityAccessDenied = 0x33,
  kInvalidKey = 0x35,
  kExceedNumberOfAttempts = 0x36,
  kRequiredTimeDelayNotExpired = 0x37,
  kUploadDownloadNotAccepted = 0x70,
  kTransferDataSuspended = 0x71,
  kGeneralProgrammingFailure = 0x72,
  kWrongBlockSequenceCounter = 0x73,
  kRequestCorrectlyReceivedResponsePending = 0x78,
  kSubFunctionNotSupportedInActiveSession = 0x7E,
  kServiceNotSupportedInActiveSession = 0x7F,
  kRpmTooHigh = 0x81,
  kRpmTooLow = 0x82,
  kEngineIsRunning = 0x83,
  kEngineIsNotRunning = 0x84,
  kEngineRunTimeTooLow = 0x85,
  kTemperatureTooHigh = 0x86,
  kTemperatureTooLow = 0x87,
  kVehicleSpeedTooHigh = 0x88,
  kVehicleSpeedTooLow = 0x89,
  kThrottlePedalTooHigh = 0x8A,
  kThrottlePedalTooLow = 0x8B,
  kTransmissionRangeNotInNeutral = 0x8C,
  kTransmissionRangeNotInGear = 0x8D,
  kBrakeSwitchNotClosed = 0x8F,
  kShifterLeverNotInPark = 0x90,
  kTorqueConverterClutchLocked = 0x91,
  kVoltageTooHigh = 0x92,
  kVoltageTooLow = 0x93,
  kResourceTemporarilyNotAvailable = 0x94,
  kNoProcessingNoResponse = 0xFF,
};

class DiagUdsNrcException : public ara::core::Exception {
 public:
  explicit DiagUdsNrcException(ara::core::ErrorCode err) noexcept
      : ara::core::Exception(std::move(err)) {}
};

class DiagUdsNrcErrorDomain final : public ara::core::ErrorDomain {
 public:
  using Errc = ara::diag::DiagUdsNrcErrc;
  using Exception = ara::diag::DiagUdsNrcException;

  constexpr static ErrorDomain::IdType kId = 0x8000000000000411;
  constexpr DiagUdsNrcErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}

  const char* Name() const noexcept override {
    return "DiagUdsNrc";
  }

  const char* Message(ara::core::ErrorDomain::CodeType code) const noexcept override {
    switch (static_cast<Errc>(code)) {
      case Errc::kGeneralReject: {
        return "GeneralReject";
      }

      case Errc::kServiceNotSupported: {
        return "ServiceNotSupported";
      }

      case Errc::kSubfunctionNotSupported: {
        return "SubfunctionNotSupported";
      }

      case Errc::kIncorrectMessageLengthOrInvalid: {
        return "IncorrectMessageLengthOrInvalid";
      }

      case Errc::kResponseTooLong: {
        return "ResponseTooLong";
      }

      case Errc::kBusyRepeatRequest: {
        return "BusyRepeatRequest";
      }

      case Errc::kConditionsNotCorrect: {
        return "ConditionsNotCorrect";
      }

      case Errc::kRequestSequenceError: {
        return "RequestSequenceError";
      }

      case Errc::kNoResponseFromSubnetComponent: {
        return "NoResponseFromSubnetComponent";
      }

      case Errc::kFailurePreventsExecutionOfRequestedAction: {
        return "FailurePreventsExecutionOfRequestedAction";
      }

      case Errc::kRequestOutOfRange: {
        return "RequestOutOfRange";
      }

      case Errc::kSecurityAccessDenied: {
        return "SecurityAccessDenied";
      }

      case Errc::kInvalidKey: {
        return "InvalidKey";
      }

      case Errc::kExceedNumberOfAttempts: {
        return "ExceedNumberOfAttempts";
      }

      case Errc::kRequiredTimeDelayNotExpired: {
        return "RequiredTimeDelayNotExpired";
      }

      case Errc::kUploadDownloadNotAccepted: {
        return "UploadDownloadNotAccepted";
      }

      case Errc::kTransferDataSuspended: {
        return "TransferDataSuspended";
      }

      case Errc::kGeneralProgrammingFailure: {
        return "GeneralProgrammingFailure";
      }

      case Errc::kWrongBlockSequenceCounter: {
        return "WrongBlockSequenceCounter";
      }

      case Errc::kSubFunctionNotSupportedInActiveSession: {
        return "SubFunctionNotSupportedInActiveSession";
      }

      case Errc::kServiceNotSupportedInActiveSession: {
        return "ServiceNotSupportedInActiveSession";
      }

      case Errc::kRpmTooHigh: {
        return "RpmTooHigh";
      }

      case Errc::kRpmTooLow: {
        return "RpmTooLow";
      }

      case Errc::kEngineIsRunning: {
        return "EngineIsRunning";
      }

      case Errc::kEngineIsNotRunning: {
        return "EngineIsNotRunning";
      }

      case Errc::kEngineRunTimeTooLow: {
        return "EngineRunTimeTooLow";
      }

      case Errc::kTemperatureTooHigh: {
        return "TemperatureTooHigh";
      }

      case Errc::kTemperatureTooLow: {
        return "TemperatureTooLow";
      }

      case Errc::kVehicleSpeedTooHigh: {
        return "VehicleSpeedTooHigh";
      }

      case Errc::kVehicleSpeedTooLow: {
        return "VehicleSpeedTooLow";
      }

      case Errc::kThrottlePedalTooHigh: {
        return "ThrottlePedalTooHigh";
      }

      case Errc::kThrottlePedalTooLow: {
        return "kThrottlePedalTooLow";
      }

      case Errc::kTransmissionRangeNotInNeutral: {
        return "TransmissionRangeNotInNeutral";
      }

      case Errc::kTransmissionRangeNotInGear: {
        return "TransmissionRangeNotInGear";
      }

      case Errc::kBrakeSwitchNotClosed: {
        return "BrakeSwitchNotClosed";
      }

      case Errc::kShifterLeverNotInPark: {
        return "ShifterLeverNotInPark";
      }

      case Errc::kTorqueConverterClutchLocked: {
        return "TorqueConverterClutchLocked";
      }

      case Errc::kVoltageTooHigh: {
        return "VoltageTooHigh";
      }

      case Errc::kVoltageTooLow: {
        return "VoltageTooLow";
      }

      case Errc::kResourceTemporarilyNotAvailable: {
        return "ResourceTemporarilyNotAvailable";
      }

      case Errc::kNoProcessingNoResponse: {
        return "kNoProcessingNoResponse";
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
constexpr DiagUdsNrcErrorDomain g_diag_uds_nrc_error_domain;
} // namespace internal

inline constexpr const ara::core::ErrorDomain& GetDiagUdsNrcDomain() noexcept {
  return internal::g_diag_uds_nrc_error_domain;
}

inline constexpr ara::core::ErrorCode MakeErrorCode(DiagUdsNrcErrc code,
                                                    ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode(static_cast<ara::core::ErrorDomain::CodeType>(code),
                              GetDiagUdsNrcDomain(),
                              data);
}

} // namespace diag
} // namespace ara

#endif // TUSIMPLEAP_ARA_DIAG_DIAG_UDS_NRC_ERROR_DOMAIN_H_
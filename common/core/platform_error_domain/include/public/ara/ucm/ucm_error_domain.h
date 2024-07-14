#ifndef TUSIMPLEAP_ARA_UCM_UCM_ERROR_DOMAIN_H_
#define TUSIMPLEAP_ARA_UCM_UCM_ERROR_DOMAIN_H_

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"

namespace ara {
namespace ucm {

enum class UcmErrc : core::ErrorDomain::CodeType {
  kAuthenticationFailed = 8,
  kBlockInconsistent = 25,
  kBusyWithCampaign = 34,
  kCancelFailed = 16,
  kIncompatibleDelta = 29,
  kIncompatiblePackageVersion = 24,
  kIncorrectBlock = 2,
  kIncorrectBlockSize = 30,
  kIncorrectSize = 3,
  kInsufficientData = 6,
  kInsufficientMemory = 1,
  kInvalidChecksumDescription = 35,
  kInvalidPackageManifest = 13,
  kInvalidTransferId = 4,
  kMissingDependencies = 21,
  kNewCampaignDisabled = 31,
  kNotAbleToRevertPackages = 15,
  kNotAbleToRollback = 18,
  kOldVersion = 9,
  kOperationNotPermitted = 5,
  kPackageInconsistent = 7,
  kPreActivationFailed = 19,
  kProcessSwPackageCancelled = 22,
  kProcessedSoftwarePackageInconsistent = 23,
  kServiceBusy = 12,
  kSoftwareClusterMissing = 37,
  kTransferFailed = 38,
  kUnexpectedPackage = 32,
  kUpdateSessionRejected = 33,
  kVerificationFailed = 36,
  kMultiUpdateSessions = 0xFF01,
  kRevertToIdle = 0xFF02,
  kOpenFileFailed = 0xFF03,
  kNoSuchUpdateSession = 0xFF04,
  kPreActivationFailedBecauseFgNonOff = 0xFF05
};

class UcmException : public core::Exception {
 public:
  explicit UcmException(core::ErrorCode err) noexcept : Exception(err) {}
};

class UcmErrorDomain final : public core::ErrorDomain {
 public:
  using Errc = UcmErrc;
  using Exception = UcmException;

  constexpr static ErrorDomain::IdType kId = 0x8000000000000502;

  constexpr UcmErrorDomain() noexcept : core::ErrorDomain(kId) {}

  const char* Name() const noexcept final {
    return "Ucm";
  }

  const char* Message(ErrorDomain::CodeType errorCode) const noexcept final {
    switch (static_cast<Errc>(errorCode)) {
      case Errc::kAuthenticationFailed: {
        return "Package authentication failed";
      }
      case Errc::kBlockInconsistent: {
        return "Consistency check for transferred block failed";
      }
      case Errc::kBusyWithCampaign: {
        return "Campaign has already started";
      }
      case Errc::kCancelFailed: {
        return "Cancel failed";
      }
      case Errc::kIncompatibleDelta: {
        return "Delta package dependency check failed";
      }
      case Errc::kIncompatiblePackageVersion: {
        return "Version not compatible";
      }
      case Errc::kIncorrectBlock: {
        return "Incorrect block number";
      }
      case Errc::kIncorrectBlockSize: {
        return "Exceeds the provided block sized";
      }
      case Errc::kIncorrectSize: {
        return "Exceeds the provided size in TransferStart";
      }
      case Errc::kInsufficientData: {
        return " Total transferred data size does not match expected data size";
      }
      case Errc::kInsufficientMemory: {
        return "Insufficient memory to perform operation";
      }
      case Errc::kInvalidChecksumDescription: {
        return "AChecksum attribute not recognised";
      }
      case Errc::kInvalidPackageManifest: {
        return "Package manifest could not be read";
      }
      case Errc::kInvalidTransferId: {
        return "The Transfer ID is invalid";
      }
      case Errc::kMissingDependencies: {
        return "Activate cannot be performed because of missing dependencies";
      }
      case Errc::kNewCampaignDisabled: {
        return "New campaigns are disabled,";
      }
      case Errc::kNotAbleToRevertPackages: {
        return "RevertProcessedSwPackages failed";
      }
      case Errc::kNotAbleToRollback: {
        return "ARollback failed";
      }
      case Errc::kOldVersion: {
        return "Software Package version is too old";
      }
      case Errc::kOperationNotPermitted: {
        return "The operation is not supported";
      }
      case Errc::kPackageInconsistent: {
        return "Package integrity check failed";
      }
      case Errc::kPreActivationFailed: {
        return "Error during preActivation step";
      }
      case Errc::kProcessSwPackageCancelled: {
        return "ProcessSwPackage has been interrupted";
      }
      case Errc::kProcessedSoftwarePackageInconsistent: {
        return " Processed Software Package integrity check failed";
      }
      case Errc::kServiceBusy: {
        return "Another processing is already ongoing ";
      }
      case Errc::kSoftwareClusterMissing: {
        return "Software Cluster is not present in the Machine";
      }
      case Errc::kTransferFailed: {
        return "UCM cannot persist transferred block";
      }
      case Errc::kUnexpectedPackage: {
        return "Software Package name does not correspond to the "
               "RequestedPackage";
      }
      case Errc::kUpdateSessionRejected: {
        return "Start of an update session was rejected by SM";
      }
      case Errc::kVerificationFailed: {
        return "SM returned verification failure";
      }
      case Errc::kMultiUpdateSessions: {
        return "Only one update session can be started at the same time";
      }
      case Errc::kRevertToIdle: {
        return "Ucm has been reset to kIdle, a restart-update is needed";
      }
      case Errc::kOpenFileFailed: {
        return "Can not open the update file";
      }
      case Errc::kNoSuchUpdateSession: {
        return "Can not find the specific update session";
      }
      case Errc::kPreActivationFailedBecauseFgNonOff: {
        return "Error during preActivation step because the functional group is not in Off state";
      }
      default: {
        return "Unknown error";
      }
    }
  }

  void ThrowAsException(const core::ErrorCode& errorCode) const final {
    core::ThrowOrTerminate<Exception>(errorCode);
  }
};

namespace internal {

constexpr UcmErrorDomain g_UcmErrorDomain;

}

constexpr const core::ErrorDomain& GetUcmErrorDomain() noexcept {
  return internal::g_UcmErrorDomain;
}

inline constexpr core::ErrorCode MakeErrorCode(UcmErrc code, core::ErrorDomain::SupportDataType data) noexcept {
  return core::ErrorCode(static_cast<core::ErrorDomain::CodeType>(code), GetUcmErrorDomain(), data);
}

}  // namespace ucm
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_UCM_UCM_ERROR_DOMAIN_H_

#ifndef TUSIMPLEAP_ARA_EXEC_EXEC_ERROR_DOMAIN_H_
#define TUSIMPLEAP_ARA_EXEC_EXEC_ERROR_DOMAIN_H_

#include <cerrno>

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"

namespace ara {
namespace exec {

using ara::core::ErrorDomain;
using ara::core::Exception;
using ara::core::ErrorCode;
using ara::core::ThrowOrTerminate;

// {SWS_EM_02281}
// Defines an enumeration class for the Execution Management error codes.
enum class ExecErrc : ErrorDomain::CodeType {
  // Some unspecified error occurred
  kGeneralError = 1,
  // Invalid argument was passed
  kInvalidArguments = 2,
  // Communication error occurred
  kCommunicationError = 3,
  // Wrong meta model identifier passed to a function
  kMetaModelError = 4,
  // Transition to the requested Function Group state was cancelled by a newer
  // request
  kCancelled = 5,
  // Requested operation could not be performed
  kFailed = 6,
  // Unexpected Termination during transition in Process of previous Function
  // Group State happened
  kFailedUnexpectedTerminationOnExit = 7,
  // Unexpected Termination during transition in Process of target Function
  // Group State happened
  kFailedUnexpectedTerminationOnEnter = 8,
  // Transition invalid (e.g. report kRunning when already in Running Process
  // State)
  kInvalidTransition = 9,
  // Transition to the requested Function Group state failed because it is
  // already in requested state
  kAlreadyInState = 10,
  // Transition to the requested Function Group state failed because transition
  // to requested state is already in progress
  kInTransitionToSameState = 11,
  // DeterministicClient time stamp information is not available Values
  kNoTimeStamp = 12,
  // Deterministic activation cycle time exceeded
  kCycleOverrun = 13,

  // The following is a custom error code
  // Executable manifest configuration error, such as execution dependency errors, etc.
  kExecManifestConfigError = 0x10001,
  // Machine manifest configuration error
  kMachineManifestConfigError = 0x10002,
  // ApplicationExecutionMgr init failed
  kExecutionMgrInitFailed = 0x10003,
  // ApplicationExecutionMgr run failed
  kExecutionMgrRunFailed = 0x10004,
  // Child process does not exist
  kChildNotExist = 0x10005,
  // System software cluster manifest configuration error
  kSystemSwclManifestConfigError = 0x10006,
  // Software cluster manifest configuration error
  kSoftwareClusterConfigError = 0x10007,
  // "Process name not exist"
  kProcessNameNotExist = 0x10008,
  // "Function group is in Off state, operation cannot be performed"
  kFgInOffOrUndefinedState = 0x10009
};

// {SWS_EM_02282}
// Defines a class for exceptions to be thrown by the Execution Management
class ExecException final : public Exception {
public:
  // {SWS_EM_02283}
  // Args: errorCode: The error code.
  // Constructs a new ExecException object containing an error code.
  explicit ExecException(ErrorCode errorCode) noexcept :
    Exception(std::move(errorCode)) {
}

};

// {SWS_EM_02284}
// Defines a class representing the Execution Management error domain.
class ExecErrorDomain final : public ErrorDomain {
  constexpr static ErrorDomain::IdType kId = 0x8000000000000202;

public:
  // {SWS_EM_02286}
  // Constructs a new ExecErrorDomain object.
  constexpr ExecErrorDomain() noexcept : ErrorDomain(kId) {}
  // {SWS_EM_02287}
  // Returns: const char *: "Exec".
  // Returns a string constant associated with ExecErrorDomain.
  const char* Name() const noexcept final { return "Exec"; }
  // {SWS_EM_02288}
  // Args: errorCode: The error code number.
  // Return: const char *: The message associated with the error code.
  // Returns the message associated with errorCode.
  const char* Message(
      ErrorDomain::CodeType errorCode) const noexcept final {
    switch (static_cast<ExecErrc>(errorCode)) {
      case ExecErrc::kGeneralError: {
        return "Some unspecified error occurred";
        break;
      }
      case ExecErrc::kInvalidArguments: {
        return "Invalid argument was passed";
        break;
      }
      case ExecErrc::kCommunicationError: {
        return "Communication error occurred";
        break;
      }
      case ExecErrc::kMetaModelError: {
        return "Wrong meta model identifier passed to a function";
        break;
      }
      case ExecErrc::kCancelled: {
        return "Transition to the requested Function Group state was cancelled "
               "by a newer request";
        break;
      }
      case ExecErrc::kFailed: {
        return "Requested operation could not be performed";
        break;
      }
      case ExecErrc::kFailedUnexpectedTerminationOnExit: {
        return "Unexpected Termination during transition in Process of "
               "previous "
               "Function Group State happened";
        break;
      }
      case ExecErrc::kFailedUnexpectedTerminationOnEnter: {
        return "Unexpected Termination during transition in Process of target "
               "Function Group State happened";
        break;
      }
      case ExecErrc::kInvalidTransition: {
        return "Transition invalid";
        break;
      }
      case ExecErrc::kAlreadyInState: {
        return "Transition to the requested Function Group state failed "
               "because it is already in requested state";
        break;
      }
      case ExecErrc::kInTransitionToSameState: {
        return "Transition to the requested Function Group state failed "
               "because transition to requested state is already in progress";
        break;
      }
      case ExecErrc::kNoTimeStamp: {
        return "DeterministicClient time stamp information is not available "
               "Values";
        break;
      }
      case ExecErrc::kCycleOverrun: {
        return "Deterministic activation cycle time exceeded";
        break;
      }
      case ExecErrc::kExecManifestConfigError: {
        return "Executable manifest configuration error";
        break;
      }
      case ExecErrc::kMachineManifestConfigError: {
        return "Machine manifest configuration error";
        break;
      }
      case ExecErrc::kExecutionMgrInitFailed: {
        return "EM init failed";
        break;
      }
      case ExecErrc::kExecutionMgrRunFailed: {
        return "EM run failed";
        break;
      }
      case ExecErrc::kChildNotExist: {
        return "Child process does not exist";
        break;
      }
      case ExecErrc::kSystemSwclManifestConfigError: {
        return "System software cluster manifest configuration error";
        break;
      }
      case ExecErrc::kSoftwareClusterConfigError: {
        return "Software cluster manifest configuration error";
        break;
      }
      case ExecErrc::kProcessNameNotExist: {
        return "Process name not exist";
        break;
      }
      case ExecErrc::kFgInOffOrUndefinedState: {
        return "Function group is in Off / UndefinedFunctionGroupState, operation cannot be performed";
        break;
      }
      default: {
        return "unknown future error";
        break;
      }
    }
  }

  // {SWS_EM_02289}
  // Args: errorCode: The error to throw.
  // Creates a new instance of ExecException from errorCode and throws it as a
  // C++ exception.
  void ThrowAsException(const ErrorCode& errorCode) const
      noexcept(false) final {
    ThrowOrTerminate<Exception>(errorCode);
  }
};

namespace internal {
constexpr ExecErrorDomain g_exec_error_domain;
}

// {SWS_EM_02290}
// Return: const ErrorDomain &: Return a reference to the global
// ExecErrorDomain object. Returns a reference to the global ExecErrorDomain
// object.
constexpr const ErrorDomain& GetExecErrorDomain() noexcept {
  return internal::g_exec_error_domain;
}
// {SWS_EM_02291}
// Args: code: Error code number.
//       data: Vendor defined data associated with the error.
// Creates an instance of ErrorCode.
constexpr ErrorCode MakeErrorCode(ExecErrc code, ErrorDomain::SupportDataType data) noexcept {
  return ErrorCode(static_cast<ErrorDomain::CodeType>(code), GetExecErrorDomain(), data);
}
  
}  // namespace exec
}  // namespace ara

#endif // TUSIMPLEAP_ARA_EXEC_EXEC_ERROR_DOMAIN_H_
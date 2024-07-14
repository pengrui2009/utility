/*
 * @Author: sl.wang shuailin.wang@tusimple.ai
 * @Date: 2022-06-08 12:21:30
 * @LastEditors: sl.wang shuailin.wang@tusimple.ai
 * @LastEditTime: 2022-06-29 10:40:58
 * @FilePath: /com/include/ara/com/internal/comapi/com_error_domian.h
 * @Description:
 *
 * Copyright (c) 2022 by Tusen, All Rights Reserved.
 */

/**
 * \brief The ComErrc enumeration defines the error codes for the ComErrorDomain
 *
 */


#ifndef TUSIMPLEAP_ARA_COM_COM_ERROR_DOMIAN_H_
#define TUSIMPLEAP_ARA_COM_COM_ERROR_DOMIAN_H_

#include <cerrno>

#include <ara/core/error_code.h>
#include <ara/core/error_domain.h>
#include <ara/core/exception.h>

// using ara::core::ErrorDomain;
// using ara::core::Exception;
// using ara::core::ErrorCode;
// using ara::core::ThrowOrTerminate;


namespace ara {
namespace com {

/**
    SWS_CM_10432
*/    
enum class ComErrc : ara::core::ErrorDomain::CodeType {
    kServiceNotAvailable = 1,    ///< Service is not available.
    kMaxSamplesExceeded = 2,     ///< Application holds more SamplePtrs than commited in Subscribe().
    kNetworkBindingFailure = 3,  ///< Local failure has been detected by the network binding.
    kGrantEnforcementError = 4,  ///< Request was refused by Grant enforcement laye
    kPeerIsUnreachable = 5,      ///< TLS handshake fail
    kFieldValueIsNotValid= 6,    ///< Field Value is not valid,.
    kSetHandlerNotSet= 7,        ///< SetHandler has not been registered.
    kUnsetFailure= 8,            ///< Failure has been detected by unset operation.
    kSampleAllocationFailure= 9, ///< Not Sufficient memory resources can be allocated.
    kIllegalUseOfAllocate= 10,   ///< The allocation was illegally done via custom allocator (i.e., not via shared memory allocation). 
    kServiceNotOffered= 11,      ///< Service not offered.
    kCommunicationLinkError= 12, ///< Communication link is broken.
    kCommunicationStackError= 14, ///< Communication Stack Error, e.g. network stack,
                                  /// network binding, or communication framework reports an error
    kInstanceIDCouldNotBeResolved= 15, ///< ResolveInstanceIDs() failed to resolve InstanceID from InstanceSpecifier
    kMaxSampleCountNotRealizable= 16, ///< Provided maxSampleCount not realizable
    kWrongMethodCallProcessingMode= 17, ///< Wrong processing mode passed to constructor method call.
    kErroneousFileHandle= 18,           ///< The FileHandle returned from FindServce is corrupt/service not available.
    kCouldNotExecute= 19,               ///< Command could not be executed in provided,Execution Context
    kInvalidInstanceIdentifierString= 20, ///< Given InstanceIdentifier string is corrupted or non-compliant
};


class ComException : public ara::core::Exception
{
public:
    explicit ComException(ara::core::ErrorCode errorCode) noexcept
        : ara::core::Exception(std::move(errorCode))
    { }
};

class ComErrorDomain final : public ara::core::ErrorDomain
{
    /**
    SWS_CM_11329
    */
    constexpr static ara::core::ErrorDomain::IdType kId = 0x8000000000001267;

    public:
        using Errc = ComErrc;
        using Exception = ComException;

         /// @brief Default constructor
         // [SWS_CORE_00241]
         // [SWS_CORE_00012]
         constexpr ComErrorDomain() noexcept
            : ara::core::ErrorDomain(kId)
        { }

        /// @brief Return the "shortname" of this error domain.
        char const* Name() const noexcept override{ return "Com";}

        char const* Message(ara::core::ErrorDomain::CodeType errorCode) const noexcept override
        {
            Errc const code = static_cast<Errc>(errorCode);
            switch (code) {
            case Errc::kServiceNotAvailable:
                return "Service is not available.";
            case Errc::kMaxSamplesExceeded:
                return "Application holds more SamplePtrs than commited in Subscribe().";
            case Errc::kNetworkBindingFailure:
                return "Local failure has been detected by the network binding.";
            case Errc::kGrantEnforcementError:
                return "Request was refused by Grant enforcement layer.";
            case Errc::kPeerIsUnreachable:
                return "TLS handshake fail.";
            case Errc::kFieldValueIsNotValid:
                return "Field Value is not valid";
            case Errc::kSetHandlerNotSet:
                return "SetHandler has not been registered";
            case Errc::kUnsetFailure:
                return "Failure has been detected by unset operation";
            case Errc::kSampleAllocationFailure:
                return "Not Sufficient memory resources can be allocated";
            case Errc::kIllegalUseOfAllocate:
                return "The allocation was illegally done via custom allocator";
            case Errc::kServiceNotOffered:
                return "Service not offered.";
            case Errc::kCommunicationLinkError:
                return "Communication link is broken.";
            case Errc::kCommunicationStackError:
                return "Communication Stack Error.";
            case Errc::kInstanceIDCouldNotBeResolved:
                return "ResolveInstanceIDs() failed to resolve InstanceID from InstanceSpecifier.";
            case Errc::kMaxSampleCountNotRealizable:
                return "Provided maxSampleCount not realizable.";
            case Errc::kWrongMethodCallProcessingMode:
                return "Wrong processing mode passed to constructor method call.";
            case Errc::kErroneousFileHandle:
                return "The FileHandle returned from FindServce is corrupt/service not available.";
            case Errc::kCouldNotExecute:
                return "Command could not be executed in provided Execution Context.";
            case Errc::kInvalidInstanceIdentifierString:
                return "Given InstanceIdentifier string is corrupted or non-compliant.";
            default:
                return "Unknown error code";
            }

        }

        // {SWS_CM_11333}
        // Args: errorCode: The error to throw.
        // Creates a new instance of ComException from errorCode and throws it as a
        // C++ exception.
        void ThrowAsException(const ara::core::ErrorCode& errorCode) const noexcept(false) override {
            ara::core::ThrowOrTerminate<Exception>(errorCode);
        }

};

namespace internal
{
    constexpr ComErrorDomain g_ComErrorDomain;
}


// {SWS_CM_11334}
// Return: const ErrorDomain &: Return a reference to the global
// ExecErrorDomain object. Returns a reference to the global ComErrorDomain
// object.
inline constexpr ara::core::ErrorDomain const& GetComErrorDomain() noexcept{
    return internal::g_ComErrorDomain;
}

// {SWS_CM_11335}
// Args: code: Error code number.
//       data: Vendor defined data associated with the error.
// Creates an instance of ErrorCode.
inline constexpr ara::core::ErrorCode MakeErrorCode(ComErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept{
    return ara::core::ErrorCode(static_cast<ara::core::ErrorDomain::CodeType>(code), GetComErrorDomain(), data);
}


}  // namespace com
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_COM_COM_ERROR_DOMIAN_H_



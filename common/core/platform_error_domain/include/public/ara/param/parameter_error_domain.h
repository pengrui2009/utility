/*
 * @Description: 
 * @Author: an.huang@tusen.ai
 * @Date: 2023-08-04 06:12:17
 * @LastEditors: an.huang
 * @LastEditTime: 2023-08-11 02:39:53
 * @FilePath: /adas-monorepo/modules/adaptive_autosar/ara-api/parameter_service/include/ara/parameter/common/parameter_error_domain.h
 * @version: 
 *  
 * Copyright (c) 2023 by Tusen, All Rights Reserved.
 */
#ifndef ARA_PARAMETER_COMMON_PARAMETER_ERROR_DOMAIN_H_
#define ARA_PARAMETER_COMMON_PARAMETER_ERROR_DOMAIN_H_

#include <ara/core/error_code.h>
#include <ara/core/error_domain.h>
#include <ara/core/exception.h>

namespace ara{
namespace parameter{

enum class ParamErrc : ara::core::ErrorDomain::CodeType {
    // parameter service error
    kKeyNotFound = 1,
    kCommunicationError = 2,
    kModifyReadOnlyDataError = 3,
    kTimeOutError = 4,
    kParameterServiceNotAvailable = 5,

    // parse key error
    kInvalidKey = 6,

    // save and load data error
    kFileOpenFailed = 7,
    kUnexpectedSaveError = 8,
    kUnexpectedLoadError = 9,

    // register handler error
    kRegisterGetHandlerError = 10,
    kRegisterSetHandlerError = 11,
    kRegisterUpdateHandlerError= 12,

    // parameter error
    kValueTypeError = 13,
    kInvalidParameter = 14,
    kBadKeyMapError = 15
};

class ParamException : public ara::core::Exception{
    public:
    explicit ParamException(ara::core::ErrorCode errorCode) noexcept
        : ara::core::Exception(std::move(errorCode)){}
};

class ParamErrorDomain final : public ara::core::ErrorDomain{
    constexpr static ara::core::ErrorDomain::IdType kId = 0x8000000000011111;

    public:
    constexpr ParamErrorDomain() noexcept
        : ara::core::ErrorDomain(kId){}
    
    char const* Name() const noexcept override { return "Param"; }

    char const* Message(ara::core::ErrorDomain::CodeType errorCode) const noexcept override{
        switch(static_cast<ParamErrc>(errorCode)){
            case ParamErrc::kKeyNotFound:
                return "Key not found.";
            case ParamErrc::kCommunicationError:
                return "Communication with ara_extend_param failed.";
            case ParamErrc::kTimeOutError:
                return "Parameter service time out.";
            case ParamErrc::kInvalidKey:
                return "Invalid key.";
            case ParamErrc::kFileOpenFailed:
                return "Database open failed.";
            case ParamErrc::kUnexpectedLoadError:
                return "Unexpected load error occurred.";
            case ParamErrc::kUnexpectedSaveError:
                return "Unexpected save error occurred.";
            case ParamErrc::kRegisterGetHandlerError:
                return "Register Get() callback failed.";
            case ParamErrc::kRegisterSetHandlerError:
                return "Register Set() callback failed.";
            case ParamErrc::kRegisterUpdateHandlerError:
                return "Register Update() callback failed.";
            case ParamErrc::kValueTypeError:
                return "Parameter value type error.";
            case ParamErrc::kInvalidParameter:
                return "Invalid parameter detected.";
            case ParamErrc::kBadKeyMapError:
                return "Stored keys are incompatible.";
            case ParamErrc::kModifyReadOnlyDataError:
                return "Modify readonly data is prohibited.";
            case ParamErrc::kParameterServiceNotAvailable:
                return "Parameter service is not available.";
            default:
                return "Unknown error code.";
        }
    }

    void ThrowAsException(const ara::core::ErrorCode& errorCode) const noexcept(false) override{
        ara::core::ThrowOrTerminate<ParamException>(errorCode);
    }
};

namespace{
    constexpr ParamErrorDomain t_ParamErrorDomain;
}

inline constexpr ara::core::ErrorDomain const& GetParamErrorDomain() noexcept{
    return t_ParamErrorDomain;
}

inline constexpr ara::core::ErrorCode MakeErrorCode(ParamErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept{
    return ara::core::ErrorCode(static_cast<ara::core::ErrorDomain::CodeType>(code), GetParamErrorDomain(), data);
}

}
}

#endif
#ifndef TUSIMPLEAP_ARA_DIAG_UDS_TRANSPORT_DOIP_DOIP_ERROR_DOMAIN_H_
#define TUSIMPLEAP_ARA_DIAG_UDS_TRANSPORT_DOIP_DOIP_ERROR_DOMAIN_H_

#include <ara/core/error_code.h>
#include <ara/core/error_domain.h>
#include <ara/core/exception.h>

namespace ara {
namespace diag {
namespace uds_transport {
namespace doip {

enum class DoipHeaderErrc : ara::core::ErrorDomain::CodeType {
  kIncorrectPatternFormat = 0x00,
  kUnknownPayloadType = 0x01,
  kMessageTooLarge = 0x02,
  kOutOfMemory = 0x03,
  kInvalidPayloadLength = 0x04
};

class DoipHeaderException : public ara::core::Exception {
  explicit DoipHeaderException(ara::core::ErrorCode err) noexcept : ara::core::Exception(std::move(err)) {}
};

class DoipHeaderErrorDomain final : public ara::core::ErrorDomain {
 public:
  using Errc = ara::diag::uds_transport::doip::DoipHeaderErrc;
  using Exception = ara::diag::uds_transport::doip::DoipHeaderException;

  constexpr static ErrorDomain::IdType kId = 0x8000000000000501;
  constexpr DoipHeaderErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}

  const char* Name() const noexcept override { return "DoipHeader"; }

  const char* Message(ara::core::ErrorDomain::CodeType code) const noexcept override {
    switch (static_cast<Errc>(code)) {
      case Errc::kIncorrectPatternFormat: {
        return "IncorrectPatternFormat";
      }

      case Errc::kUnknownPayloadType: {
        return "UnknownPayloadType";
      }

      case Errc::kMessageTooLarge: {
        return "MessageTooLarge";
      }

      case Errc::kOutOfMemory: {
        return "OutOfMemory";
      }

      case Errc::kInvalidPayloadLength: {
        return "InvalidPayloadLength";
      }

      default: {
        return "UnknownError";
      }
    }
  }

  void ThrowAsException(const ara::core::ErrorCode& code) const noexcept(false) override {
    ara::core::ThrowOrTerminate<ara::core::Exception>(code);
  }
};

namespace internal {

constexpr DoipHeaderErrorDomain g_doip_header_error_domain;

}  // namespace internal

inline constexpr const ara::core::ErrorDomain& GetDoipHeaderDomain() noexcept {
  return internal::g_doip_header_error_domain;
}

inline constexpr ara::core::ErrorCode MakeErrorCode(
    DoipHeaderErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode(static_cast<ara::core::ErrorDomain::CodeType>(code), GetDoipHeaderDomain(), data);
}

}  // namespace doip
}  // namespace uds_transport
}  // namespace diag
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_DIAG_UDS_TRANSPORT_DOIP_DOIP_ERROR_DOMAIN_H_
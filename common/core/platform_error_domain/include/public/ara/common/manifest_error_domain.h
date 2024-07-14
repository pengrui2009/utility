#ifndef TUSIMPLEAP_ARA_CORE_CORE_ERROR_DOMAIN_H_
#define TUSIMPLEAP_ARA_CORE_CORE_ERROR_DOMAIN_H_

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"

namespace ara {
namespace common {

enum class ManifestErrc : core::ErrorDomain::CodeType {
  kMachineManifestError = 1,
  kDiagnosticManifestError,
};

class ManifestException final : public core::Exception {
 public:
  explicit ManifestException(core::ErrorCode err) noexcept : Exception(err) {}
};

class ManifestErrorDomain final : public core::ErrorDomain {
  constexpr static core::ErrorDomain::IdType kId = 0x8000000000010000;

 public:
  using Errc = ManifestErrc;
  using Exception = ManifestException;

  constexpr ManifestErrorDomain() noexcept : ErrorDomain(kId) {}

  const char* Name() const noexcept final {
    return "Manifest";
  }

  const char* Message(core::ErrorDomain::CodeType errorCode) const noexcept final {
    switch (static_cast<Errc>(errorCode)) {
      case Errc::kMachineManifestError: {
        return "Machine manifest configuration error";
        break;
      }
      case Errc::kDiagnosticManifestError: {
        return "Diagnostic manifest configuration error";
        break;
      }
      default: {
        return "manifest unknown error";
        break;
      }
    }
  }

  void ThrowAsException(const core::ErrorCode& errorCode) const final {
    core::ThrowOrTerminate<Exception>(errorCode);
  }
};

namespace internal {

constexpr ManifestErrorDomain g_ManifestErrorDomain;

}

constexpr const core::ErrorDomain& GetManifestErrorDomain() noexcept {
  return internal::g_ManifestErrorDomain;
}

inline constexpr core::ErrorCode MakeErrorCode(ManifestErrc code, core::ErrorDomain::SupportDataType data) noexcept {
  return core::ErrorCode(static_cast<core::ErrorDomain::CodeType>(code), GetManifestErrorDomain(), data);
}

}  // namespace common
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_CORE_ERROR_DOMAIN_H_
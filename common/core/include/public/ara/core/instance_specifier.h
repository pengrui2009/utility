#ifndef ARA_CORE_INSTANCE_SPECIFIER_H
#define ARA_CORE_INSTANCE_SPECIFIER_H

#include <algorithm>
#include <regex>

#include "ara/core/core_error_domain.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/string_view.h"

namespace ara {
namespace core {

class InstanceSpecifier final {
public:
  static Result<InstanceSpecifier> Create(StringView meta_model_identifier) {
    Result<void> ret = Validate(meta_model_identifier);
    if (!ret) {
      return Result<InstanceSpecifier>::FromError(ret.Error());
    }

    return InstanceSpecifier(meta_model_identifier, true);
  }

  explicit InstanceSpecifier(StringView meta_model_identifier) :
      InstanceSpecifier(Create(meta_model_identifier).ValueOrThrow()) {}

  InstanceSpecifier(const InstanceSpecifier& other) = default;

  InstanceSpecifier(InstanceSpecifier&& other) noexcept = default;

  InstanceSpecifier& operator=(const InstanceSpecifier& other) = default;

  InstanceSpecifier& operator=(InstanceSpecifier&& other) = default;

  ~InstanceSpecifier() = default;

  StringView ToString() const noexcept { return static_cast<StringView>(meta_model_identifier_); }

  bool operator==(const InstanceSpecifier& other) const noexcept {
    return meta_model_identifier_ == other.meta_model_identifier_;
  }

  bool operator==(StringView other) const noexcept {
    return meta_model_identifier_ == other.data();
  }

  bool operator!=(const InstanceSpecifier& other) const noexcept {
    return meta_model_identifier_ != other.meta_model_identifier_;
  }

  bool operator!=(StringView other) const noexcept {
    return meta_model_identifier_ != other.data();
  }

  bool operator<(const InstanceSpecifier& other) const noexcept {
    return meta_model_identifier_ < other.meta_model_identifier_;
  }

private:
  InstanceSpecifier(StringView meta_model_identifier, bool /*tag*/) :
      meta_model_identifier_(meta_model_identifier) {}

  static Result<void> Validate(const StringView meta_model_identifier) noexcept {
    size_t ite = meta_model_identifier.find_first_of('/');
    const StringView path_component = meta_model_identifier.substr(0, ite);
    if (path_component.empty()) {
      return Result<void>(CoreErrc::invalid_meta_model_path);
    }
    if (!IsValidPart(path_component)) {
      return Result<void>(CoreErrc::invalid_meta_model_shortname);
    }
    if (ite != std::string::npos) {
      return Validate(meta_model_identifier.substr(ite + 1));
    }
    return Result<void>();
  }

  static bool IsValidPart(const StringView part) {
    return std::regex_match(part.begin(), part.end(), std::regex("^[A-Za-z]\\w*$"));
  }

private:
  ara::core::String meta_model_identifier_;
};

inline bool operator==(StringView lhs, const InstanceSpecifier& rhs) noexcept {
  return rhs.ToString() == lhs.data();
}

inline bool operator!=(StringView lhs, const InstanceSpecifier& rhs) noexcept {
  return rhs.ToString() != lhs.data();
}

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_INSTANCE_SPECIFIER_H

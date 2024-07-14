

/**
 * @file
 * @brief Interface to class ara::core::String
 */

#ifndef TUSIMPLEAP_ARA_CORE_STRING_H_
#define TUSIMPLEAP_ARA_CORE_STRING_H_

#include <functional>
#include <string>
#include <type_traits>

#include "ara/core/string_view.h"

namespace ara {
namespace core {

template <typename Allocator = std::allocator<char>>
class BasicString final
    : public std::basic_string<char, std::char_traits<char>, Allocator> {
public:
  // Default constructors are not inherited by a using declaration, and since
  // we are also adding other constructors, no default one is being generated
  // by the compiler. Thus, we must define this explicitly.
  BasicString() noexcept(noexcept(Allocator())) : Base(Allocator()) {}

  ~BasicString() = default;

  using Base = std::basic_string<char, std::char_traits<char>, Allocator>;

  // Inherit most constructors of std::string
  // (except for copy/move and default ctor, see C++11, 12.9.3)
  using Base::Base;
  using Base::operator=;
  using Base::operator+=;
  using Base::append;
  using Base::assign;
  using Base::compare;
  using Base::find;
  using Base::find_first_not_of;
  using Base::find_first_of;
  using Base::find_last_not_of;
  using Base::find_last_of;
  using Base::insert;
  using Base::replace;
  using Base::rfind;
  using Base::substr;
  // Single-argument copy/move constructors are not inherited by a using
  // declaration, but we want them for interoperability with std::string, so
  // that we can support assignment from std::string.

  BasicString(const Base& s) : Base(s) {}

  BasicString(Base&& s) noexcept : Base(std::move(s)) {}

  // Standard copy and move constructors.

  BasicString(const BasicString& s) = default;

  BasicString(BasicString&& s) noexcept : Base(std::move(s)) {}

  // Inherit assignment operators from std::string. As this subclass does not
  // add any additional data member, there is no danger of slicing.

  // Standard copy and move assignment operators.

  BasicString& operator=(const BasicString& other) {
    Base::assign(other);
    return *this;
  }

  BasicString& operator=(BasicString&& other) & noexcept {
    Base::assign(std::move(other));
    return *this;
  }

  //
  // Interoperability with StringView (taken from C++17)
  //

  using size_type = typename Base::size_type;
  using const_iterator = typename Base::const_iterator;
  using Base::npos;

  /// @uptrace{SWS_CORE_03301}
  explicit operator StringView() const noexcept {
    return StringView(Base::data(), Base::size());
  }

  /// @uptrace{SWS_CORE_03302}
  explicit BasicString(StringView sv) : Base(sv.data(), sv.size()) {}

  /// @uptrace{SWS_CORE_03303}
  template <
      typename T,
      typename = typename std::enable_if<
          std::is_convertible<const T&, StringView>::value &&
          !std::is_convertible<const T&, const char*>::value>::type>
  BasicString(
      const T& t,
      size_type pos,
      size_type n,
      const Allocator& a = Allocator()) :
      BasicString(StringView(t).substr(pos, n), a) {}

  /// @uptrace{SWS_CORE_03304}
  BasicString& operator=(StringView sv) {
    return assign(sv);
  }

  /// @uptrace{SWS_CORE_03307}
  BasicString& operator+=(StringView sv) & { return this->append(sv); }

  /// @uptrace{SWS_CORE_03308}
  BasicString& append(StringView sv) {
    Base::append(sv.data(), sv.size());
    return *this;
  }

  /// @uptrace{SWS_CORE_03309}
  template <
      typename T,
      typename = typename std::enable_if<
          std::is_convertible<const T&, StringView>::value &&
          !std::is_convertible<const T&, const char*>::value>::type>
  BasicString& append(const T& t, size_type pos, size_type n = npos) {
    // TODO: out_of_range if pos > sv.size()
    const StringView sv = t;
    const size_type rlen = std::min(n, size_type(sv.size() - pos));
    Base::append(sv.data() + pos, rlen);
    return *this;
  }

  /// @uptrace{SWS_CORE_03305}
  BasicString& assign(const StringView& sv) {
    Base::assign(sv.data(), sv.size());
    return *this;
  }

  /// @uptrace{SWS_CORE_03306}
  template <
      typename T,
      typename = typename std::enable_if<
          std::is_convertible<const T&, StringView>::value &&
          !std::is_convertible<const T&, const char*>::value>::type>
  BasicString& assign(
      const T& t, size_type pos, size_type n = npos) & noexcept {
    // TODO: out_of_range if pos > sv.size()
    const StringView sv = t;
    const size_type rlen = std::min(n, size_type(Base::size() - pos));
    Base::assign(sv.data() + pos, rlen);
    return *this;
  }

  /// @uptrace{SWS_CORE_03310}
  BasicString& insert(size_type pos, StringView sv) {
    Base::insert(pos, sv.data(), sv.size());
    return *this;
  }

  /// @uptrace{SWS_CORE_03311}
  template <
      typename T,
      typename = typename std::enable_if<
          std::is_convertible<const T&, StringView>::value &&
          !std::is_convertible<const T&, const char*>::value>::type>
  BasicString& insert(
      size_type pos1, const T& t, size_type pos2, size_type n = npos) {
    // TODO: out_of_range if pos1 > size() or pos2 > sv.size()
    const StringView sv = t;
    const size_type rlen = std::min(n, size_type(Base::size() - pos2));
    Base::insert(pos1, sv.data() + pos2, rlen);
    return *this;
  }

  /// @uptrace{SWS_CORE_03312}
  BasicString& replace(size_type pos1, size_type n1, StringView sv) {
    Base::replace(pos1, n1, sv.data(), sv.size());
    return *this;
  }

  /// @uptrace{SWS_CORE_03313}
  template <
      typename T,
      typename = typename std::enable_if<
          std::is_convertible<const T&, StringView>::value &&
          !std::is_convertible<const T&, const char*>::value>::type>
  BasicString& replace(
      size_type pos1,
      size_type n1,
      const T& t,
      size_type pos2,
      size_type n2 = npos) {
    // TODO: out_of_range if pos1 > size() or pos2 > sv.size()
    const StringView sv = t;
    const size_type rlen = std::min(n2, size_type(sv.size() - pos2));
    Base::replace(pos1, n1, sv.data() + pos2, rlen);
    return *this;
  }

  /// @uptrace{SWS_CORE_03314}
  BasicString& replace(const_iterator i1, const_iterator i2, StringView sv) {
    return this->replace(i1 - Base::begin(), i2 - i1, sv);
  }

  /// @uptrace{SWS_CORE_03315}
  size_type find(StringView sv, size_type pos = 0ULL) const noexcept {
    return Base::find(sv.data(), pos, sv.size());
  }

  /// @uptrace{SWS_CORE_03316}
  size_type rfind(StringView sv, size_type pos = npos) const noexcept {
    return Base::rfind(sv.data(), pos, sv.size());
  }

  /// @uptrace{SWS_CORE_03317}
  size_type find_first_of(StringView sv, size_type pos = 0ULL) const noexcept {
    return Base::find_first_of(sv.data(), pos, sv.size());
  }

  /// @uptrace{SWS_CORE_03318}
  size_type find_last_of(StringView sv, size_type pos = npos) const noexcept {
    return Base::find_last_of(sv.data(), pos, sv.size());
  }

  /// @uptrace{SWS_CORE_03319}
  size_type find_first_not_of(
      StringView sv, size_type pos = 0ULL) const noexcept {
    return Base::find_first_not_of(sv.data(), pos, sv.size());
  }

  /// @uptrace{SWS_CORE_03320}
  size_type find_last_not_of(
      StringView sv, size_type pos = npos) const noexcept {
    return Base::find_last_not_of(sv.data(), pos, sv.size());
  }

  /// @uptrace{SWS_CORE_03321}
  int compare(StringView sv) const noexcept {
    const size_type rlen = std::min(Base::size(), sv.size());
    const int ret =
        std::char_traits<char>::compare(Base::data(), sv.data(), rlen);
    if (ret == 0) {
      return (Base::size() < sv.size()) ? -1 : (Base::size() > sv.size());
    }
    return ret;
  }

  /// @uptrace{SWS_CORE_03322}
  int compare(size_type pos1, size_type n1, StringView sv) const {
    return static_cast<BasicString>(Base::substr(pos1, n1)).compare(sv);
  }

  /// @uptrace{SWS_CORE_03323}
  template <
      typename T,
      typename = typename std::enable_if<
          std::is_convertible<const T&, StringView>::value &&
          !std::is_convertible<const T&, const char*>::value>::type>
  int compare(
      size_type pos1,
      size_type n1,
      const T& t,
      size_type pos2,
      size_type n2 = npos) const {
    const StringView sv = t;
    return static_cast<BasicString>(Base::substr(pos1, n1))
        .compare(sv.substr(pos2, n2));
  }
};

/**
 * @brief Representation of a contiguous sequence of narrow characters
 *
 * @uptrace{SWS_CORE_03001}
 */
using String = BasicString<>;

// Transitional compatibility name; should remove this before R18-10.
/// @brief Add overload of std::swap for String.
///
/// We actually don't need this overload at all, because our implementation
/// inherits from std::string and thus can simply use the overload for that.
/// However, we need this symbol in order to provide uptracing.
///
/// @param lhs  the first argument of the swap invocation
/// @param rhs  the second argument of the swap invocation
///
/// @uptrace{SWS_CORE_03296}
template <typename Allocator>
void swap(BasicString<Allocator>& lhs, BasicString<Allocator>& rhs) {
  lhs.swap(rhs);
}

}  // namespace core
}  // namespace ara

namespace std {

/// @brief Specialization of std::hash for ara::core::String
template <>
class hash<ara::core::String> final {
public:
  using result_type = std::size_t;

  /// @brief Function call operator to retrieve the hash value of
  /// ara::core::String.
  ///
  /// By providing this, ara::core::String can be used in std::unordered_map
  /// and std::unordered_set.
  ///
  /// @param k  the ara::core::String to be hashed.
  /// @returns the hash value
  result_type operator()(const ara::core::String& s) const noexcept {
    using std::hash;
    result_type ret = 0ULL;
    for (result_type i = 0ULL; i < s.length(); ++i) {
      ret += hash<char>()(s[i]);
    }
    return ret;
  }
};

}  // namespace std

#endif  // TUSIMPLEAP_ARA_CORE_STRING_H_

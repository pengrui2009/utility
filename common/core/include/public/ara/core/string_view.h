

#ifndef TUSIMPLEAP_ARA_CORE_STRING_VIEW_H_
#define TUSIMPLEAP_ARA_CORE_STRING_VIEW_H_

#include <cstddef>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace ara {
namespace core {
namespace internal {

namespace traits {
constexpr std::size_t length(const char* start) {
  const char* end = start;
  for (; *end != '\0'; ++end) {}
  return end - start;
}
}  // namespace traits

template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_string_view final {
public:
  using traits_type = Traits;
  using value_type = CharT;
  using pointer = value_type*;
  using reference = value_type&;
  using const_pointer = const value_type*;
  using const_reference = const value_type&;
  using const_iterator = const value_type*;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using size_type = std::size_t;
  constexpr static size_type npos = SIZE_MAX;

public:
  // 24.4.2.1, construction and assignment
  constexpr basic_string_view() noexcept : basic_string_view(nullptr, 0) {}

  constexpr basic_string_view(const_pointer p, size_type count) : mPtr(p), mSize(count) {}

  constexpr basic_string_view(const_pointer p) : mPtr(p), mSize(traits::length(p)) {}

  constexpr basic_string_view(const basic_string_view& other) noexcept = default;

  // Not "constexpr" because that would make it also "const" on C++11 compilers.
  basic_string_view& operator=(const basic_string_view& other) & noexcept = default;

  ~basic_string_view() = default;

  // 24.4.2.2, iterator support
  constexpr const_iterator begin() const noexcept { return mPtr; }
  constexpr const_iterator cbegin() const noexcept { return mPtr; }
  constexpr const_iterator end() const noexcept { return mPtr + mSize; }
  constexpr const_iterator cend() const noexcept { return mPtr + mSize; }
  constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
  constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
  constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

  // 24.4.2.3, capacity
  constexpr size_type size() const noexcept { return mSize; }
  constexpr size_type length() const noexcept { return mSize; }
  constexpr size_type max_size() const noexcept {
    // TODO: not for wchar!
    return std::numeric_limits<size_type>::max();
  }
  constexpr bool empty() const noexcept { return mSize == 0; }

  // 24.4.2.4, element access
  constexpr const_reference operator[](size_type pos) const { return mPtr[pos]; }

  constexpr const_reference at(size_type pos) const {
    if (pos >= mSize) { throw std::out_of_range("out of range"); }
    return mPtr[pos];
  }
  constexpr const_reference front() const { return mPtr[0]; }
  constexpr const_reference back() const { return mPtr[mSize - 1]; }
  constexpr const_pointer data() const noexcept { return mPtr; }

  // 24.4.2.5, modifiers
  constexpr void remove_prefix(size_type n) {
    mPtr += n;
    mSize -= n;
  }
  constexpr void remove_suffix(size_type n) { mSize -= n; }
  constexpr void swap(basic_string_view& other) noexcept {
    std::swap(mPtr, other.mPtr);
    std::swap(mSize, other.mSize);
  }

  // 24.4.2.6, operations
  size_type copy(pointer dest, size_type count, size_type pos = 0ULL) const {
    if (pos > mSize) { throw std::out_of_range("out of range"); }

    const size_t rcount = std::min(count, mSize - pos);
    std::copy(mPtr + pos, mPtr + pos + rcount, dest);
    return rcount;
  }
  constexpr basic_string_view substr(size_type pos = 0ULL, size_type count = npos) const {
    if (pos > mSize) { throw std::out_of_range("out of range"); }

    const size_t rcount = std::min(count, mSize - pos);
    return basic_string_view(mPtr + pos, rcount);
  }
  constexpr int compare(basic_string_view v) const noexcept {
    int ret = traits_type::compare(mPtr, v.mPtr, std::min(mSize, v.mSize));
    if (ret == 0) { return (mSize < v.mSize) ? -1 : (mSize > v.mSize); }
    return ret;
  }
  constexpr int compare(size_type pos1, size_type count1, basic_string_view v) const {
    return substr(pos1, count1).compare(v);
  }
  constexpr int compare(size_type pos1, size_type count1, basic_string_view v, size_type pos2, size_type count2) const {
    return substr(pos1, count1).compare(v.substr(pos2, count2));
  }
  constexpr int compare(const_pointer s) const { return compare(basic_string_view(s)); }
  constexpr int compare(size_type pos1, size_type count1, const_pointer s) const {
    return substr(pos1, count1).compare(basic_string_view(s));
  }
  constexpr int compare(size_type pos1, size_type count1, const_pointer s, size_type count2) const {
    return substr(pos1, count1).compare(basic_string_view(s, count2));
  }

  size_type find(basic_string_view v, size_type pos = 0ULL) const noexcept { return find(v.data(), pos, v.size()); }
  size_type find(value_type ch, size_type pos = 0ULL) const noexcept {
    return std::basic_string<value_type, traits_type>(mPtr, mSize).find(ch, pos);
  }
  size_type find(const_pointer s, size_type pos, size_type count) const {
    return std::basic_string<value_type, traits_type>(mPtr, mSize).find(s, pos, count);
  }
  size_type find(const_pointer s, size_type pos = 0ULL) const { return find(s, pos, traits::length(s)); }

  size_type rfind(basic_string_view v, size_type pos = npos) const noexcept { return rfind(v.data(), pos, v.size()); }
  size_type rfind(value_type c, size_type pos = npos) const noexcept {
    return std::basic_string<value_type, traits_type>(mPtr, mSize).rfind(c, pos);
  }
  size_type rfind(const_pointer s, size_type pos, size_type count) const {
    return std::basic_string<value_type, traits_type>(mPtr, mSize).rfind(s, pos, count);
  }
  size_type rfind(const_pointer s, size_type pos = npos) const { return rfind(s, pos, traits::length(s)); }

  size_type find_first_of(basic_string_view v, size_type pos = 0ULL) const noexcept {
    return find_first_of(v.data(), pos, v.size());
  }
  size_type find_first_of(value_type c, size_type pos = 0ULL) const noexcept {
    return std::basic_string<value_type, traits_type>(mPtr, mSize).find_first_of(c, pos);
  }
  size_type find_first_of(const_pointer s, size_type pos, size_type count) const {
    return std::basic_string<value_type, traits_type>(mPtr, mSize).find_first_of(s, pos, count);
  }
  size_type find_first_of(const_pointer s, size_type pos = 0ULL) const {
    return find_first_of(s, pos, traits::length(s));
  }

  size_type find_last_of(basic_string_view v, size_type pos = npos) const noexcept {
    return find_last_of(v.data(), pos, v.size());
  }
  size_type find_last_of(value_type c, size_type pos = npos) const noexcept {
    return std::basic_string<value_type, traits_type>(mPtr, mSize).find_last_of(c, pos);
  }
  size_type find_last_of(const_pointer s, size_type pos, size_type count) const {
    return std::basic_string<value_type, traits_type>(mPtr, mSize).find_last_of(s, pos, count);
  }
  size_type find_last_of(const_pointer s, size_type pos = npos) const {
    return find_last_of(s, pos, traits::length(s));
  }

  size_type find_first_not_of(basic_string_view v, size_type pos = 0ULL) const noexcept {
    return find_first_not_of(v.data(), pos, v.size());
  }
  size_type find_first_not_of(value_type c, size_type pos = 0ULL) const noexcept {
    return std::basic_string<value_type, traits_type>(mPtr, mSize).find_first_not_of(c, pos);
  }
  size_type find_first_not_of(const_pointer s, size_type pos, size_type count) const {
    return std::basic_string<value_type, traits_type>(mPtr, mSize).find_first_not_of(s, pos, count);
  }
  size_type find_first_not_of(const_pointer s, size_type pos = 0ULL) const {
    return find_first_not_of(s, pos, traits::length(s));
  }

  size_type find_last_not_of(basic_string_view v, size_type pos = npos) const noexcept {
    return find_last_not_of(v.data(), pos, v.size());
  }
  size_type find_last_not_of(value_type c, size_type pos = npos) const noexcept {
    return std::basic_string<value_type, traits_type>(mPtr, mSize).find_last_not_of(c, pos);
  }
  size_type find_last_not_of(const_pointer s, size_type pos, size_type count) const {
    return std::basic_string<value_type, traits_type>(mPtr, mSize).find_last_not_of(s, pos, count);
  }
  size_type find_last_not_of(const_pointer s, size_type pos = npos) const {
    return find_last_not_of(s, pos, traits::length(s));
  }

private:
  const_pointer mPtr;
  size_type mSize;
};

// 24.4.4, inserters and extractors
template <typename value_type, typename traits_type>
inline std::basic_ostream<value_type, traits_type>& operator<<(std::basic_ostream<value_type, traits_type>& os,
                                                               const basic_string_view<value_type, traits_type>& v) {
  os << std::basic_string<value_type, traits_type>(v.data(), v.size());
  return os;
}

// 24.4.3, non-member comparison functions,
//         incl. additional overloads

// Helper type for forcing template argument deduction to proceed based on
// only one of the arguments to a comparison operator. See n3766 for details.
template <typename T>
using Identity = typename std::decay<T>::type;

template <typename value_type, typename traits_type>
constexpr bool operator==(basic_string_view<value_type, traits_type> lhs,
                          basic_string_view<value_type, traits_type> rhs) noexcept {
  return lhs.compare(rhs) == 0;
}
template <typename value_type, typename traits_type>
constexpr bool operator==(basic_string_view<value_type, traits_type> lhs,
                          Identity<basic_string_view<value_type, traits_type>> rhs) noexcept {
  return lhs.compare(rhs) == 0;
}
template <typename value_type, typename traits_type>
constexpr bool operator==(Identity<basic_string_view<value_type, traits_type>> lhs,
                          basic_string_view<value_type, traits_type> rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

template <typename value_type, typename traits_type>
constexpr bool operator!=(basic_string_view<value_type, traits_type> lhs,
                          basic_string_view<value_type, traits_type> rhs) noexcept {
  return lhs.compare(rhs) != 0;
}
template <typename value_type, typename traits_type>
constexpr bool operator!=(basic_string_view<value_type, traits_type> lhs,
                          Identity<basic_string_view<value_type, traits_type>> rhs) noexcept {
  return lhs.compare(rhs) != 0;
}
template <typename value_type, typename traits_type>
constexpr bool operator!=(Identity<basic_string_view<value_type, traits_type>> lhs,
                          basic_string_view<value_type, traits_type> rhs) noexcept {
  return lhs.compare(rhs) != 0;
}

template <typename value_type, typename traits_type>
constexpr bool operator<(basic_string_view<value_type, traits_type> lhs,
                         basic_string_view<value_type, traits_type> rhs) noexcept {
  return lhs.compare(rhs) < 0;
}
template <typename value_type, typename traits_type>
constexpr bool operator<(basic_string_view<value_type, traits_type> lhs,
                         Identity<basic_string_view<value_type, traits_type>> rhs) noexcept {
  return lhs.compare(rhs) < 0;
}
template <typename value_type, typename traits_type>
constexpr bool operator<(Identity<basic_string_view<value_type, traits_type>> lhs,
                         basic_string_view<value_type, traits_type> rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

template <typename value_type, typename traits_type>
constexpr bool operator>(basic_string_view<value_type, traits_type> lhs,
                         basic_string_view<value_type, traits_type> rhs) noexcept {
  return lhs.compare(rhs) > 0;
}
template <typename value_type, typename traits_type>
constexpr bool operator>(basic_string_view<value_type, traits_type> lhs,
                         Identity<basic_string_view<value_type, traits_type>> rhs) noexcept {
  return lhs.compare(rhs) > 0;
}
template <typename value_type, typename traits_type>
constexpr bool operator>(Identity<basic_string_view<value_type, traits_type>> lhs,
                         basic_string_view<value_type, traits_type> rhs) noexcept {
  return lhs.compare(rhs) > 0;
}

template <typename value_type, typename traits_type>
constexpr bool operator<=(basic_string_view<value_type, traits_type> lhs,
                          basic_string_view<value_type, traits_type> rhs) noexcept {
  return lhs.compare(rhs) <= 0;
}
template <typename value_type, typename traits_type>
constexpr bool operator<=(basic_string_view<value_type, traits_type> lhs,
                          Identity<basic_string_view<value_type, traits_type>> rhs) noexcept {
  return lhs.compare(rhs) <= 0;
}
template <typename value_type, typename traits_type>
constexpr bool operator<=(Identity<basic_string_view<value_type, traits_type>> lhs,
                          basic_string_view<value_type, traits_type> rhs) noexcept {
  return lhs.compare(rhs) <= 0;
}

template <typename value_type, typename traits_type>
constexpr bool operator>=(basic_string_view<value_type, traits_type> lhs,
                          basic_string_view<value_type, traits_type> rhs) noexcept {
  return lhs.compare(rhs) >= 0;
}
template <typename value_type, typename traits_type>
constexpr bool operator>=(basic_string_view<value_type, traits_type> lhs,
                          Identity<basic_string_view<value_type, traits_type>> rhs) noexcept {
  return lhs.compare(rhs) >= 0;
}
template <typename value_type, typename traits_type>
constexpr bool operator>=(Identity<basic_string_view<value_type, traits_type>> lhs,
                          basic_string_view<value_type, traits_type> rhs) noexcept {
  return lhs.compare(rhs) >= 0;
}

}  // namespace internal

/// @brief A read-only view over a contiguous sequence of characters.
///
/// @uptrace{SWS_CORE_02001}
using StringView = internal::basic_string_view<char>;

}  // namespace core
}  // namespace ara

namespace std {

namespace literals {
constexpr ara::core::StringView operator"" _sv(const char* str, std::size_t len) noexcept {
  return ara::core::StringView(str, len);
}
}  // namespace literals

/// @brief Specialization of std::hash for ara::core::String
template <>
class hash<ara::core::StringView> final {
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
  result_type operator()(const ara::core::StringView& s) const noexcept {
    using std::hash;
    result_type ret = 0ULL;
    for (result_type i = 0ULL; i < s.length(); ++i) { ret += hash<char>()(s[i]); }
    return ret;
  }
};

}  // namespace std
#endif  // TUSIMPLEAP_ARA_CORE_STRING_VIEW_H_

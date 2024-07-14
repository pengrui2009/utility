

/// @file
/// @brief Interface to class ara::core::array

#ifndef TUSIMPLEAP_ARA_CORE_ARRAY_H_
#define TUSIMPLEAP_ARA_CORE_ARRAY_H_

#include <array>
#include <type_traits>
#include <limits>

namespace ara {
namespace core {

/// @brief A sequence container that encapsulates fixed sized arrays
///
/// @tparam T  the type of contained values
/// @tparam N  the number of elements in this Array
///
/// @uptrace{SWS_CORE_01201}
template <typename T, std::size_t N>
using Array = std::array<T, N>;

/// @brief Add overload of swap for ara::core::Array
///
/// We actually don't need this overload at all, because our implementation is
/// just a type alias and thus can simply use the overload for the std:: type.
/// However, we need this symbol in order to provide uptracing.
///
/// @tparam T  the type of values in the Arrays
/// @tparam N  the size of the Arrays
/// @param lhs  the first argument of the swap invocation
/// @param rhs  the second argument of the swap invocation
///

// @uptrace{SWS_CORE_01296}
template <typename T,
          std::size_t N,
          typename = typename std::enable_if<!std::is_same<Array<T, N>, std::array<T, N>>::value>::type>
void swap(Array<T, N> &lhs, Array<T, N> &rhs) {
  lhs.swap(rhs);
}

// @uptrace{SWS_CORE_01290}
template <typename T,
          std::size_t N,
          typename = typename std::enable_if<!std::is_same<Array<T, N>, std::array<T, N>>::value>::type>
bool operator==(const Array<T, N> &lhs, const Array<T, N> &rhs) {
  return std::operator==(lhs, rhs);
}

// @uptrace{SWS_CORE_01291}
template <typename T,
          std::size_t N,
          typename = typename std::enable_if<!std::is_same<Array<T, N>, std::array<T, N>>::value>::type>
bool operator!=(const Array<T, N> &lhs, const Array<T, N> &rhs) {
  return std::operator!=(lhs, rhs);
}

// @uptrace{SWS_CORE_01292}
template <typename T,
          std::size_t N,
          typename = typename std::enable_if<!std::is_same<Array<T, N>, std::array<T, N>>::value>::type>
bool operator<(const Array<T, N> &lhs, const Array<T, N> &rhs) {
  return std::operator<(lhs, rhs);
}

// @uptrace{SWS_CORE_01293}
template <typename T,
          std::size_t N,
          typename = typename std::enable_if<!std::is_same<Array<T, N>, std::array<T, N>>::value>::type>
bool operator>(const Array<T, N> &lhs, const Array<T, N> &rhs) {
  return std::operator>(lhs, rhs);
}

// @uptrace{SWS_CORE_01294}
template <typename T,
          std::size_t N,
          typename = typename std::enable_if<!std::is_same<Array<T, N>, std::array<T, N>>::value>::type>
bool operator<=(const Array<T, N> &lhs, const Array<T, N> &rhs) {
  return std::operator<=(lhs, rhs);
}

// @uptrace{SWS_CORE_01295}
template <typename T,
          std::size_t N,
          typename = typename std::enable_if<!std::is_same<Array<T, N>, std::array<T, N>>::value>::type>
bool operator>=(const Array<T, N> &lhs, const Array<T, N> &rhs) {
  return std::operator>=(lhs, rhs);
}

}  // namespace core
}  // namespace ara

namespace std {

// @uptrace{SWS_CORE_01282}
template <std::size_t I,
          typename T,
          std::size_t N,
          typename = typename std::enable_if<!std::is_same<ara::core::Array<T, N>, std::array<T, N>>::value>::type>
constexpr T &get(ara::core::Array<T, N> &a) noexcept {
  static_assert(I < N, "array index is within bounds");
  return a[I];
}

// @uptrace{SWS_CORE_01283}
template <std::size_t I,
          typename T,
          std::size_t N,
          typename = typename std::enable_if<!std::is_same<ara::core::Array<T, N>, std::array<T, N>>::value>::type>
constexpr T &&get(ara::core::Array<T, N> &&a) noexcept {
  static_assert(I < N, "array index is within bounds");
  return a[I];
}

// @uptrace{SWS_CORE_01284}
template <std::size_t I,
          typename T,
          std::size_t N,
          typename = typename std::enable_if<!std::is_same<ara::core::Array<T, N>, std::array<T, N>>::value>::type>
constexpr T const &get(const ara::core::Array<T, N> &a) noexcept {
  static_assert(I < N, "array index is within bounds");
  return a[I];
}

constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();

}  // namespace std

#endif  // TUSIMPLEAP_ARA_CORE_ARRAY_H_

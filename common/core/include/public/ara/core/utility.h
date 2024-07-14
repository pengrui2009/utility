

#ifndef TUSIMPLEAP_ARA_CORE_UTILITY_H_
#define TUSIMPLEAP_ARA_CORE_UTILITY_H_

#include <cstddef>
#include <initializer_list>
#include <iterator>

namespace ara {
namespace core {

using Byte = char;

// In-place construction
class in_place_t final {
public:
  explicit in_place_t() = default;
};
constexpr in_place_t in_place {};

template <typename T>
class in_place_type_t final {
public:
  explicit in_place_type_t() = default;
};
template <typename T>
constexpr in_place_type_t<T> in_place_type;

template <std::size_t I>
class in_place_index_t final {
public:
  explicit in_place_index_t() = default;
};
template <std::size_t I>
constexpr in_place_index_t<I> in_place_index {};

/// @brief Return a pointer to the block of memory that contains the elements of a container.
/// @tparam Container  a type with a data() method
/// @param c  an instance of Container
/// @returns a pointer to the first element of the container
///
/// @uptrace{SWS_CORE_04110}
template <typename Container>
constexpr auto data(Container& c) -> decltype(c.data()) {
  return c.data();
}

/// @brief Return a const_pointer to the block of memory that contains the elements of a container.
/// @tparam Container  a type with a data() method
/// @param c  an instance of Container
/// @returns a pointer to the first element of the container
///
/// @uptrace{SWS_CORE_04111}
template <typename Container>
constexpr auto data(const Container& c) -> decltype(c.data()) {
  return c.data();
}

/// @brief Return a pointer to the block of memory that contains the elements of a raw array.
/// @tparam T  the type of array elements
/// @tparam N  the number of elements in the array
/// @param array  reference to a raw array
/// @returns a pointer to the first element of the array
///
/// @uptrace{SWS_CORE_04112}
template <typename T, std::size_t N>
constexpr T* data(T (&array)[N]) noexcept {
  return array;
}

/// @brief Return a pointer to the block of memory that contains the elements of a std::initializer_list.
/// @tparam E  the type of elements in the std::initializer_list
/// @param il  the std::initializer_list
/// @returns a pointer to the first element of the std::initializer_list
///
/// @uptrace{SWS_CORE_04113}
template <typename E>
constexpr const E* data(std::initializer_list<E> il) noexcept {
  return il.begin();
}

/// @brief Return the size of a container.
/// @tparam Container  a type with a data() method
/// @param c  an instance of Container
/// @returns the size of the container
///
/// @uptrace{SWS_CORE_04120}
template <typename Container>
constexpr auto size(const Container& c) -> decltype(c.size()) {
  return c.size();
}

/// @brief Return the size of a raw array.
/// @tparam T  the type of array elements
/// @tparam N  the number of elements in the array
/// @param array  reference to a raw array
/// @returns the size of the array, i.e. N
///
/// @uptrace{SWS_CORE_04121}
template <typename T, std::size_t N>
constexpr std::size_t size(const T (&array)[N]) noexcept {
  return N;
}

template <typename Container>
constexpr auto empty(const Container& c) -> decltype(c.empty()) {
  return c.empty();
}

template <typename T, std::size_t N>
constexpr bool empty(const T (&array)[N]) noexcept {
  return N == 0;
}

template <typename E>
constexpr bool empty(std::initializer_list<E> il) noexcept {
  return il.begin() == il.end();
}

}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_UTILITY_H_

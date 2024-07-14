

/**
 * @file
 * @brief Interface to class ara::core::Vector
 */

#ifndef TUSIMPLEAP_ARA_CORE_VECTOR_H_
#define TUSIMPLEAP_ARA_CORE_VECTOR_H_

#include <memory>
#include <type_traits>
#include <vector>

namespace ara {
namespace core {

/**
 * @brief A sequence container that encapsulates dynamically sized arrays
 *
 * @tparam T  the type of contained values
 * @tparam Allocator  the type of Allocator to use for this container
 *
 * @uptrace{SWS_CORE_01301}
 */
template <typename T, typename Allocator = std::allocator<T>>
using Vector = std::vector<T, Allocator>;

/**
 * @brief Global operator== for Vector instances
 *
 * @uptrace{SWS_CORE_01390}
 */
// template <typename T, typename Allocator>
template <
    typename T,
    typename Allocator,
    typename = typename std::enable_if<!std::is_same<Vector<T, Allocator>, std::vector<T, Allocator>>::value>::type>
inline bool operator==(const Vector<T, Allocator>& lhs, const Vector<T, Allocator>& rhs) {
  return std::operator==(lhs, rhs);
}

/**
 * @brief Global operator!= for Vector instances
 *
 * @uptrace{SWS_CORE_01391}
 */
// template <typename T, typename Allocator>
template <
    typename T,
    typename Allocator,
    typename = typename std::enable_if<!std::is_same<Vector<T, Allocator>, std::vector<T, Allocator>>::value>::type>
inline bool operator!=(const Vector<T, Allocator>& lhs, const Vector<T, Allocator>& rhs) {
  return std::operator!=(lhs, rhs);
}

/**
 * @brief Global operator< for Vector instances
 *
 * @uptrace{SWS_CORE_01392}
 */
// template <typename T, typename Allocator>
template <
    typename T,
    typename Allocator,
    typename = typename std::enable_if<!std::is_same<Vector<T, Allocator>, std::vector<T, Allocator>>::value>::type>
inline bool operator<(const Vector<T, Allocator>& lhs, const Vector<T, Allocator>& rhs) {
  return std::operator<(lhs, rhs);
}

/**
 * @brief Global operator<= for Vector instances
 *
 * @uptrace{SWS_CORE_01393}
 */
// template <typename T, typename Allocator>
template <
    typename T,
    typename Allocator,
    typename = typename std::enable_if<!std::is_same<Vector<T, Allocator>, std::vector<T, Allocator>>::value>::type>
inline bool operator<=(const Vector<T, Allocator>& lhs, const Vector<T, Allocator>& rhs) {
  return std::operator<=(lhs, rhs);
}

/**
 * @brief Global operator> for Vector instances
 *
 * @uptrace{SWS_CORE_01394}
 */
// template <typename T, typename Allocator>
template <
    typename T,
    typename Allocator,
    typename = typename std::enable_if<!std::is_same<Vector<T, Allocator>, std::vector<T, Allocator>>::value>::type>
bool operator>(const Vector<T, Allocator>& lhs, const Vector<T, Allocator>& rhs) {
  return std::operator>(lhs, rhs);
}

/**
 * @brief Global operator>= for Vector instances
 *
 * @uptrace{SWS_CORE_01395}
 */
// template <typename T, typename Allocator>
template <
    typename T,
    typename Allocator,
    typename = typename std::enable_if<!std::is_same<Vector<T, Allocator>, std::vector<T, Allocator>>::value>::type>
bool operator>=(const Vector<T, Allocator>& lhs, const Vector<T, Allocator>& rhs) {
  return std::operator>=(lhs, rhs);
}

/**
 * @brief Add overload of swap for ara::core::Vector
 *
 * We actually don't need this overload at all, because our implementation is
 * just a type alias and thus can simply use the overload for the std:: type.
 * However, we need this symbol in order to provide uptracing.
 *
 * @tparam T  the type of values in the Vectors
 * @tparam Allocator  the Allocator used by the Vectors
 * @param lhs  the first argument of the swap invocation
 * @param rhs  the second argument of the swap invocation
 *
 * @uptrace{SWS_CORE_01396}
 */
// template <typename T, typename Allocator>
template <
    typename T,
    typename Allocator,
    typename = typename std::enable_if<!std::is_same<Vector<T, Allocator>, std::vector<T, Allocator>>::value>::type>
void swap(Vector<T, Allocator>& lhs, Vector<T, Allocator>& rhs) {
  lhs.swap(rhs);
}

}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_VECTOR_H_



/**
 * @file
 * @brief Interface to class ara::core::map
 */

#ifndef TUSIMPLEAP_ARA_CORE_MAP_H_
#define TUSIMPLEAP_ARA_CORE_MAP_H_

#include <map>
#include <memory>
#include <type_traits>

namespace ara {
namespace core {

/**
 * @brief A container that contains key-value pairs with unique keys
 *
 * @tparam Key  the type of keys in this Map
 * @tparam T  the type of values in this Map
 * @tparam C  the type of comparison Callable
 * @tparam Allocator  the type of Allocator to use for this container
 *
 * @uptrace{SWS_CORE_01400}
 */
template <typename Key,
          typename T,
          typename C = std::less<Key>,
          typename Allocator = std::allocator<std::pair<const Key, T>>>
using Map = std::map<Key, T, C, Allocator>;

/// @brief Add overload of std::swap for Map.
///
/// We actually don't need this overload at all, because our implementation is
/// just a type alias and thus can simply use the overload for the std:: type.
/// However, we need this symbol in order to provide uptracing.
///
/// @tparam Key  the type of keys in the Maps
/// @tparam T  the type of values in the Maps
/// @tparam C  the type of comparison Callables in the Maps
/// @tparam Allocator  the type of Allocators in the Maps
/// @param lhs  the first argument of the swap invocation
/// @param rhs  the second argument of the swap invocation
///
/// @uptrace{SWS_CORE_01496}
template <typename Key,
          typename T,
          typename C,
          typename Allocator,
          typename = typename std::enable_if<
              !std::is_same<Map<Key, T, C, Allocator>, std::map<Key, T, C, Allocator>>::value>::type>
void swap(Map<Key, T, C, Allocator>& lhs, Map<Key, T, C, Allocator>& rhs) {
  lhs.swap(rhs);
}

}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_MAP_H_

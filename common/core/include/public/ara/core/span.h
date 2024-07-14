

#ifndef TUSIMPLEAP_ARA_CORE_SPAN_H_
#define TUSIMPLEAP_ARA_CORE_SPAN_H_

#include "ara/core/array.h"
#include "ara/core/utility.h"

#include <cassert>
#include <cstddef>
#include <type_traits>

namespace ara {
namespace core {

/// @uptrace{SWS_CORE_01901}
constexpr std::ptrdiff_t dynamic_extent = -1;

template <typename T, std::ptrdiff_t Extent>
class Span;

namespace internal {

template <typename T>
constexpr bool is_ara_core_span_checker = false;
template <typename T, std::ptrdiff_t Extent>
constexpr bool is_ara_core_span_checker<core::Span<T, Extent>> = true;
template <typename T>
constexpr bool is_ara_core_span = is_ara_core_span_checker<typename std::remove_cv<T>::type>;

template <typename T>
constexpr bool is_ara_core_array_checker = false;
template <typename T, std::size_t N>
constexpr bool is_ara_core_array_checker<core::Array<T, N>> = true;
template <typename T>
constexpr bool is_ara_core_array = is_ara_core_array_checker<typename std::remove_cv<T>::type>;

template <typename, typename = size_t>
constexpr bool is_complete = false;
template <typename T>
constexpr bool is_complete<T, decltype(sizeof(T))> = true;

}  // namespace internal

/// @brief A view over a contiguous sequence of objects
///
/// @uptrace{SWS_CORE_01900}
template <typename T, std::ptrdiff_t Extent = dynamic_extent>
class Span final {
  static_assert(Extent == dynamic_extent || Extent >= 0, "invalid extent for a Span");
  static_assert(std::is_object<T>::value, "Span cannot contain reference or void types");
  static_assert(!std::is_abstract<T>::value, "Span cannot contain abstract types");
  static_assert(internal::is_complete<T>, "Span cannot contain incomplete types");

  template <typename U>
  using type_of_data = decltype(ara::core::data(std::declval<U>()));

  template <typename U>
  using type_of_size = decltype(ara::core::size(std::declval<U>()));

public:
  /// @uptrace{SWS_CORE_01911}
  using element_type = T;
  /// @uptrace{SWS_CORE_01912}
  using value_type = typename std::remove_cv<element_type>::type;
  /// @uptrace{SWS_CORE_01913}
  using size_type = std::ptrdiff_t;
  /// @uptrace{SWS_CORE_01914}
  using difference_type = std::ptrdiff_t;
  /// @uptrace{SWS_CORE_01915}
  using pointer = element_type*;
  using const_pointer = const element_type*;
  /// @uptrace{SWS_CORE_01916}
  using reference = element_type&;
  using const_reference = const element_type&;
  /// @uptrace{SWS_CORE_01917}
  using iterator = element_type*;
  /// @uptrace{SWS_CORE_01918}
  using const_iterator = const element_type*;
  /// @uptrace{SWS_CORE_01919}
  using reverse_iterator = std::reverse_iterator<iterator>;
  /// @uptrace{SWS_CORE_01920}
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  /// @uptrace{SWS_CORE_01931}
  static constexpr size_type extent = Extent;

  // 21.7.3.2, constructors, copy, and assignment

  /// @uptrace{SWS_CORE_01941}
  template <typename U = void, typename = typename std::enable_if<Extent == dynamic_extent || Extent == 0, U>::type>
  constexpr Span() noexcept : mData(nullptr), mExtent(0) {}

  /// @uptrace{SWS_CORE_01942}
  constexpr Span(pointer ptr, size_type count) : mData(ptr), mExtent(count) {
    if (Extent != dynamic_extent && count != Extent) { assert(!"inconsistent extent"); }
  }

  /// @uptrace{SWS_CORE_01943}
  constexpr Span(pointer firstElem, pointer lastElem) : Span(firstElem, lastElem - firstElem) {
    auto tmp = std::distance(firstElem, lastElem);
    assert(tmp >= 0);
  }

  /// @uptrace{SWS_CORE_01944}
  template <std::size_t N,
            typename = typename std::enable_if<
                (Extent == dynamic_extent || static_cast<std::size_t>(Extent) == N) &&
                std::is_convertible<typename std::remove_pointer<type_of_data<element_type (&)[N]>>::type (*)[],
                                    element_type (*)[]>::value>::type>
  constexpr Span(element_type (&arr)[N]) noexcept : Span(std::addressof(arr[0]), N) {}

  /// @uptrace{SWS_CORE_01953}
  template <std::size_t N,
            typename = typename std::enable_if<
                (Extent == dynamic_extent || static_cast<std::size_t>(Extent) == N) &&
                std::is_convertible<typename std::remove_pointer<
                                        decltype(std::declval<std::array<value_type, N>>().data())>::type (*)[],
                                    element_type (*)[]>::value>::type>
  constexpr Span(std::array<value_type, N>& arr) noexcept : Span(arr.data(), N) {}

  /// @uptrace{SWS_CORE_01954}
  template <std::size_t N,
            typename = typename std::enable_if<
                (Extent == dynamic_extent || static_cast<std::size_t>(Extent) == N) &&
                std::is_convertible<typename std::remove_pointer<
                                        decltype(std::declval<const std::array<value_type, N>>().data())>::type (*)[],
                                    element_type (*)[]>::value>::type>
  constexpr Span(const std::array<value_type, N>& arr) noexcept : Span(arr.data(), N) {}
  ///// @uptrace{SWS_CORE_01945}
  // template <
  //    std::size_t N,
  //    typename = typename std::enable_if<
  //        (Extent == dynamic_extent || static_cast<std::size_t>(Extent) == N) &&
  //        std::is_convertible<
  //            typename std::remove_pointer<decltype(std::declval<Array<value_type, N>>().data())>::type (*)[],
  //            element_type (*)[]>::value && !std::is_same<ara::core::Array<value_type, N>, std::array<value_type,
  //            N>>::value>::type>
  // constexpr Span(Array<value_type, N>& arr) noexcept : Span(arr.data(), N) {
  //}

  ///// @uptrace{SWS_CORE_01946}
  // template <
  //    std::size_t N,
  //    typename = typename std::enable_if<
  //        (Extent == dynamic_extent || static_cast<std::size_t>(Extent) == N) &&
  //        std::is_convertible<
  //            typename std::remove_pointer<decltype(std::declval<const Array<value_type, N>>().data())>::type (*)[],
  //            element_type (*)[]>::value && !std::is_same<ara::core::Array<value_type, N>, std::array<value_type,
  //            N>>::value>::type>
  // constexpr Span(const Array<value_type, N>& arr) noexcept : Span(arr.data(), N) {
  //}

  /// @uptrace{SWS_CORE_01947}
  template <typename Container,
            typename = typename std::enable_if<!internal::is_ara_core_span<Container> &&
                                               !internal::is_ara_core_array<Container> &&
                                               !std::is_array<Container>::value &&
                                               std::is_pointer<Container>::value
                                               >::type>
  constexpr Span(Container& cont) : Span(core::data(cont), static_cast<size_type>(core::size(cont))) {
    auto tmp = std::distance(core::data(cont), core::data(cont) + core::size(cont));
    assert(tmp >= 0);
  }

  /// @uptrace{SWS_CORE_01948}
  template <typename Container,
            typename = typename std::enable_if<!internal::is_ara_core_span<Container> &&
                                               !internal::is_ara_core_array<Container> &&
                                               !std::is_array<Container>::value &&
                                               std::is_pointer<Container>::value
                                               >::type>
  constexpr Span(const Container& cont) : Span(core::data(cont), static_cast<size_type>(core::size(cont))) {
    auto tmp = std::distance(core::data(cont), core::data(cont) + core::size(cont));
    assert(tmp >= 0);
  }

  /// @uptrace{SWS_CORE_01949}
  constexpr Span(const Span& other) noexcept = default;

  /// @brief Converting constructor
  ///
  /// This ctor allows assignment of a cv-Span<> from a normal Span<>,
  /// and also of a dynamic_extent-Span<> from a static extent-one.
  ///
  /// @uptrace{SWS_CORE_01950}
  template <typename U,
            std::ptrdiff_t N,
            typename = typename std::enable_if<(Extent == dynamic_extent || Extent == N) &&
                                               std::is_convertible<U (*)[], element_type (*)[]>::value>::type*>
  constexpr Span(const Span<U, N>& s) noexcept : Span(s.data(), s.size()) {}

  /// @uptrace{SWS_CORE_01951}
  ~Span() noexcept = default;

  // Not "constexpr" because that would make it also "const" on C++11 compilers.
  /// @uptrace{SWS_CORE_01952}
  Span& operator=(const Span& other) & noexcept = default;

  // 21.7.3.3, subviews

  /// @uptrace{SWS_CORE_01961}
  template <std::ptrdiff_t Count>
  constexpr Span<element_type, Count> first() const {
    static_assert(0 <= Count, "subview size cannot be negative");
    assert(Count <= mExtent);

    return {mData, Count};
  }

  /// @uptrace{SWS_CORE_01962}
  constexpr Span<element_type, dynamic_extent> first(size_type count) const {
    assert(0 <= count && count <= mExtent);

    return {mData, count};
  }

  /// @uptrace{SWS_CORE_01963}
  template <std::ptrdiff_t Count>
  constexpr Span<element_type, Count> last() const {
    static_assert(0 <= Count, "subview size cannot be negative");
    assert(Count <= mExtent);

    return {mData + (size() - Count), Count};
  }

  /// @uptrace{SWS_CORE_01964}
  constexpr Span<element_type, dynamic_extent> last(size_type count) const {
    assert(0 <= count && count <= mExtent);

    return {mData + (size() - count), count};
  }

  // TODO: need proper C++11 compatible return type
  /// @uptrace{SWS_CORE_01965}
  template <std::ptrdiff_t Offset, std::ptrdiff_t Count = dynamic_extent>
  constexpr auto subspan() const /*-> Span<element_type, XXX> */
  {
    assert(0 <= Offset && Offset <= mExtent);
    assert(Count == dynamic_extent || (Count >= 0 && Offset + Count <= mExtent));

    constexpr size_type E =
        (Count != dynamic_extent) ? Count : (Extent != dynamic_extent ? Extent - Offset : dynamic_extent);

    return Span<element_type, E>(mData + Offset, Count != dynamic_extent ? Count : size() - Offset);
  }

  /// @uptrace{SWS_CORE_01966}
  constexpr Span<element_type, dynamic_extent> subspan(size_type offset, size_type count = dynamic_extent) const {
    assert(0 <= offset && offset <= mExtent);
    assert(count == dynamic_extent || (count >= 0 && offset + count <= mExtent));

    return {mData + offset, count == dynamic_extent ? size() - offset : count};
  }

  // 21.7.3.4, observers

  /// @uptrace{SWS_CORE_01967}
  constexpr size_type size() const noexcept { return mExtent; }

  /// @uptrace{SWS_CORE_01968}
  constexpr size_type size_bytes() const noexcept { return mExtent * sizeof(T); }

  /// @uptrace{SWS_CORE_01969}
  constexpr bool empty() const noexcept { return mExtent == 0; }

  // 21.7.3.5, element access

  /// @uptrace{SWS_CORE_01970}
  constexpr reference operator[](size_type idx) const { return mData[idx]; }
  reference operator[](size_type idx) { return mData[idx]; }

  /// @uptrace{SWS_CORE_01971}
  constexpr pointer data() const noexcept { return mData; }

  /// @uptrace{SWS_CORE_01959}
  constexpr reference front() const { return mData[0]; }

  /// @uptrace{SWS_CORE_01960}
  constexpr reference back() const { return mData[mExtent]; }

  // 21.7.3.6, iterator support

  /// @uptrace{SWS_CORE_01972}
  constexpr iterator begin() const noexcept { return &mData[0]; }

  /// @uptrace{SWS_CORE_01973}
  constexpr iterator end() const noexcept { return &mData[mExtent]; }

  /// @uptrace{SWS_CORE_01974}
  constexpr const_iterator cbegin() const noexcept { return &mData[0]; }

  /// @uptrace{SWS_CORE_01975}
  constexpr const_iterator cend() const noexcept { return &mData[mExtent]; }

  /// @uptrace{SWS_CORE_01976}
  constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }

  /// @uptrace{SWS_CORE_01977}
  constexpr reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

  /// @uptrace{SWS_CORE_01978}
  constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

  /// @uptrace{SWS_CORE_01979}
  constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

private:
  pointer mData;
  size_type mExtent;
};

/// @uptrace{SWS_CORE_01990}
template <typename T>
constexpr Span<T> MakeSpan(T* ptr, typename Span<T>::size_type count) {
  return Span<T>(ptr, count);
}

/// @uptrace{SWS_CORE_01991}
template <typename T>
constexpr Span<T> MakeSpan(T* firstElem, T* lastElem) {
  return Span<T>(firstElem, lastElem);
}

/// @uptrace{SWS_CORE_01992}
template <typename T, std::size_t N>
constexpr Span<T, N> MakeSpan(T (&arr)[N]) noexcept {
  return Span<T, N>(arr);
}

/// @uptrace{SWS_CORE_01993}
template <typename Container>
constexpr Span<typename Container::value_type> MakeSpan(Container& cont) {
  return Span<typename Container::value_type>(cont);
}

/// @uptrace{SWS_CORE_01994}
template <typename Container>
constexpr Span<typename Container::value_type const> MakeSpan(const Container& cont) {
  return Span<typename Container::value_type const>(cont);
}

template <typename ElementType, size_t Extent>
inline Span<const Byte, Extent == dynamic_extent ? dynamic_extent : Extent * sizeof(ElementType)> as_bytes(
    Span<ElementType, Extent> s) noexcept {
  auto data = reinterpret_cast<const Byte*>(s.data());
  auto size = s.size_bytes();
  constexpr auto extent = Extent == dynamic_extent ? dynamic_extent : Extent * sizeof(ElementType);
  return Span<const Byte, Extent> {data, size};
}

template <typename ElementType, size_t Extent>
inline Span<Byte, Extent == dynamic_extent ? dynamic_extent : Extent * sizeof(ElementType)> as_writable_bytes(
    Span<ElementType, Extent> s) noexcept {
  auto data = reinterpret_cast<Byte*>(s.data());
  auto size = s.size_bytes();
  constexpr auto extent = Extent == dynamic_extent ? dynamic_extent : Extent * sizeof(ElementType);
  return Span<Byte, Extent> {data, size};
}

}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_SPAN_H_

#ifndef TUSIMPLEAP_ARA_CORE_VARIANT_INL_HPP_
#define TUSIMPLEAP_ARA_CORE_VARIANT_INL_HPP_

/*
   Variant synopsis

namespace std {

  // 20.7.2, class template Variant
  template <class... Types>
  class Variant {
  public:

    // 20.7.2.1, constructors
    constexpr Variant() noexcept(see below);
    Variant(const Variant&);
    Variant(Variant&&) noexcept(see below);

    template <class T> constexpr Variant(T&&) noexcept(see below);

    template <class T, class... Args>
    constexpr explicit Variant(in_place_type_t<T>, Args&&...);

    template <class T, class U, class... Args>
    constexpr explicit Variant(
        in_place_type_t<T>, initializer_list<U>, Args&&...);

    template <size_t I, class... Args>
    constexpr explicit Variant(in_place_index_t<I>, Args&&...);

    template <size_t I, class U, class... Args>
    constexpr explicit Variant(
        in_place_index_t<I>, initializer_list<U>, Args&&...);

    // 20.7.2.2, destructor
    ~Variant();

    // 20.7.2.3, assignment
    Variant& operator=(const Variant&);
    Variant& operator=(Variant&&) noexcept(see below);

    template <class T> Variant& operator=(T&&) noexcept(see below);

    // 20.7.2.4, modifiers
    template <class T, class... Args>
    T& emplace(Args&&...);

    template <class T, class U, class... Args>
    T& emplace(initializer_list<U>, Args&&...);

    template <size_t I, class... Args>
    variant_alternative<I, Variant>& emplace(Args&&...);

    template <size_t I, class U, class...  Args>
    variant_alternative<I, Variant>& emplace(initializer_list<U>, Args&&...);

    // 20.7.2.5, value status
    constexpr bool valueless_by_exception() const noexcept;
    constexpr size_t index() const noexcept;

    // 20.7.2.6, swap
    void swap(Variant&) noexcept(see below);
  };

  // 20.7.3, Variant helper classes
  template <class T> struct variant_size; // undefined

  template <class T>
  constexpr size_t variant_size_v = variant_size<T>::value;

  template <class T> struct variant_size<const T>;
  template <class T> struct variant_size<volatile T>;
  template <class T> struct variant_size<const volatile T>;

  template <class... Types>
  struct variant_size<Variant<Types...>>;

  template <size_t I, class T> struct variant_alternative; // undefined

  template <size_t I, class T>
  using variant_alternative_t = typename variant_alternative<I, T>::type;

  template <size_t I, class T> struct variant_alternative<I, const T>;
  template <size_t I, class T> struct variant_alternative<I, volatile T>;
  template <size_t I, class T> struct variant_alternative<I, const volatile T>;

  template <size_t I, class... Types>
  struct variant_alternative<I, Variant<Types...>>;

  constexpr size_t variant_npos = -1;

  // 20.7.4, value access
  template <class T, class... Types>
  constexpr bool holds_alternative(const Variant<Types...>&) noexcept;

  template <size_t I, class... Types>
  constexpr variant_alternative_t<I, Variant<Types...>>&
  get(Variant<Types...>&);

  template <size_t I, class... Types>
  constexpr variant_alternative_t<I, Variant<Types...>>&&
  get(Variant<Types...>&&);

  template <size_t I, class... Types>
  constexpr variant_alternative_t<I, Variant<Types...>> const&
  get(const Variant<Types...>&);

  template <size_t I, class... Types>
  constexpr variant_alternative_t<I, Variant<Types...>> const&&
  get(const Variant<Types...>&&);

  template <class T, class...  Types>
  constexpr T& get(Variant<Types...>&);

  template <class T, class... Types>
  constexpr T&& get(Variant<Types...>&&);

  template <class T, class... Types>
  constexpr const T& get(const Variant<Types...>&);

  template <class T, class... Types>
  constexpr const T&& get(const Variant<Types...>&&);

  template <size_t I, class... Types>
  constexpr variant_add_pointer_t<variant_alternative_t<I, Variant<Types...>>>
  get_if(Variant<Types...>*) noexcept;

  template <size_t I, class... Types>
  constexpr variant_add_pointer_t<const variant_alternative_t<I, Variant<Types...>>>
  get_if(const Variant<Types...>*) noexcept;

  template <class T, class... Types>
  constexpr variant_add_pointer_t<T>
  get_if(Variant<Types...>*) noexcept;

  template <class T, class... Types>
  constexpr variant_add_pointer_t<const T>
  get_if(const Variant<Types...>*) noexcept;

  // 20.7.5, relational operators
  template <class... Types>
  constexpr bool operator==(const Variant<Types...>&, const Variant<Types...>&);

  template <class... Types>
  constexpr bool operator!=(const Variant<Types...>&, const Variant<Types...>&);

  template <class... Types>
  constexpr bool operator<(const Variant<Types...>&, const Variant<Types...>&);

  template <class... Types>
  constexpr bool operator>(const Variant<Types...>&, const Variant<Types...>&);

  template <class... Types>
  constexpr bool operator<=(const Variant<Types...>&, const Variant<Types...>&);

  template <class... Types>
  constexpr bool operator>=(const Variant<Types...>&, const Variant<Types...>&);

  // 20.7.6, visitation
  template <class Visitor, class... Variants>
  constexpr see below visit(Visitor&&, Variants&&...);

  // 20.7.7, class Monostate
  struct Monostate;

  // 20.7.8, Monostate relational operators
  constexpr bool operator<(Monostate, Monostate) noexcept;
  constexpr bool operator>(Monostate, Monostate) noexcept;
  constexpr bool operator<=(Monostate, Monostate) noexcept;
  constexpr bool operator>=(Monostate, Monostate) noexcept;
  constexpr bool operator==(Monostate, Monostate) noexcept;
  constexpr bool operator!=(Monostate, Monostate) noexcept;

  // 20.7.9, specialized algorithms
  template <class... Types>
  void swap(Variant<Types...>&, Variant<Types...>&) noexcept(see below);

  // 20.7.10, class bad_variant_access
  class bad_variant_access;

  // 20.7.11, hash support
  template <class T> struct hash;
  template <class... Types> struct hash<Variant<Types...>>;
  template <> struct hash<Monostate>;

} // namespace std

*/

#include <cstddef>
#include <memory>
#include <functional>
#include <type_traits>
#include <utility>

#include "ara/core/utility.h"

namespace ara {
namespace core {

using namespace std;
/***************************All Variant config begin************************/

// MSVC 2015 Update 3.
#if __cplusplus < 201103L && (!defined(_MSC_VER) || _MSC_FULL_VER < 190024210)
#error "MPark.Variant requires C++11 support."
#endif

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#ifndef __has_include
#define __has_include(x) 0
#endif

#ifndef __has_feature
#define __has_feature(x) 0
#endif

#if __has_attribute(always_inline) || defined(__GNUC__)
#define ARA_CORE_ALWAYS_INLINE __attribute__((__always_inline__)) inline
#elif defined(_MSC_VER)
#define ARA_CORE_ALWAYS_INLINE __forceinline
#else
#define ARA_CORE_ALWAYS_INLINE inline
#endif

#if __has_builtin(__builtin_addressof) || \
    (defined(__GNUC__) && __GNUC__ >= 7) || defined(_MSC_VER)
#define ARA_CORE_BUILTIN_ADDRESSOF
#endif

#if __has_builtin(__builtin_unreachable) || defined(__GNUC__)
#define ARA_CORE_BUILTIN_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define ARA_CORE_BUILTIN_UNREACHABLE __assume(false)
#else
#define ARA_CORE_BUILTIN_UNREACHABLE
#endif

#if __has_builtin(__type_pack_element) && !(defined(__ICC))
#define ARA_CORE_TYPE_PACK_ELEMENT
#endif

#if defined(__cpp_constexpr) && __cpp_constexpr >= 200704 && \
    !(defined(__GNUC__) && __GNUC__ == 4 && __GNUC_MINOR__ == 9)
#define ARA_CORE_CPP11_CONSTEXPR
#endif

#if defined(__cpp_constexpr) && __cpp_constexpr >= 201304
#define ARA_CORE_CPP14_CONSTEXPR
#endif

#if defined(__cpp_generic_lambdas) || defined(_MSC_VER)
#define ARA_CORE_GENERIC_LAMBDAS
#endif

#if defined(__cpp_lib_integer_sequence)
#define ARA_CORE_INTEGER_SEQUENCE
#endif

#if (defined(__cpp_decltype_auto) && defined(__cpp_return_type_deduction)) || defined(_MSC_VER)
#define ARA_CORE_RETURN_TYPE_DEDUCTION
#endif

#if defined(__cpp_lib_transparent_operators) || defined(_MSC_VER)
#define ARA_CORE_TRANSPARENT_OPERATORS
#endif

#if defined(__cpp_variable_templates) || defined(_MSC_VER)
#define ARA_CORE_VARIABLE_TEMPLATES
#endif

#if !defined(__GLIBCXX__) || __has_include(<codecvt>)  // >= libstdc++-5
#define ARA_CORE_TRIVIALITY_TYPE_TRAITS
#define ARA_CORE_INCOMPLETE_TYPE_TRAITS
#endif

/***************************All Variant config end************************/

/***************************Variant using names begin**************************/
#define ARA_CORE_RETURN(...) \
  noexcept(noexcept(__VA_ARGS__)) -> decltype(__VA_ARGS__) { return __VA_ARGS__; }

template <typename T>
struct variant_identity { using type = T; };

// inline namespace cpp14 {
  template <typename T, std::size_t N>
  struct variant_array {
    constexpr const T &operator[](std::size_t index) const {
      return data[index];
    }

    T data[N == 0 ? 1 : N];
  };

  template <typename T>
  using variant_add_pointer_t = typename std::add_pointer<T>::type;

  template <typename... Ts>
  using variant_common_type_t = typename std::common_type<Ts...>::type;

  template <typename T>
  using variant_decay_t = typename std::decay<T>::type;

  template <bool B, typename T = void>
  using variant_enable_if_t = typename std::enable_if<B, T>::type;

  template <typename T>
  using variant_remove_const_t = typename std::remove_const<T>::type;

  template <typename T>
  using variant_remove_reference_t = typename std::remove_reference<T>::type;

  template <typename T>
  using variant_remove_cvref_t =
      typename std::remove_cv<variant_remove_reference_t<T>>::type;

  template <typename T>
  inline constexpr T &&variant_forward(variant_remove_reference_t<T> &t) noexcept {
    return static_cast<T &&>(t);
  }

  template <typename T>
  inline constexpr T &&variant_forward(variant_remove_reference_t<T> &&t) noexcept {
    static_assert(!std::is_lvalue_reference<T>::value,
                  "can not variant_forward an rvalue as an lvalue");
    return static_cast<T &&>(t);
  }

  template <typename T>
  inline constexpr variant_remove_reference_t<T> &&variant_move(T &&t) noexcept {
    return static_cast<variant_remove_reference_t<T> &&>(t);
  }

#ifdef ARA_CORE_INTEGER_SEQUENCE
  using std::integer_sequence;
  using std::index_sequence;
  using std::make_index_sequence;
  using std::index_sequence_for;
#else
  template <typename T, T... Is>
  struct integer_sequence {
    using value_type = T;
    static constexpr std::size_t size() noexcept { return sizeof...(Is); }
  };

  template <std::size_t... Is>
  using index_sequence = integer_sequence<std::size_t, Is...>;

  template <typename Lhs, typename Rhs>
  struct make_index_sequence_concat;

  template <std::size_t... Lhs, std::size_t... Rhs>
  struct make_index_sequence_concat<index_sequence<Lhs...>,
                                    index_sequence<Rhs...>>
      : variant_identity<index_sequence<Lhs..., (sizeof...(Lhs) + Rhs)...>> {};

  template <std::size_t N>
  struct make_index_sequence_impl;

  template <std::size_t N>
  using make_index_sequence = typename make_index_sequence_impl<N>::type;

  template <std::size_t N>
  struct make_index_sequence_impl
      : make_index_sequence_concat<make_index_sequence<N / 2>,
                                   make_index_sequence<N - (N / 2)>> {};

  template <>
  struct make_index_sequence_impl<0> : variant_identity<index_sequence<>> {};

  template <>
  struct make_index_sequence_impl<1> : variant_identity<index_sequence<0>> {};

  template <typename... Ts>
  using index_sequence_for = make_index_sequence<sizeof...(Ts)>;
#endif

  // <functional>
#ifdef ARA_CORE_TRANSPARENT_OPERATORS
  using variant_equal_to = std::equal_to<>;
#else
  struct variant_equal_to {
    template <typename Lhs, typename Rhs>
    inline constexpr auto operator()(Lhs &&lhs, Rhs &&rhs) const
      ARA_CORE_RETURN(variant_forward<Lhs>(lhs) == variant_forward<Rhs>(rhs))
  };
#endif

#ifdef ARA_CORE_TRANSPARENT_OPERATORS
  using variant_not_equal_to = std::not_equal_to<>;
#else
  struct variant_not_equal_to {
    template <typename Lhs, typename Rhs>
    inline constexpr auto operator()(Lhs &&lhs, Rhs &&rhs) const
      ARA_CORE_RETURN(variant_forward<Lhs>(lhs) != variant_forward<Rhs>(rhs))
  };
#endif

#ifdef ARA_CORE_TRANSPARENT_OPERATORS
  using variant_less = std::less<>;
#else
  struct variant_less {
    template <typename Lhs, typename Rhs>
    inline constexpr auto operator()(Lhs &&lhs, Rhs &&rhs) const
      ARA_CORE_RETURN(variant_forward<Lhs>(lhs) < variant_forward<Rhs>(rhs))
  };
#endif

#ifdef ARA_CORE_TRANSPARENT_OPERATORS
  using variant_greater = std::greater<>;
#else
  struct variant_greater {
    template <typename Lhs, typename Rhs>
    inline constexpr auto operator()(Lhs &&lhs, Rhs &&rhs) const
      ARA_CORE_RETURN(variant_forward<Lhs>(lhs) > variant_forward<Rhs>(rhs))
  };
#endif

#ifdef ARA_CORE_TRANSPARENT_OPERATORS
  using variant_less_equal = std::less_equal<>;
#else
  struct variant_less_equal {
    template <typename Lhs, typename Rhs>
    inline constexpr auto operator()(Lhs &&lhs, Rhs &&rhs) const
      ARA_CORE_RETURN(variant_forward<Lhs>(lhs) <= variant_forward<Rhs>(rhs))
  };
#endif

#ifdef ARA_CORE_TRANSPARENT_OPERATORS
  using variant_greater_equal = std::greater_equal<>;
#else
  struct variant_greater_equal {
    template <typename Lhs, typename Rhs>
    inline constexpr auto operator()(Lhs &&lhs, Rhs &&rhs) const
      ARA_CORE_RETURN(variant_forward<Lhs>(lhs) >= variant_forward<Rhs>(rhs))
  };
#endif
// }  // namespace cpp14

// inline namespace cpp17 {

  // <type_traits>
  template <bool B>
  using variant_bool_constant = std::integral_constant<bool, B>;

  template <typename...>
  struct variant_voider : variant_identity<void> {};

  template <typename... Ts>
  using variant_void_t = typename variant_voider<Ts...>::type;

  namespace variant_detail {
    namespace swappable {

      using std::swap;

      template <typename T>
      struct is_swappable {
        private:
        template <typename U,
                  typename = decltype(swap(std::declval<U &>(),
                                           std::declval<U &>()))>
        inline static std::true_type test(int);

        template <typename U>
        inline static std::false_type test(...);

        public:
        static constexpr bool value = decltype(test<T>(0))::value;
      };

      template <bool IsSwappable, typename T>
      struct is_nothrow_swappable {
        static constexpr bool value =
            noexcept(swap(std::declval<T &>(), std::declval<T &>()));
      };

      template <typename T>
      struct is_nothrow_swappable<false, T> : std::false_type {};

    }  // namespace swappable
  }  // namespace variant_detail

  using variant_detail::swappable::is_swappable;

  template <typename T>
  using is_nothrow_swappable =
      variant_detail::swappable::is_nothrow_swappable<is_swappable<T>::value, T>;

  // <functional>
  namespace variant_detail {

    template <typename T>
    struct is_reference_wrapper : std::false_type {};

    template <typename T>
    struct is_reference_wrapper<std::reference_wrapper<T>>
        : std::true_type {};

    template <bool, int>
    struct Invoke;

    template <>
    struct Invoke<true /* pmf */, 0 /* is_base_of */> {
      template <typename R, typename T, typename Arg, typename... Args>
      inline static constexpr auto invoke(R T::*pmf, Arg &&arg, Args &&... args)
        ARA_CORE_RETURN((variant_forward<Arg>(arg).*pmf)(variant_forward<Args>(args)...))
    };

    template <>
    struct Invoke<true /* pmf */, 1 /* is_reference_wrapper */> {
      template <typename R, typename T, typename Arg, typename... Args>
      inline static constexpr auto invoke(R T::*pmf, Arg &&arg, Args &&... args)
        ARA_CORE_RETURN((variant_forward<Arg>(arg).get().*pmf)(variant_forward<Args>(args)...))
    };

    template <>
    struct Invoke<true /* pmf */, 2 /* otherwise */> {
      template <typename R, typename T, typename Arg, typename... Args>
      inline static constexpr auto invoke(R T::*pmf, Arg &&arg, Args &&... args)
        ARA_CORE_RETURN(((*variant_forward<Arg>(arg)).*pmf)(variant_forward<Args>(args)...))
    };

    template <>
    struct Invoke<false /* pmo */, 0 /* is_base_of */> {
      template <typename R, typename T, typename Arg>
      inline static constexpr auto invoke(R T::*pmo, Arg &&arg)
        ARA_CORE_RETURN(variant_forward<Arg>(arg).*pmo)
    };

    template <>
    struct Invoke<false /* pmo */, 1 /* is_reference_wrapper */> {
      template <typename R, typename T, typename Arg>
      inline static constexpr auto invoke(R T::*pmo, Arg &&arg)
        ARA_CORE_RETURN(variant_forward<Arg>(arg).get().*pmo)
    };

    template <>
    struct Invoke<false /* pmo */, 2 /* otherwise */> {
      template <typename R, typename T, typename Arg>
      inline static constexpr auto invoke(R T::*pmo, Arg &&arg)
          ARA_CORE_RETURN((*variant_forward<Arg>(arg)).*pmo)
    };

    template <typename R, typename T, typename Arg, typename... Args>
    inline constexpr auto invoke(R T::*f, Arg &&arg, Args &&... args)
      ARA_CORE_RETURN(
          Invoke<std::is_function<R>::value,
                 (std::is_base_of<T, variant_decay_t<Arg>>::value
                      ? 0
                      : is_reference_wrapper<variant_decay_t<Arg>>::value
                            ? 1
                            : 2)>::invoke(f,
                                          variant_forward<Arg>(arg),
                                          variant_forward<Args>(args)...))

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)
#endif
    template <typename F, typename... Args>
    inline constexpr auto invoke(F &&f, Args &&... args)
      ARA_CORE_RETURN(variant_forward<F>(f)(variant_forward<Args>(args)...))
#ifdef _MSC_VER
#pragma warning(pop)
#endif
  }  // namespace variant_detail

  template <typename F, typename... Args>
  inline constexpr auto variant_invoke(F &&f, Args &&... args)
    ARA_CORE_RETURN(variant_detail::invoke(variant_forward<F>(f),
                                variant_forward<Args>(args)...))

  namespace variant_detail {

    template <typename Void, typename, typename...>
    struct invoke_result {};

    template <typename F, typename... Args>
    struct invoke_result<variant_void_t<decltype(variant_invoke(
                             std::declval<F>(), std::declval<Args>()...))>,
                         F,
                         Args...>
        : variant_identity<decltype(
              variant_invoke(std::declval<F>(), std::declval<Args>()...))> {};

  }  // namespace variant_detail

  template <typename F, typename... Args>
  using variant_invoke_result = variant_detail::invoke_result<void, F, Args...>;

  template <typename F, typename... Args>
  using variant_invoke_result_t = typename variant_invoke_result<F, Args...>::type;

  namespace variant_detail {

    template <typename Void, typename, typename...>
    struct is_invocable : std::false_type {};

    template <typename F, typename... Args>
    struct is_invocable<variant_void_t<variant_invoke_result_t<F, Args...>>, F, Args...>
        : std::true_type {};

    template <typename Void, typename, typename, typename...>
    struct is_invocable_r : std::false_type {};

    template <typename R, typename F, typename... Args>
    struct is_invocable_r<variant_void_t<variant_invoke_result_t<F, Args...>>,
                          R,
                          F,
                          Args...>
        : std::is_convertible<variant_invoke_result_t<F, Args...>, R> {};

  }  // namespace variant_detail

  template <typename F, typename... Args>
  using variant_is_invocable = variant_detail::is_invocable<void, F, Args...>;

  template <typename R, typename F, typename... Args>
  using variant_is_invocable_r = variant_detail::is_invocable_r<void, R, F, Args...>;

  // <memory>
#ifdef ARA_CORE_BUILTIN_ADDRESSOF
  template <typename T>
  inline constexpr T *variant_addressof(T &arg) noexcept {
    return __builtin_addressof(arg);
  }
#else
  namespace variant_detail {

    namespace has_addressof_impl {

      struct fail;

      template <typename T>
      inline fail operator&(T &&);

      template <typename T>
      inline static constexpr bool impl() {
        return (std::is_class<T>::value || std::is_union<T>::value) &&
               !std::is_same<decltype(&std::declval<T &>()), fail>::value;
      }

    }  // namespace has_addressof_impl

    template <typename T>
    using has_addressof = variant_bool_constant<has_addressof_impl::impl<T>()>;

    template <typename T>
    inline constexpr T *addressof(T &arg, std::true_type) noexcept {
      return std::addressof(arg);
    }

    template <typename T>
    inline constexpr T *addressof(T &arg, std::false_type) noexcept {
      return &arg;
    }

  }  // namespace variant_detail

  template <typename T>
  inline constexpr T *variant_addressof(T &arg) noexcept {
    return variant_detail::addressof(arg, variant_detail::has_addressof<T>{});
  }
#endif

  template <typename T>
  inline constexpr T *variant_addressof(const T &&) = delete;

// }  // namespace cpp17

template <typename T>
struct variant_remove_all_extents : variant_identity<T> {};

template <typename T, std::size_t N>
struct variant_remove_all_extents<variant_array<T, N>> : variant_remove_all_extents<T> {};

template <typename T>
using variant_remove_all_extents_t = typename variant_remove_all_extents<T>::type;

template <std::size_t N>
using variant_size_constant = std::integral_constant<std::size_t, N>;

template <std::size_t I, typename T>
struct variant_indexed_type : variant_size_constant<I> { using type = T; };

template <bool... Bs>
using variant_all = std::is_same<integer_sequence<bool, true, Bs...>,
                             integer_sequence<bool, Bs..., true>>;

#ifdef ARA_CORE_TYPE_PACK_ELEMENT
template <std::size_t I, typename... Ts>
using variant_type_pack_element_t = __type_pack_element<I, Ts...>;
#else
template <std::size_t I, typename... Ts>
struct variant_type_pack_element_impl {
  private:
  template <typename>
  struct set;

  template <std::size_t... Is>
  struct set<index_sequence<Is...>> : variant_indexed_type<Is, Ts>... {};

  template <typename T>
  inline static std::enable_if<true, T> impl(variant_indexed_type<I, T>);

  inline static std::enable_if<false> impl(...);

  public:
  using type = decltype(impl(set<index_sequence_for<Ts...>>{}));
};

template <std::size_t I, typename... Ts>
using variant_type_pack_element = typename variant_type_pack_element_impl<I, Ts...>::type;

template <std::size_t I, typename... Ts>
using variant_type_pack_element_t = typename variant_type_pack_element<I, Ts...>::type;
#endif

#ifdef ARA_CORE_TRIVIALITY_TYPE_TRAITS
using std::is_trivially_copy_constructible;
using std::is_trivially_move_constructible;
using std::is_trivially_copy_assignable;
using std::is_trivially_move_assignable;
#else
template <typename T>
struct is_trivially_copy_constructible
    : variant_bool_constant<
          std::is_copy_constructible<T>::value && __has_trivial_copy(T)> {};

template <typename T>
struct is_trivially_move_constructible : variant_bool_constant<__is_trivial(T)> {};

template <typename T>
struct is_trivially_copy_assignable
    : variant_bool_constant<
          std::is_copy_assignable<T>::value && __has_trivial_assign(T)> {};

template <typename T>
struct is_trivially_move_assignable : variant_bool_constant<__is_trivial(T)> {};
#endif

template <typename T, bool>
struct variant_dependent_type : T {};

template <typename Is, std::size_t J>
struct variant_push_back;

template <typename Is, std::size_t J>
using variant_push_back_t = typename variant_push_back<Is, J>::type;

template <std::size_t... Is, std::size_t J>
struct variant_push_back<index_sequence<Is...>, J> {
  using type = index_sequence<Is..., J>;
};

/***************************Variant using names end***************************/

}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_VARIANT_INL_HPP_

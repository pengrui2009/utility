#ifndef TUSIMPLEAP_ARA_CORE_VARIANT_HPP_
#define TUSIMPLEAP_ARA_CORE_VARIANT_HPP_

#include <exception>
#include <initializer_list>
#include <limits>
#include <new>

#include "ara/core/variant_inl.h"

namespace ara {
namespace core {

#ifdef ARA_CORE_RETURN_TYPE_DEDUCTION

#define AUTO auto
#define AUTO_RETURN(...) { return __VA_ARGS__; }

#define AUTO_REFREF auto &&
#define AUTO_REFREF_RETURN(...) { return __VA_ARGS__; }

#define DECLTYPE_AUTO decltype(auto)
#define DECLTYPE_AUTO_RETURN(...) { return __VA_ARGS__; }

#else

#define AUTO auto
#define AUTO_RETURN(...) \
  -> variant_decay_t<decltype(__VA_ARGS__)> { return __VA_ARGS__; }

#define AUTO_REFREF auto
#define AUTO_REFREF_RETURN(...)                                           \
  -> decltype((__VA_ARGS__)) {                                            \
    static_assert(std::is_reference<decltype((__VA_ARGS__))>::value, ""); \
    return __VA_ARGS__;                                                   \
  }

#define DECLTYPE_AUTO auto
#define DECLTYPE_AUTO_RETURN(...) \
  -> decltype(__VA_ARGS__) { return __VA_ARGS__; }

#endif

  class bad_variant_access : public std::exception {
    public:
    virtual const char *what() const noexcept override { return "bad_variant_access"; }
  };

  [[noreturn]] inline void throw_bad_variant_access() {
    throw bad_variant_access{};
  }

  template <typename... Ts>
  class Variant;

  template <typename T>
  struct variant_size;

#ifdef ARA_CORE_VARIABLE_TEMPLATES
  template <typename T>
  constexpr std::size_t variant_size_v = variant_size<T>::value;
#endif

  template <typename T>
  struct variant_size<const T> : variant_size<T> {};

  template <typename T>
  struct variant_size<volatile T> : variant_size<T> {};

  template <typename T>
  struct variant_size<const volatile T> : variant_size<T> {};

  template <typename... Ts>
  struct variant_size<Variant<Ts...>> : variant_size_constant<sizeof...(Ts)> {};

  template <std::size_t I, typename T>
  struct variant_alternative;

  template <std::size_t I, typename T>
  using variant_alternative_t = typename variant_alternative<I, T>::type;

  template <std::size_t I, typename T>
  struct variant_alternative<I, const T>
      : std::add_const<variant_alternative_t<I, T>> {};

  template <std::size_t I, typename T>
  struct variant_alternative<I, volatile T>
      : std::add_volatile<variant_alternative_t<I, T>> {};

  template <std::size_t I, typename T>
  struct variant_alternative<I, const volatile T>
      : std::add_cv<variant_alternative_t<I, T>> {};

  template <std::size_t I, typename... Ts>
  struct variant_alternative<I, Variant<Ts...>> {
    static_assert(I < sizeof...(Ts),
                  "index out of bounds in `std::variant_alternative<>`");
    using type = variant_type_pack_element_t<I, Ts...>;
  };

  constexpr std::size_t variant_npos = static_cast<std::size_t>(-1);

  namespace variant_detail {

    constexpr std::size_t not_found = static_cast<std::size_t>(-1);
    constexpr std::size_t ambiguous = static_cast<std::size_t>(-2);

#ifdef ARA_CORE_CPP14_CONSTEXPR
    template <typename T, typename... Ts>
    inline constexpr std::size_t find_index() {
      constexpr variant_array<bool, sizeof...(Ts)> matches = {
          {std::is_same<T, Ts>::value...}
      };
      std::size_t result = not_found;
      for (std::size_t i = 0; i < sizeof...(Ts); ++i) {
        if (matches[i]) {
          if (result != not_found) {
            return ambiguous;
          }
          result = i;
        }
      }
      return result;
    }
#else
    inline constexpr std::size_t find_index_impl(std::size_t result,
                                                 std::size_t) {
      return result;
    }

    template <typename... Bs>
    inline constexpr std::size_t find_index_impl(std::size_t result,
                                                 std::size_t idx,
                                                 bool b,
                                                 Bs... bs) {
      return b ? (result != not_found ? ambiguous
                                      : find_index_impl(idx, idx + 1, bs...))
               : find_index_impl(result, idx + 1, bs...);
    }

    template <typename T, typename... Ts>
    inline constexpr std::size_t find_index() {
      return find_index_impl(not_found, 0, std::is_same<T, Ts>::value...);
    }
#endif

    template <std::size_t I>
    using find_index_sfinae_impl =
        variant_enable_if_t<I != not_found && I != ambiguous,
                         variant_size_constant<I>>;

    template <typename T, typename... Ts>
    using find_index_sfinae = find_index_sfinae_impl<find_index<T, Ts...>()>;

    template <std::size_t I>
    struct find_index_checked_impl : variant_size_constant<I> {
      static_assert(I != not_found, "the specified type is not found.");
      static_assert(I != ambiguous, "the specified type is ambiguous.");
    };

    template <typename T, typename... Ts>
    using find_index_checked = find_index_checked_impl<find_index<T, Ts...>()>;

    struct valueless_t {};

    enum class Trait { TriviallyAvailable, Available, Unavailable };

    template <typename T,
              template <typename> class IsTriviallyAvailable,
              template <typename> class IsAvailable>
    inline constexpr Trait trait() {
      return IsTriviallyAvailable<T>::value
                 ? Trait::TriviallyAvailable
                 : IsAvailable<T>::value ? Trait::Available
                                         : Trait::Unavailable;
    }

#ifdef ARA_CORE_CPP14_CONSTEXPR
    template <typename... Traits>
    inline constexpr Trait common_trait(Traits... traits_) {
      Trait result = Trait::TriviallyAvailable;
      variant_array<Trait, sizeof...(Traits)> traits = {{traits_...}};
      for (std::size_t i = 0; i < sizeof...(Traits); ++i) {
        Trait t = traits[i];
        if (static_cast<int>(t) > static_cast<int>(result)) {
          result = t;
        }
      }
      return result;
    }
#else
    inline constexpr Trait common_trait_impl(Trait result) { return result; }

    template <typename... Traits>
    inline constexpr Trait common_trait_impl(Trait result,
                                             Trait t,
                                             Traits... ts) {
      return static_cast<int>(t) > static_cast<int>(result)
                 ? common_trait_impl(t, ts...)
                 : common_trait_impl(result, ts...);
    }

    template <typename... Traits>
    inline constexpr Trait common_trait(Traits... ts) {
      return common_trait_impl(Trait::TriviallyAvailable, ts...);
    }
#endif

    template <typename... Ts>
    struct traits {
      static constexpr Trait copy_constructible_trait =
          common_trait(trait<Ts,
                             is_trivially_copy_constructible,
                             std::is_copy_constructible>()...);

      static constexpr Trait move_constructible_trait =
          common_trait(trait<Ts,
                             is_trivially_move_constructible,
                             std::is_move_constructible>()...);

      static constexpr Trait copy_assignable_trait =
          common_trait(copy_constructible_trait,
                       trait<Ts,
                             is_trivially_copy_assignable,
                             std::is_copy_assignable>()...);

      static constexpr Trait move_assignable_trait =
          common_trait(move_constructible_trait,
                       trait<Ts,
                             is_trivially_move_assignable,
                             std::is_move_assignable>()...);

      static constexpr Trait destructible_trait =
          common_trait(trait<Ts,
                             std::is_trivially_destructible,
                             std::is_destructible>()...);
    };

    namespace access {

      struct recursive_union {
#ifdef ARA_CORE_RETURN_TYPE_DEDUCTION
        template <typename V>
        inline static constexpr auto &&get_alt(V &&v, in_place_index_t<0>) {
          return variant_forward<V>(v).head_;
        }

        template <typename V, std::size_t I>
        inline static constexpr auto &&get_alt(V &&v, in_place_index_t<I>) {
          return get_alt(variant_forward<V>(v).tail_, in_place_index_t<I - 1>{});
        }
#else
        template <std::size_t I, bool Dummy = true>
        struct get_alt_impl {
          template <typename V>
          inline constexpr AUTO_REFREF operator()(V &&v) const
            AUTO_REFREF_RETURN(get_alt_impl<I - 1>{}(variant_forward<V>(v).tail_))
        };

        template <bool Dummy>
        struct get_alt_impl<0, Dummy> {
          template <typename V>
          inline constexpr AUTO_REFREF operator()(V &&v) const
            AUTO_REFREF_RETURN(variant_forward<V>(v).head_)
        };

        template <typename V, std::size_t I>
        inline static constexpr AUTO_REFREF get_alt(V &&v, in_place_index_t<I>)
          AUTO_REFREF_RETURN(get_alt_impl<I>{}(variant_forward<V>(v)))
#endif
      };

      struct base {
        template <std::size_t I, typename V>
        inline static constexpr AUTO_REFREF get_alt(V &&v)
#ifdef _MSC_VER
          AUTO_REFREF_RETURN(recursive_union::get_alt(
              variant_forward<V>(v).data_, in_place_index_t<I>{}))
#else
          AUTO_REFREF_RETURN(recursive_union::get_alt(
              data(variant_forward<V>(v)), in_place_index_t<I>{}))
#endif
      };

      struct Variant {
        template <std::size_t I, typename V>
        inline static constexpr AUTO_REFREF get_alt(V &&v)
          AUTO_REFREF_RETURN(base::get_alt<I>(variant_forward<V>(v).impl_))
      };

    }  // namespace access

    namespace visitation {

#if defined(ARA_CORE_CPP14_CONSTEXPR) && !defined(_MSC_VER)
#define ARA_CORE_VARIANT_SWITCH_VISIT
#endif

      struct base {
        template <typename Visitor, typename... Vs>
        using dispatch_result_t = decltype(
            variant_invoke(std::declval<Visitor>(),
                        access::base::get_alt<0>(std::declval<Vs>())...));

        template <typename Expected>
        struct expected {
          template <typename Actual>
          inline static constexpr bool but_got() {
            return std::is_same<Expected, Actual>::value;
          }
        };

        template <typename Expected, typename Actual>
        struct visit_return_type_check {
          static_assert(
              expected<Expected>::template but_got<Actual>(),
              "`visit` requires the visitor to have a single return type");

          template <typename Visitor, typename... Alts>
          inline static constexpr DECLTYPE_AUTO invoke(Visitor &&visitor,
                                                       Alts &&... alts)
            DECLTYPE_AUTO_RETURN(variant_invoke(variant_forward<Visitor>(visitor),
                                             variant_forward<Alts>(alts)...))
        };

#ifdef ARA_CORE_VARIANT_SWITCH_VISIT
        template <bool B, typename R, typename... ITs>
        struct dispatcher;

        template <typename R, typename... ITs>
        struct dispatcher<false, R, ITs...> {
          template <std::size_t B, typename F, typename... Vs>
          ARA_CORE_ALWAYS_INLINE static constexpr R dispatch(
              F &&, typename ITs::type &&..., Vs &&...) {
            ARA_CORE_BUILTIN_UNREACHABLE;
          }

          template <std::size_t I, typename F, typename... Vs>
          ARA_CORE_ALWAYS_INLINE static constexpr R dispatch_case(F &&, Vs &&...) {
            ARA_CORE_BUILTIN_UNREACHABLE;
          }

          template <std::size_t B, typename F, typename... Vs>
          ARA_CORE_ALWAYS_INLINE static constexpr R dispatch_at(std::size_t,
                                                             F &&,
                                                             Vs &&...) {
            ARA_CORE_BUILTIN_UNREACHABLE;
          }
        };

        template <typename R, typename... ITs>
        struct dispatcher<true, R, ITs...> {
          template <std::size_t B, typename F>
          ARA_CORE_ALWAYS_INLINE static constexpr R dispatch(
              F &&f, typename ITs::type &&... visited_vs) {
            using Expected = R;
            using Actual = decltype(variant_invoke(
                variant_forward<F>(f),
                access::base::get_alt<ITs::value>(
                    variant_forward<typename ITs::type>(visited_vs))...));
            return visit_return_type_check<Expected, Actual>::invoke(
                variant_forward<F>(f),
                access::base::get_alt<ITs::value>(
                    variant_forward<typename ITs::type>(visited_vs))...);
          }

          template <std::size_t B, typename F, typename V, typename... Vs>
          ARA_CORE_ALWAYS_INLINE static constexpr R dispatch(
              F &&f, typename ITs::type &&... visited_vs, V &&v, Vs &&... vs) {
#define ARA_CORE_DISPATCH(I)                                                   \
  dispatcher<(I < variant_decay_t<V>::size()),                                 \
             R,                                                             \
             ITs...,                                                        \
             variant_indexed_type<I, V>>::                                     \
      template dispatch<0>(variant_forward<F>(f),                              \
                           variant_forward<typename ITs::type>(visited_vs)..., \
                           variant_forward<V>(v),                              \
                           variant_forward<Vs>(vs)...)

#define ARA_CORE_DEFAULT(I)                                                      \
  dispatcher<(I < variant_decay_t<V>::size()), R, ITs...>::template dispatch<I>( \
      variant_forward<F>(f),                                                     \
      variant_forward<typename ITs::type>(visited_vs)...,                        \
      variant_forward<V>(v),                                                     \
      variant_forward<Vs>(vs)...)

            switch (v.index()) {
              case B + 0: return ARA_CORE_DISPATCH(B + 0);
              case B + 1: return ARA_CORE_DISPATCH(B + 1);
              case B + 2: return ARA_CORE_DISPATCH(B + 2);
              case B + 3: return ARA_CORE_DISPATCH(B + 3);
              case B + 4: return ARA_CORE_DISPATCH(B + 4);
              case B + 5: return ARA_CORE_DISPATCH(B + 5);
              case B + 6: return ARA_CORE_DISPATCH(B + 6);
              case B + 7: return ARA_CORE_DISPATCH(B + 7);
              case B + 8: return ARA_CORE_DISPATCH(B + 8);
              case B + 9: return ARA_CORE_DISPATCH(B + 9);
              case B + 10: return ARA_CORE_DISPATCH(B + 10);
              case B + 11: return ARA_CORE_DISPATCH(B + 11);
              case B + 12: return ARA_CORE_DISPATCH(B + 12);
              case B + 13: return ARA_CORE_DISPATCH(B + 13);
              case B + 14: return ARA_CORE_DISPATCH(B + 14);
              case B + 15: return ARA_CORE_DISPATCH(B + 15);
              case B + 16: return ARA_CORE_DISPATCH(B + 16);
              case B + 17: return ARA_CORE_DISPATCH(B + 17);
              case B + 18: return ARA_CORE_DISPATCH(B + 18);
              case B + 19: return ARA_CORE_DISPATCH(B + 19);
              case B + 20: return ARA_CORE_DISPATCH(B + 20);
              case B + 21: return ARA_CORE_DISPATCH(B + 21);
              case B + 22: return ARA_CORE_DISPATCH(B + 22);
              case B + 23: return ARA_CORE_DISPATCH(B + 23);
              case B + 24: return ARA_CORE_DISPATCH(B + 24);
              case B + 25: return ARA_CORE_DISPATCH(B + 25);
              case B + 26: return ARA_CORE_DISPATCH(B + 26);
              case B + 27: return ARA_CORE_DISPATCH(B + 27);
              case B + 28: return ARA_CORE_DISPATCH(B + 28);
              case B + 29: return ARA_CORE_DISPATCH(B + 29);
              case B + 30: return ARA_CORE_DISPATCH(B + 30);
              case B + 31: return ARA_CORE_DISPATCH(B + 31);
              default: return ARA_CORE_DEFAULT(B + 32);
            }

#undef ARA_CORE_DEFAULT
#undef ARA_CORE_DISPATCH
          }

          template <std::size_t I, typename F, typename... Vs>
          ARA_CORE_ALWAYS_INLINE static constexpr R dispatch_case(F &&f,
                                                               Vs &&... vs) {
            using Expected = R;
            using Actual = decltype(
                variant_invoke(variant_forward<F>(f),
                            access::base::get_alt<I>(variant_forward<Vs>(vs))...));
            return visit_return_type_check<Expected, Actual>::invoke(
                variant_forward<F>(f),
                access::base::get_alt<I>(variant_forward<Vs>(vs))...);
          }

          template <std::size_t B, typename F, typename V, typename... Vs>
          ARA_CORE_ALWAYS_INLINE static constexpr R dispatch_at(std::size_t index,
                                                             F &&f,
                                                             V &&v,
                                                             Vs &&... vs) {
            static_assert(variant_all<(variant_decay_t<V>::size() ==
                                    variant_decay_t<Vs>::size())...>::value,
                          "variant_all of the variants must be the same size.");
#define ARA_CORE_DISPATCH_AT(I)                                               \
  dispatcher<(I < variant_decay_t<V>::size()), R>::template dispatch_case<I>( \
      variant_forward<F>(f), variant_forward<V>(v), variant_forward<Vs>(vs)...)

#define ARA_CORE_DEFAULT(I)                                                 \
  dispatcher<(I < variant_decay_t<V>::size()), R>::template dispatch_at<I>( \
      index, variant_forward<F>(f), variant_forward<V>(v), variant_forward<Vs>(vs)...)

            switch (index) {
              case B + 0: return ARA_CORE_DISPATCH_AT(B + 0);
              case B + 1: return ARA_CORE_DISPATCH_AT(B + 1);
              case B + 2: return ARA_CORE_DISPATCH_AT(B + 2);
              case B + 3: return ARA_CORE_DISPATCH_AT(B + 3);
              case B + 4: return ARA_CORE_DISPATCH_AT(B + 4);
              case B + 5: return ARA_CORE_DISPATCH_AT(B + 5);
              case B + 6: return ARA_CORE_DISPATCH_AT(B + 6);
              case B + 7: return ARA_CORE_DISPATCH_AT(B + 7);
              case B + 8: return ARA_CORE_DISPATCH_AT(B + 8);
              case B + 9: return ARA_CORE_DISPATCH_AT(B + 9);
              case B + 10: return ARA_CORE_DISPATCH_AT(B + 10);
              case B + 11: return ARA_CORE_DISPATCH_AT(B + 11);
              case B + 12: return ARA_CORE_DISPATCH_AT(B + 12);
              case B + 13: return ARA_CORE_DISPATCH_AT(B + 13);
              case B + 14: return ARA_CORE_DISPATCH_AT(B + 14);
              case B + 15: return ARA_CORE_DISPATCH_AT(B + 15);
              case B + 16: return ARA_CORE_DISPATCH_AT(B + 16);
              case B + 17: return ARA_CORE_DISPATCH_AT(B + 17);
              case B + 18: return ARA_CORE_DISPATCH_AT(B + 18);
              case B + 19: return ARA_CORE_DISPATCH_AT(B + 19);
              case B + 20: return ARA_CORE_DISPATCH_AT(B + 20);
              case B + 21: return ARA_CORE_DISPATCH_AT(B + 21);
              case B + 22: return ARA_CORE_DISPATCH_AT(B + 22);
              case B + 23: return ARA_CORE_DISPATCH_AT(B + 23);
              case B + 24: return ARA_CORE_DISPATCH_AT(B + 24);
              case B + 25: return ARA_CORE_DISPATCH_AT(B + 25);
              case B + 26: return ARA_CORE_DISPATCH_AT(B + 26);
              case B + 27: return ARA_CORE_DISPATCH_AT(B + 27);
              case B + 28: return ARA_CORE_DISPATCH_AT(B + 28);
              case B + 29: return ARA_CORE_DISPATCH_AT(B + 29);
              case B + 30: return ARA_CORE_DISPATCH_AT(B + 30);
              case B + 31: return ARA_CORE_DISPATCH_AT(B + 31);
              default: return ARA_CORE_DEFAULT(B + 32);
            }

#undef ARA_CORE_DEFAULT
#undef ARA_CORE_DISPATCH_AT
          }
        };
#else
        template <typename T>
        inline static constexpr const T &at(const T &elem) noexcept {
          return elem;
        }

        template <typename T, std::size_t N, typename... Is>
        inline static constexpr const variant_remove_all_extents_t<T> &at(
            const variant_array<T, N> &elems, std::size_t i, Is... is) noexcept {
          return at(elems[i], is...);
        }

        template <typename F, typename... Fs>
        inline static constexpr variant_array<variant_decay_t<F>, sizeof...(Fs) + 1>
        make_farray(F &&f, Fs &&... fs) {
          return {{variant_forward<F>(f), variant_forward<Fs>(fs)...}};
        }

        template <typename F, typename... Vs>
        struct make_fmatrix_impl {

          template <std::size_t... Is>
          inline static constexpr dispatch_result_t<F, Vs...> dispatch(
              F &&f, Vs &&... vs) {
            using Expected = dispatch_result_t<F, Vs...>;
            using Actual = decltype(variant_invoke(
                variant_forward<F>(f),
                access::base::get_alt<Is>(variant_forward<Vs>(vs))...));
            return visit_return_type_check<Expected, Actual>::invoke(
                variant_forward<F>(f),
                access::base::get_alt<Is>(variant_forward<Vs>(vs))...);
          }

#ifdef ARA_CORE_RETURN_TYPE_DEDUCTION
          template <std::size_t... Is>
          inline static constexpr auto impl(index_sequence<Is...>) {
            return &dispatch<Is...>;
          }

          template <typename Is, std::size_t... Js, typename... Ls>
          inline static constexpr auto impl(Is,
                                            index_sequence<Js...>,
                                            Ls... ls) {
            return make_farray(impl(variant_push_back_t<Is, Js>{}, ls...)...);
          }
#else
          template <typename...>
          struct impl;

          template <std::size_t... Is>
          struct impl<index_sequence<Is...>> {
            inline constexpr AUTO operator()() const
              AUTO_RETURN(&dispatch<Is...>)
          };

          template <typename Is, std::size_t... Js, typename... Ls>
          struct impl<Is, index_sequence<Js...>, Ls...> {
            inline constexpr AUTO operator()() const
              AUTO_RETURN(
                  make_farray(impl<variant_push_back_t<Is, Js>, Ls...>{}()...))
          };
#endif
        };

#ifdef ARA_CORE_RETURN_TYPE_DEDUCTION
        template <typename F, typename... Vs>
        inline static constexpr auto make_fmatrix() {
          return make_fmatrix_impl<F, Vs...>::impl(
              index_sequence<>{},
              make_index_sequence<variant_decay_t<Vs>::size()>{}...);
        }
#else
        template <typename F, typename... Vs>
        inline static constexpr AUTO make_fmatrix()
          AUTO_RETURN(
              typename make_fmatrix_impl<F, Vs...>::template impl<
                  index_sequence<>,
                  make_index_sequence<variant_decay_t<Vs>::size()>...>{}())
#endif

        template <typename F, typename... Vs>
        struct make_fdiagonal_impl {
          template <std::size_t I>
          inline static constexpr dispatch_result_t<F, Vs...> dispatch(
              F &&f, Vs &&... vs) {
            using Expected = dispatch_result_t<F, Vs...>;
            using Actual = decltype(
                variant_invoke(variant_forward<F>(f),
                            access::base::get_alt<I>(variant_forward<Vs>(vs))...));
            return visit_return_type_check<Expected, Actual>::invoke(
                variant_forward<F>(f),
                access::base::get_alt<I>(variant_forward<Vs>(vs))...);
          }

          template <std::size_t... Is>
          inline static constexpr AUTO impl(index_sequence<Is...>)
            AUTO_RETURN(make_farray(&dispatch<Is>...))
        };

        template <typename F, typename V, typename... Vs>
        inline static constexpr auto make_fdiagonal()
            -> decltype(make_fdiagonal_impl<F, V, Vs...>::impl(
                make_index_sequence<variant_decay_t<V>::size()>{})) {
          static_assert(variant_all<(variant_decay_t<V>::size() ==
                                  variant_decay_t<Vs>::size())...>::value,
                        "variant_all of the variants must be the same size.");
          return make_fdiagonal_impl<F, V, Vs...>::impl(
              make_index_sequence<variant_decay_t<V>::size()>{});
        }
#endif
      };

#if !defined(ARA_CORE_VARIANT_SWITCH_VISIT) && \
    (!defined(_MSC_VER) || _MSC_VER >= 1910)
      template <typename F, typename... Vs>
      using fmatrix_t = decltype(base::make_fmatrix<F, Vs...>());

      template <typename F, typename... Vs>
      struct fmatrix {
        static constexpr fmatrix_t<F, Vs...> value =
            base::make_fmatrix<F, Vs...>();
      };

      template <typename F, typename... Vs>
      constexpr fmatrix_t<F, Vs...> fmatrix<F, Vs...>::value;

      template <typename F, typename... Vs>
      using fdiagonal_t = decltype(base::make_fdiagonal<F, Vs...>());

      template <typename F, typename... Vs>
      struct fdiagonal {
        static constexpr fdiagonal_t<F, Vs...> value =
            base::make_fdiagonal<F, Vs...>();
      };

      template <typename F, typename... Vs>
      constexpr fdiagonal_t<F, Vs...> fdiagonal<F, Vs...>::value;
#endif

      struct alt {
        template <typename Visitor, typename... Vs>
        inline static constexpr DECLTYPE_AUTO visit_alt(Visitor &&visitor,
                                                        Vs &&... vs)
#ifdef ARA_CORE_VARIANT_SWITCH_VISIT
          DECLTYPE_AUTO_RETURN(
              base::dispatcher<
                  true,
                  base::dispatch_result_t<Visitor,
                                          decltype(as_base(
                                              variant_forward<Vs>(vs)))...>>::
                  template dispatch<0>(variant_forward<Visitor>(visitor),
                                       as_base(variant_forward<Vs>(vs))...))
#elif !defined(_MSC_VER) || _MSC_VER >= 1910
          DECLTYPE_AUTO_RETURN(base::at(
              fmatrix<Visitor &&,
                      decltype(as_base(variant_forward<Vs>(vs)))...>::value,
              vs.index()...)(variant_forward<Visitor>(visitor),
                             as_base(variant_forward<Vs>(vs))...))
#else
          DECLTYPE_AUTO_RETURN(base::at(
              base::make_fmatrix<Visitor &&,
                      decltype(as_base(variant_forward<Vs>(vs)))...>(),
              vs.index()...)(variant_forward<Visitor>(visitor),
                             as_base(variant_forward<Vs>(vs))...))
#endif

        template <typename Visitor, typename... Vs>
        inline static constexpr DECLTYPE_AUTO visit_alt_at(std::size_t index,
                                                           Visitor &&visitor,
                                                           Vs &&... vs)
#ifdef ARA_CORE_VARIANT_SWITCH_VISIT
          DECLTYPE_AUTO_RETURN(
              base::dispatcher<
                  true,
                  base::dispatch_result_t<Visitor,
                                          decltype(as_base(
                                              variant_forward<Vs>(vs)))...>>::
                  template dispatch_at<0>(index,
                                          variant_forward<Visitor>(visitor),
                                          as_base(variant_forward<Vs>(vs))...))
#elif !defined(_MSC_VER) || _MSC_VER >= 1910
          DECLTYPE_AUTO_RETURN(base::at(
              fdiagonal<Visitor &&,
                        decltype(as_base(variant_forward<Vs>(vs)))...>::value,
              index)(variant_forward<Visitor>(visitor),
                     as_base(variant_forward<Vs>(vs))...))
#else
          DECLTYPE_AUTO_RETURN(base::at(
              base::make_fdiagonal<Visitor &&,
                        decltype(as_base(variant_forward<Vs>(vs)))...>(),
              index)(variant_forward<Visitor>(visitor),
                     as_base(variant_forward<Vs>(vs))...))
#endif
      };

      struct Variant {
        private:
        template <typename Visitor>
        struct visitor {
          template <typename... Values>
          inline static constexpr bool does_not_handle() {
            return variant_is_invocable<Visitor, Values...>::value;
          }
        };

        template <typename Visitor, typename... Values>
        struct visit_exhaustiveness_check {
          static_assert(visitor<Visitor>::template does_not_handle<Values...>(),
                        "`visit` requires the visitor to be exhaustive.");

          inline static constexpr DECLTYPE_AUTO invoke(Visitor &&visitor,
                                                       Values &&... values)
            DECLTYPE_AUTO_RETURN(variant_invoke(variant_forward<Visitor>(visitor),
                                             variant_forward<Values>(values)...))
        };

        template <typename Visitor>
        struct value_visitor {
          Visitor &&visitor_;

          template <typename... Alts>
          inline constexpr DECLTYPE_AUTO operator()(Alts &&... alts) const
            DECLTYPE_AUTO_RETURN(
                visit_exhaustiveness_check<
                    Visitor,
                    decltype((variant_forward<Alts>(alts).value))...>::
                    invoke(variant_forward<Visitor>(visitor_),
                           variant_forward<Alts>(alts).value...))
        };

        template <typename Visitor>
        inline static constexpr AUTO make_value_visitor(Visitor &&visitor)
          AUTO_RETURN(value_visitor<Visitor>{variant_forward<Visitor>(visitor)})

        public:
        template <typename Visitor, typename... Vs>
        inline static constexpr DECLTYPE_AUTO visit_alt(Visitor &&visitor,
                                                        Vs &&... vs)
          DECLTYPE_AUTO_RETURN(alt::visit_alt(variant_forward<Visitor>(visitor),
                                              variant_forward<Vs>(vs).impl_...))

        template <typename Visitor, typename... Vs>
        inline static constexpr DECLTYPE_AUTO visit_alt_at(std::size_t index,
                                                           Visitor &&visitor,
                                                           Vs &&... vs)
          DECLTYPE_AUTO_RETURN(
              alt::visit_alt_at(index,
                                variant_forward<Visitor>(visitor),
                                variant_forward<Vs>(vs).impl_...))

        template <typename Visitor, typename... Vs>
        inline static constexpr DECLTYPE_AUTO visit_value(Visitor &&visitor,
                                                          Vs &&... vs)
          DECLTYPE_AUTO_RETURN(
              visit_alt(make_value_visitor(variant_forward<Visitor>(visitor)),
                        variant_forward<Vs>(vs)...))

        template <typename Visitor, typename... Vs>
        inline static constexpr DECLTYPE_AUTO visit_value_at(std::size_t index,
                                                             Visitor &&visitor,
                                                             Vs &&... vs)
          DECLTYPE_AUTO_RETURN(
              visit_alt_at(index,
                           make_value_visitor(variant_forward<Visitor>(visitor)),
                           variant_forward<Vs>(vs)...))
      };

    }  // namespace visitation

    template <std::size_t Index, typename T>
    struct alt {
      using value_type = T;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif
      template <typename... Args>
      inline explicit constexpr alt(in_place_t, Args &&... args)
          : value(variant_forward<Args>(args)...) {}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

      T value;
    };

    template <Trait DestructibleTrait, std::size_t Index, typename... Ts>
    union recursive_union;

    template <Trait DestructibleTrait, std::size_t Index>
    union recursive_union<DestructibleTrait, Index> {};

#define ARA_CORE_VARIANT_RECURSIVE_UNION(destructible_trait, destructor)      \
  template <std::size_t Index, typename T, typename... Ts>                 \
  union recursive_union<destructible_trait, Index, T, Ts...> {             \
    public:                                                                \
    inline explicit constexpr recursive_union(valueless_t) noexcept        \
        : dummy_{} {}                                                      \
                                                                           \
    template <typename... Args>                                            \
    inline explicit constexpr recursive_union(in_place_index_t<0>,         \
                                              Args &&... args)             \
        : head_(in_place_t{}, variant_forward<Args>(args)...) {}              \
                                                                           \
    template <std::size_t I, typename... Args>                             \
    inline explicit constexpr recursive_union(in_place_index_t<I>,         \
                                              Args &&... args)             \
        : tail_(in_place_index_t<I - 1>{}, variant_forward<Args>(args)...) {} \
                                                                           \
    recursive_union(const recursive_union &) = default;                    \
    recursive_union(recursive_union &&) = default;                         \
                                                                           \
    destructor                                                             \
                                                                           \
    recursive_union &operator=(const recursive_union &) = default;         \
    recursive_union &operator=(recursive_union &&) = default;              \
                                                                           \
    private:                                                               \
    char dummy_;                                                           \
    alt<Index, T> head_;                                                   \
    recursive_union<destructible_trait, Index + 1, Ts...> tail_;           \
                                                                           \
    friend struct access::recursive_union;                                 \
  }

    ARA_CORE_VARIANT_RECURSIVE_UNION(Trait::TriviallyAvailable,
                                  ~recursive_union() = default;);
    ARA_CORE_VARIANT_RECURSIVE_UNION(Trait::Available,
                                  ~recursive_union() {});
    ARA_CORE_VARIANT_RECURSIVE_UNION(Trait::Unavailable,
                                  ~recursive_union() = delete;);

#undef ARA_CORE_VARIANT_RECURSIVE_UNION

    template <typename... Ts>
    using index_t = typename std::conditional<
            sizeof...(Ts) < (std::numeric_limits<unsigned char>::max)(),
            unsigned char,
            typename std::conditional<
                sizeof...(Ts) < (std::numeric_limits<unsigned short>::max)(),
                unsigned short,
                unsigned int>::type
            >::type;

    template <Trait DestructibleTrait, typename... Ts>
    class base {
      public:
      inline explicit constexpr base(valueless_t tag) noexcept
          : data_(tag), index_(static_cast<index_t<Ts...>>(-1)) {}

      template <std::size_t I, typename... Args>
      inline explicit constexpr base(in_place_index_t<I>, Args &&... args)
          : data_(in_place_index_t<I>{}, variant_forward<Args>(args)...),
            index_(I) {}

      inline constexpr bool valueless_by_exception() const noexcept {
        return index_ == static_cast<index_t<Ts...>>(-1);
      }

      inline constexpr std::size_t index() const noexcept {
        return valueless_by_exception() ? variant_npos : index_;
      }

      protected:
      using data_t = recursive_union<DestructibleTrait, 0, Ts...>;

      friend inline constexpr base &as_base(base &b) { return b; }
      friend inline constexpr const base &as_base(const base &b) { return b; }
      friend inline constexpr base &&as_base(base &&b) { return variant_move(b); }
      friend inline constexpr const base &&as_base(const base &&b) { return variant_move(b); }

      friend inline constexpr data_t &data(base &b) { return b.data_; }
      friend inline constexpr const data_t &data(const base &b) { return b.data_; }
      friend inline constexpr data_t &&data(base &&b) { return variant_move(b).data_; }
      friend inline constexpr const data_t &&data(const base &&b) { return variant_move(b).data_; }

      inline static constexpr std::size_t size() { return sizeof...(Ts); }

      data_t data_;
      index_t<Ts...> index_;

      friend struct access::base;
      friend struct visitation::base;
    };

    struct dtor {
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)
#endif
      template <typename Alt>
      inline void operator()(Alt &alt) const noexcept { alt.~Alt(); }
#ifdef _MSC_VER
#pragma warning(pop)
#endif
    };

#if !defined(_MSC_VER) || _MSC_VER >= 1910
#define ARA_CORE_INHERITING_CTOR(type, base) using base::base;
#else
#define ARA_CORE_INHERITING_CTOR(type, base)         \
  template <typename... Args>                     \
  inline explicit constexpr type(Args &&... args) \
      : base(variant_forward<Args>(args)...) {}
#endif

    template <typename Traits, Trait = Traits::destructible_trait>
    class destructor;

#define ARA_CORE_VARIANT_DESTRUCTOR(destructible_trait, definition, destroy) \
  template <typename... Ts>                                               \
  class destructor<traits<Ts...>, destructible_trait>                     \
      : public base<destructible_trait, Ts...> {                          \
    using super = base<destructible_trait, Ts...>;                        \
                                                                          \
    public:                                                               \
    ARA_CORE_INHERITING_CTOR(destructor, super)                              \
    using super::operator=;                                               \
                                                                          \
    destructor(const destructor &) = default;                             \
    destructor(destructor &&) = default;                                  \
    definition                                                            \
    destructor &operator=(const destructor &) = default;                  \
    destructor &operator=(destructor &&) = default;                       \
                                                                          \
    protected:                                                            \
    destroy                                                               \
  }

    ARA_CORE_VARIANT_DESTRUCTOR(
        Trait::TriviallyAvailable,
        ~destructor() = default;,
        inline void destroy() noexcept {
          this->index_ = static_cast<index_t<Ts...>>(-1);
        });

    ARA_CORE_VARIANT_DESTRUCTOR(
        Trait::Available,
        ~destructor() { destroy(); },
        inline void destroy() noexcept {
          if (!this->valueless_by_exception()) {
            visitation::alt::visit_alt(dtor{}, *this);
          }
          this->index_ = static_cast<index_t<Ts...>>(-1);
        });

    ARA_CORE_VARIANT_DESTRUCTOR(
        Trait::Unavailable,
        ~destructor() = delete;,
        inline void destroy() noexcept = delete;);

#undef ARA_CORE_VARIANT_DESTRUCTOR

    template <typename Traits>
    class constructor : public destructor<Traits> {
      using super = destructor<Traits>;

      public:
      ARA_CORE_INHERITING_CTOR(constructor, super)
      using super::operator=;

      protected:
#ifndef ARA_CORE_GENERIC_LAMBDAS
      struct ctor {
        template <typename LhsAlt, typename RhsAlt>
        inline void operator()(LhsAlt &lhs_alt, RhsAlt &&rhs_alt) const {
          constructor::construct_alt(lhs_alt,
                                     variant_forward<RhsAlt>(rhs_alt).value);
        }
      };
#endif

      template <std::size_t I, typename T, typename... Args>
      inline static T &construct_alt(alt<I, T> &a, Args &&... args) {
        auto *result = ::new (static_cast<void *>(variant_addressof(a)))
            alt<I, T>(in_place_t{}, variant_forward<Args>(args)...);
        return result->value;
      }

      template <typename Rhs>
      inline static void generic_construct(constructor &lhs, Rhs &&rhs) {
        lhs.destroy();
        if (!rhs.valueless_by_exception()) {
          visitation::alt::visit_alt_at(
              rhs.index(),
#ifdef ARA_CORE_GENERIC_LAMBDAS
              [](auto &lhs_alt, auto &&rhs_alt) {
                constructor::construct_alt(
                    lhs_alt, variant_forward<decltype(rhs_alt)>(rhs_alt).value);
              }
#else
              ctor{}
#endif
              ,
              lhs,
              variant_forward<Rhs>(rhs));
          lhs.index_ = rhs.index_;
        }
      }
    };

    template <typename Traits, Trait = Traits::move_constructible_trait>
    class move_constructor;

#define ARA_CORE_VARIANT_MOVE_CONSTRUCTOR(move_constructible_trait, definition) \
  template <typename... Ts>                                                  \
  class move_constructor<traits<Ts...>, move_constructible_trait>            \
      : public constructor<traits<Ts...>> {                                  \
    using super = constructor<traits<Ts...>>;                                \
                                                                             \
    public:                                                                  \
    ARA_CORE_INHERITING_CTOR(move_constructor, super)                           \
    using super::operator=;                                                  \
                                                                             \
    move_constructor(const move_constructor &) = default;                    \
    definition                                                               \
    ~move_constructor() = default;                                           \
    move_constructor &operator=(const move_constructor &) = default;         \
    move_constructor &operator=(move_constructor &&) = default;              \
  }

    ARA_CORE_VARIANT_MOVE_CONSTRUCTOR(
        Trait::TriviallyAvailable,
        move_constructor(move_constructor &&that) = default;);

    ARA_CORE_VARIANT_MOVE_CONSTRUCTOR(
        Trait::Available,
        move_constructor(move_constructor &&that) noexcept(
            variant_all<std::is_nothrow_move_constructible<Ts>::value...>::value)
            : move_constructor(valueless_t{}) {
          this->generic_construct(*this, variant_move(that));
        });

    ARA_CORE_VARIANT_MOVE_CONSTRUCTOR(
        Trait::Unavailable,
        move_constructor(move_constructor &&) = delete;);

#undef ARA_CORE_VARIANT_MOVE_CONSTRUCTOR

    template <typename Traits, Trait = Traits::copy_constructible_trait>
    class copy_constructor;

#define ARA_CORE_VARIANT_COPY_CONSTRUCTOR(copy_constructible_trait, definition) \
  template <typename... Ts>                                                  \
  class copy_constructor<traits<Ts...>, copy_constructible_trait>            \
      : public move_constructor<traits<Ts...>> {                             \
    using super = move_constructor<traits<Ts...>>;                           \
                                                                             \
    public:                                                                  \
    ARA_CORE_INHERITING_CTOR(copy_constructor, super)                           \
    using super::operator=;                                                  \
                                                                             \
    definition                                                               \
    copy_constructor(copy_constructor &&) = default;                         \
    ~copy_constructor() = default;                                           \
    copy_constructor &operator=(const copy_constructor &) = default;         \
    copy_constructor &operator=(copy_constructor &&) = default;              \
  }

    ARA_CORE_VARIANT_COPY_CONSTRUCTOR(
        Trait::TriviallyAvailable,
        copy_constructor(const copy_constructor &that) = default;);

    ARA_CORE_VARIANT_COPY_CONSTRUCTOR(
        Trait::Available,
        copy_constructor(const copy_constructor &that)
            : copy_constructor(valueless_t{}) {
          this->generic_construct(*this, that);
        });

    ARA_CORE_VARIANT_COPY_CONSTRUCTOR(
        Trait::Unavailable,
        copy_constructor(const copy_constructor &) = delete;);

#undef ARA_CORE_VARIANT_COPY_CONSTRUCTOR

    template <typename Traits>
    class assignment : public copy_constructor<Traits> {
      using super = copy_constructor<Traits>;

      public:
      ARA_CORE_INHERITING_CTOR(assignment, super)
      using super::operator=;

      template <std::size_t I, typename... Args>
      inline /* auto & */ auto emplace(Args &&... args)
          -> decltype(this->construct_alt(access::base::get_alt<I>(*this),
                                          variant_forward<Args>(args)...)) {
        this->destroy();
        auto &result = this->construct_alt(access::base::get_alt<I>(*this),
                                           variant_forward<Args>(args)...);
        this->index_ = I;
        return result;
      }

      protected:
#ifndef ARA_CORE_GENERIC_LAMBDAS
      template <typename That>
      struct assigner {
        template <typename ThisAlt, typename ThatAlt>
        inline void operator()(ThisAlt &this_alt, ThatAlt &&that_alt) const {
          self->assign_alt(this_alt, variant_forward<ThatAlt>(that_alt).value);
        }
        assignment *self;
      };
#endif

      template <std::size_t I, typename T, typename Arg>
      inline void assign_alt(alt<I, T> &a, Arg &&arg) {
        if (this->index() == I) {
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif
          a.value = variant_forward<Arg>(arg);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
        } else {
          struct {
            void operator()(std::true_type) const {
              this_->emplace<I>(variant_forward<Arg>(arg_));
            }
            void operator()(std::false_type) const {
              this_->emplace<I>(T(variant_forward<Arg>(arg_)));
            }
            assignment *this_;
            Arg &&arg_;
          } impl{this, variant_forward<Arg>(arg)};
          impl(variant_bool_constant<
                   std::is_nothrow_constructible<T, Arg>::value ||
                   !std::is_nothrow_move_constructible<T>::value>{});
        }
      }

      template <typename That>
      inline void generic_assign(That &&that) {
        if (this->valueless_by_exception() && that.valueless_by_exception()) {
          // do nothing.
        } else if (that.valueless_by_exception()) {
          this->destroy();
        } else {
          visitation::alt::visit_alt_at(
              that.index(),
#ifdef ARA_CORE_GENERIC_LAMBDAS
              [this](auto &this_alt, auto &&that_alt) {
                this->assign_alt(
                    this_alt, variant_forward<decltype(that_alt)>(that_alt).value);
              }
#else
              assigner<That>{this}
#endif
              ,
              *this,
              variant_forward<That>(that));
        }
      }
    };

    template <typename Traits, Trait = Traits::move_assignable_trait>
    class move_assignment;

#define ARA_CORE_VARIANT_MOVE_ASSIGNMENT(move_assignable_trait, definition) \
  template <typename... Ts>                                              \
  class move_assignment<traits<Ts...>, move_assignable_trait>            \
      : public assignment<traits<Ts...>> {                               \
    using super = assignment<traits<Ts...>>;                             \
                                                                         \
    public:                                                              \
    ARA_CORE_INHERITING_CTOR(move_assignment, super)                        \
    using super::operator=;                                              \
                                                                         \
    move_assignment(const move_assignment &) = default;                  \
    move_assignment(move_assignment &&) = default;                       \
    ~move_assignment() = default;                                        \
    move_assignment &operator=(const move_assignment &) = default;       \
    definition                                                           \
  }

    ARA_CORE_VARIANT_MOVE_ASSIGNMENT(
        Trait::TriviallyAvailable,
        move_assignment &operator=(move_assignment &&that) = default;);

    ARA_CORE_VARIANT_MOVE_ASSIGNMENT(
        Trait::Available,
        move_assignment &
        operator=(move_assignment &&that) noexcept(
            variant_all<(std::is_nothrow_move_constructible<Ts>::value &&
                      std::is_nothrow_move_assignable<Ts>::value)...>::value) {
          this->generic_assign(variant_move(that));
          return *this;
        });

    ARA_CORE_VARIANT_MOVE_ASSIGNMENT(
        Trait::Unavailable,
        move_assignment &operator=(move_assignment &&) = delete;);

#undef ARA_CORE_VARIANT_MOVE_ASSIGNMENT

    template <typename Traits, Trait = Traits::copy_assignable_trait>
    class copy_assignment;

#define ARA_CORE_VARIANT_COPY_ASSIGNMENT(copy_assignable_trait, definition) \
  template <typename... Ts>                                              \
  class copy_assignment<traits<Ts...>, copy_assignable_trait>            \
      : public move_assignment<traits<Ts...>> {                          \
    using super = move_assignment<traits<Ts...>>;                        \
                                                                         \
    public:                                                              \
    ARA_CORE_INHERITING_CTOR(copy_assignment, super)                        \
    using super::operator=;                                              \
                                                                         \
    copy_assignment(const copy_assignment &) = default;                  \
    copy_assignment(copy_assignment &&) = default;                       \
    ~copy_assignment() = default;                                        \
    definition                                                           \
    copy_assignment &operator=(copy_assignment &&) = default;            \
  }

    ARA_CORE_VARIANT_COPY_ASSIGNMENT(
        Trait::TriviallyAvailable,
        copy_assignment &operator=(const copy_assignment &that) = default;);

    ARA_CORE_VARIANT_COPY_ASSIGNMENT(
        Trait::Available,
        copy_assignment &operator=(const copy_assignment &that) {
          this->generic_assign(that);
          return *this;
        });

    ARA_CORE_VARIANT_COPY_ASSIGNMENT(
        Trait::Unavailable,
        copy_assignment &operator=(const copy_assignment &) = delete;);

#undef ARA_CORE_VARIANT_COPY_ASSIGNMENT

    template <typename... Ts>
    class impl : public copy_assignment<traits<Ts...>> {
      using super = copy_assignment<traits<Ts...>>;

      public:
      ARA_CORE_INHERITING_CTOR(impl, super)
      using super::operator=;

      impl(const impl&) = default;
      impl(impl&&) = default;
      ~impl() = default;
      impl &operator=(const impl &) = default;
      impl &operator=(impl &&) = default;

      template <std::size_t I, typename Arg>
      inline void assign(Arg &&arg) {
        this->assign_alt(access::base::get_alt<I>(*this),
                         variant_forward<Arg>(arg));
      }

      inline void swap(impl &that) {
        if (this->valueless_by_exception() && that.valueless_by_exception()) {
          // do nothing.
        } else if (this->index() == that.index()) {
          visitation::alt::visit_alt_at(this->index(),
#ifdef ARA_CORE_GENERIC_LAMBDAS
                                        [](auto &this_alt, auto &that_alt) {
                                          using std::swap;
                                          swap(this_alt.value,
                                               that_alt.value);
                                        }
#else
                                        swapper{}
#endif
                                        ,
                                        *this,
                                        that);
        } else {
          impl *lhs = this;
          impl *rhs = variant_addressof(that);
          if (lhs->move_nothrow() && !rhs->move_nothrow()) {
            std::swap(lhs, rhs);
          }
          impl tmp(variant_move(*rhs));
          // EXTENSION: When the move construction of `lhs` into `rhs` throws
          // and `tmp` is nothrow move constructible then we variant_move `tmp` back
          // into `rhs` and provide the strong exception safety guarantee.
          try {
            this->generic_construct(*rhs, variant_move(*lhs));
          } catch (...) {
            if (tmp.move_nothrow()) {
              this->generic_construct(*rhs, variant_move(tmp));
            }
            throw;
          }
          this->generic_construct(*lhs, variant_move(tmp));
        }
      }

      private:
#ifndef ARA_CORE_GENERIC_LAMBDAS
      struct swapper {
        template <typename ThisAlt, typename ThatAlt>
        inline void operator()(ThisAlt &this_alt, ThatAlt &that_alt) const {
          using std::swap;
          swap(this_alt.value, that_alt.value);
        }
      };
#endif

      inline constexpr bool move_nothrow() const {
        return this->valueless_by_exception() ||
               variant_array<bool, sizeof...(Ts)>{
                   {std::is_nothrow_move_constructible<Ts>::value...}
               }[this->index()];
      }
    };

#undef ARA_CORE_INHERITING_CTOR

    template <typename From, typename To>
    struct is_non_narrowing_convertible {
      template <typename T>
      static std::true_type test(T(&&)[1]);

      template <typename T>
      static auto impl(int) -> decltype(test<T>({std::declval<From>()}));

      template <typename>
      static auto impl(...) -> std::false_type;

      static constexpr bool value = decltype(impl<To>(0))::value;
    };

    template <typename Arg,
              std::size_t I,
              typename T,
              bool = std::is_arithmetic<T>::value,
              typename = void>
    struct overload_leaf {};

    template <typename Arg, std::size_t I, typename T>
    struct overload_leaf<Arg, I, T, false> {
      using impl = variant_size_constant<I> (*)(T);
      operator impl() const { return nullptr; };
    };

    template <typename Arg, std::size_t I, typename T>
    struct overload_leaf<
        Arg,
        I,
        T,
        true
#if defined(__clang__) || !defined(__GNUC__) || __GNUC__ >= 5
        ,
        variant_enable_if_t<
            std::is_same<variant_remove_cvref_t<T>, bool>::value
                ? std::is_same<variant_remove_cvref_t<Arg>, bool>::value
                : is_non_narrowing_convertible<Arg, T>::value>
#endif
        > {
      using impl = variant_size_constant<I> (*)(T);
      operator impl() const { return nullptr; };
    };

    template <typename Arg, typename... Ts>
    struct overload_impl {
      private:
      template <typename>
      struct impl;

      template <std::size_t... Is>
      struct impl<index_sequence<Is...>> : overload_leaf<Arg, Is, Ts>... {};

      public:
      using type = impl<index_sequence_for<Ts...>>;
    };

    template <typename Arg, typename... Ts>
    using overload = typename overload_impl<Arg, Ts...>::type;

    template <typename Arg, typename... Ts>
    using best_match = variant_invoke_result_t<overload<Arg, Ts...>, Arg>;

    template <typename T>
    struct is_in_place_index : std::false_type {};

    template <std::size_t I>
    struct is_in_place_index<in_place_index_t<I>> : std::true_type {};

    template <typename T>
    struct is_in_place_type : std::false_type {};

    template <typename T>
    struct is_in_place_type<in_place_type_t<T>> : std::true_type {};

  }  // variant_detail

  template <typename... Ts>
  class Variant {
    static_assert(0 < sizeof...(Ts),
                  "Variant must consist of at least one alternative.");

    static_assert(variant_all<!std::is_array<Ts>::value...>::value,
                  "Variant can not have an variant_array type as an alternative.");

    static_assert(variant_all<!std::is_reference<Ts>::value...>::value,
                  "Variant can not have a reference type as an alternative.");

    static_assert(variant_all<!std::is_void<Ts>::value...>::value,
                  "Variant can not have a void type as an alternative.");

    public:
    template <
        typename Front = variant_type_pack_element_t<0, Ts...>,
        variant_enable_if_t<std::is_default_constructible<Front>::value, int> = 0>
    inline constexpr Variant() noexcept(
        std::is_nothrow_default_constructible<Front>::value)
        : impl_(in_place_index_t<0>{}) {}

    Variant(const Variant &) = default;
    Variant(Variant &&) = default;

    template <
        typename Arg,
        typename Decayed = variant_decay_t<Arg>,
        variant_enable_if_t<!std::is_same<Decayed, Variant>::value, int> = 0,
        variant_enable_if_t<!variant_detail::is_in_place_index<Decayed>::value, int> = 0,
        variant_enable_if_t<!variant_detail::is_in_place_type<Decayed>::value, int> = 0,
        std::size_t I = variant_detail::best_match<Arg, Ts...>::value,
        typename T = variant_type_pack_element_t<I, Ts...>,
        variant_enable_if_t<std::is_constructible<T, Arg>::value, int> = 0>
    inline constexpr Variant(Arg &&arg) noexcept(
        std::is_nothrow_constructible<T, Arg>::value)
        : impl_(in_place_index_t<I>{}, variant_forward<Arg>(arg)) {}

    template <
        std::size_t I,
        typename... Args,
        typename T = variant_type_pack_element_t<I, Ts...>,
        variant_enable_if_t<std::is_constructible<T, Args...>::value, int> = 0>
    inline explicit constexpr Variant(
        in_place_index_t<I>,
        Args &&... args) noexcept(std::is_nothrow_constructible<T,
                                                                Args...>::value)
        : impl_(in_place_index_t<I>{}, variant_forward<Args>(args)...) {}

    template <
        std::size_t I,
        typename Up,
        typename... Args,
        typename T = variant_type_pack_element_t<I, Ts...>,
        variant_enable_if_t<std::is_constructible<T,
                                               std::initializer_list<Up> &,
                                               Args...>::value,
                         int> = 0>
    inline explicit constexpr Variant(
        in_place_index_t<I>,
        std::initializer_list<Up> il,
        Args &&... args) noexcept(std::
                                      is_nothrow_constructible<
                                          T,
                                          std::initializer_list<Up> &,
                                          Args...>::value)
        : impl_(in_place_index_t<I>{}, il, variant_forward<Args>(args)...) {}

    template <
        typename T,
        typename... Args,
        std::size_t I = variant_detail::find_index_sfinae<T, Ts...>::value,
        variant_enable_if_t<std::is_constructible<T, Args...>::value, int> = 0>
    inline explicit constexpr Variant(
        in_place_type_t<T>,
        Args &&... args) noexcept(std::is_nothrow_constructible<T,
                                                                Args...>::value)
        : impl_(in_place_index_t<I>{}, variant_forward<Args>(args)...) {}

    template <
        typename T,
        typename Up,
        typename... Args,
        std::size_t I = variant_detail::find_index_sfinae<T, Ts...>::value,
        variant_enable_if_t<std::is_constructible<T,
                                               std::initializer_list<Up> &,
                                               Args...>::value,
                         int> = 0>
    inline explicit constexpr Variant(
        in_place_type_t<T>,
        std::initializer_list<Up> il,
        Args &&... args) noexcept(std::
                                      is_nothrow_constructible<
                                          T,
                                          std::initializer_list<Up> &,
                                          Args...>::value)
        : impl_(in_place_index_t<I>{}, il, variant_forward<Args>(args)...) {}

    ~Variant() = default;

    Variant &operator=(const Variant &) = default;
    Variant &operator=(Variant &&) = default;

    template <typename Arg,
              variant_enable_if_t<!std::is_same<variant_decay_t<Arg>, Variant>::value,
                               int> = 0,
              std::size_t I = variant_detail::best_match<Arg, Ts...>::value,
              typename T = variant_type_pack_element_t<I, Ts...>,
              variant_enable_if_t<(std::is_assignable<T &, Arg>::value &&
                                std::is_constructible<T, Arg>::value),
                               int> = 0>
    inline Variant &operator=(Arg &&arg) noexcept(
        (std::is_nothrow_assignable<T &, Arg>::value &&
         std::is_nothrow_constructible<T, Arg>::value)) {
      impl_.template assign<I>(variant_forward<Arg>(arg));
      return *this;
    }

    template <
        std::size_t I,
        typename... Args,
        typename T = variant_type_pack_element_t<I, Ts...>,
        variant_enable_if_t<std::is_constructible<T, Args...>::value, int> = 0>
    inline T &emplace(Args &&... args) {
      return impl_.template emplace<I>(variant_forward<Args>(args)...);
    }

    template <
        std::size_t I,
        typename Up,
        typename... Args,
        typename T = variant_type_pack_element_t<I, Ts...>,
        variant_enable_if_t<std::is_constructible<T,
                                               std::initializer_list<Up> &,
                                               Args...>::value,
                         int> = 0>
    inline T &emplace(std::initializer_list<Up> il, Args &&... args) {
      return impl_.template emplace<I>(il, variant_forward<Args>(args)...);
    }

    template <
        typename T,
        typename... Args,
        std::size_t I = variant_detail::find_index_sfinae<T, Ts...>::value,
        variant_enable_if_t<std::is_constructible<T, Args...>::value, int> = 0>
    inline T &emplace(Args &&... args) {
      return impl_.template emplace<I>(variant_forward<Args>(args)...);
    }

    template <
        typename T,
        typename Up,
        typename... Args,
        std::size_t I = variant_detail::find_index_sfinae<T, Ts...>::value,
        variant_enable_if_t<std::is_constructible<T,
                                               std::initializer_list<Up> &,
                                               Args...>::value,
                         int> = 0>
    inline T &emplace(std::initializer_list<Up> il, Args &&... args) {
      return impl_.template emplace<I>(il, variant_forward<Args>(args)...);
    }

    inline constexpr bool valueless_by_exception() const noexcept {
      return impl_.valueless_by_exception();
    }

    inline constexpr std::size_t index() const noexcept {
      return impl_.index();
    }

    template <bool Dummy = true,
              variant_enable_if_t<
                  variant_all<Dummy,
                           (variant_dependent_type<std::is_move_constructible<Ts>,
                                                Dummy>::value &&
                            variant_dependent_type<is_swappable<Ts>,
                                                Dummy>::value)...>::value,
                  int> = 0>
    inline void swap(Variant &that) noexcept(
        variant_all<(std::is_nothrow_move_constructible<Ts>::value &&
                  is_nothrow_swappable<Ts>::value)...>::value) {
      impl_.swap(that.impl_);
    }

    private:
    variant_detail::impl<Ts...> impl_;

    friend struct variant_detail::access::Variant;
    friend struct variant_detail::visitation::Variant;
  };

  template <std::size_t I, typename... Ts>
  inline constexpr bool holds_alternative(const Variant<Ts...> &v) noexcept {
    return v.index() == I;
  }

  template <typename T, typename... Ts>
  inline constexpr bool holds_alternative(const Variant<Ts...> &v) noexcept {
    return holds_alternative<variant_detail::find_index_checked<T, Ts...>::value>(v);
  }

  namespace variant_detail {
    template <std::size_t I, typename V>
    struct generic_get_impl {
      constexpr generic_get_impl(int) noexcept {}

      constexpr AUTO_REFREF operator()(V &&v) const
        AUTO_REFREF_RETURN(
            access::Variant::get_alt<I>(variant_forward<V>(v)).value)
    };

    template <std::size_t I, typename V>
    inline constexpr AUTO_REFREF generic_get(V &&v)
      AUTO_REFREF_RETURN(generic_get_impl<I, V>(
          holds_alternative<I>(v) ? 0 : (throw_bad_variant_access(), 0))(
          variant_forward<V>(v)))
  }  // namespace variant_detail

  template <std::size_t I, typename... Ts>
  inline constexpr variant_alternative_t<I, Variant<Ts...>> &get(
      Variant<Ts...> &v) {
    return variant_detail::generic_get<I>(v);
  }

  template <std::size_t I, typename... Ts>
  inline constexpr variant_alternative_t<I, Variant<Ts...>> &&get(
      Variant<Ts...> &&v) {
    return variant_detail::generic_get<I>(variant_move(v));
  }

  template <std::size_t I, typename... Ts>
  inline constexpr const variant_alternative_t<I, Variant<Ts...>> &get(
      const Variant<Ts...> &v) {
    return variant_detail::generic_get<I>(v);
  }

  template <std::size_t I, typename... Ts>
  inline constexpr const variant_alternative_t<I, Variant<Ts...>> &&get(
      const Variant<Ts...> &&v) {
    return variant_detail::generic_get<I>(variant_move(v));
  }

  template <typename T, typename... Ts>
  inline constexpr T &get(Variant<Ts...> &v) {
    return get<variant_detail::find_index_checked<T, Ts...>::value>(v);
  }

  template <typename T, typename... Ts>
  inline constexpr T &&get(Variant<Ts...> &&v) {
    return get<variant_detail::find_index_checked<T, Ts...>::value>(variant_move(v));
  }

  template <typename T, typename... Ts>
  inline constexpr const T &get(const Variant<Ts...> &v) {
    return get<variant_detail::find_index_checked<T, Ts...>::value>(v);
  }

  template <typename T, typename... Ts>
  inline constexpr const T &&get(const Variant<Ts...> &&v) {
    return get<variant_detail::find_index_checked<T, Ts...>::value>(variant_move(v));
  }

  namespace variant_detail {

    template <std::size_t I, typename V>
    inline constexpr /* auto * */ AUTO generic_get_if(V *v) noexcept
      AUTO_RETURN(v && holds_alternative<I>(*v)
                      ? variant_addressof(access::Variant::get_alt<I>(*v).value)
                      : nullptr)

  }  // namespace variant_detail

  template <std::size_t I, typename... Ts>
  inline constexpr variant_add_pointer_t<variant_alternative_t<I, Variant<Ts...>>>
  get_if(Variant<Ts...> *v) noexcept {
    return variant_detail::generic_get_if<I>(v);
  }

  template <std::size_t I, typename... Ts>
  inline constexpr variant_add_pointer_t<
      const variant_alternative_t<I, Variant<Ts...>>>
  get_if(const Variant<Ts...> *v) noexcept {
    return variant_detail::generic_get_if<I>(v);
  }

  template <typename T, typename... Ts>
  inline constexpr variant_add_pointer_t<T>
  get_if(Variant<Ts...> *v) noexcept {
    return get_if<variant_detail::find_index_checked<T, Ts...>::value>(v);
  }

  template <typename T, typename... Ts>
  inline constexpr variant_add_pointer_t<const T>
  get_if(const Variant<Ts...> *v) noexcept {
    return get_if<variant_detail::find_index_checked<T, Ts...>::value>(v);
  }

  namespace variant_detail {
    template <typename RelOp>
    struct convert_to_bool {
      template <typename Lhs, typename Rhs>
      inline constexpr bool operator()(Lhs &&lhs, Rhs &&rhs) const {
        static_assert(std::is_convertible<variant_invoke_result_t<RelOp, Lhs, Rhs>,
                                          bool>::value,
                      "relational operators must return a type"
                      " implicitly convertible to bool");
        return variant_invoke(
            RelOp{}, variant_forward<Lhs>(lhs), variant_forward<Rhs>(rhs));
      }
    };
  }  // namespace variant_detail

  template <typename... Ts>
  inline constexpr bool operator==(const Variant<Ts...> &lhs,
                                   const Variant<Ts...> &rhs) {
    using variant_detail::visitation::Variant;
    using variant_equal_to = variant_detail::convert_to_bool<variant_equal_to>;
#ifdef ARA_CORE_CPP14_CONSTEXPR
    if (lhs.index() != rhs.index()) return false;
    if (lhs.valueless_by_exception()) return true;
    return Variant::visit_value_at(lhs.index(), variant_equal_to{}, lhs, rhs);
#else
    return lhs.index() == rhs.index() &&
           (lhs.valueless_by_exception() ||
            Variant::visit_value_at(lhs.index(), variant_equal_to{}, lhs, rhs));
#endif
  }

  template <typename... Ts>
  inline constexpr bool operator!=(const Variant<Ts...> &lhs,
                                   const Variant<Ts...> &rhs) {
    using variant_detail::visitation::Variant;
    using variant_not_equal_to = variant_detail::convert_to_bool<variant_not_equal_to>;
#ifdef ARA_CORE_CPP14_CONSTEXPR
    if (lhs.index() != rhs.index()) return true;
    if (lhs.valueless_by_exception()) return false;
    return Variant::visit_value_at(lhs.index(), variant_not_equal_to{}, lhs, rhs);
#else
    return lhs.index() != rhs.index() ||
           (!lhs.valueless_by_exception() &&
            Variant::visit_value_at(lhs.index(), variant_not_equal_to{}, lhs, rhs));
#endif
  }

  template <typename... Ts>
  inline constexpr bool operator<(const Variant<Ts...> &lhs,
                                  const Variant<Ts...> &rhs) {
    using variant_detail::visitation::Variant;
    using variant_less = variant_detail::convert_to_bool<variant_less>;
#ifdef ARA_CORE_CPP14_CONSTEXPR
    if (rhs.valueless_by_exception()) return false;
    if (lhs.valueless_by_exception()) return true;
    if (lhs.index() < rhs.index()) return true;
    if (lhs.index() > rhs.index()) return false;
    return Variant::visit_value_at(lhs.index(), variant_less{}, lhs, rhs);
#else
    return !rhs.valueless_by_exception() &&
           (lhs.valueless_by_exception() || lhs.index() < rhs.index() ||
            (lhs.index() == rhs.index() &&
             Variant::visit_value_at(lhs.index(), variant_less{}, lhs, rhs)));
#endif
  }

  template <typename... Ts>
  inline constexpr bool operator>(const Variant<Ts...> &lhs,
                                  const Variant<Ts...> &rhs) {
    using variant_detail::visitation::Variant;
    using variant_greater = variant_detail::convert_to_bool<variant_greater>;
#ifdef ARA_CORE_CPP14_CONSTEXPR
    if (lhs.valueless_by_exception()) return false;
    if (rhs.valueless_by_exception()) return true;
    if (lhs.index() > rhs.index()) return true;
    if (lhs.index() < rhs.index()) return false;
    return Variant::visit_value_at(lhs.index(), variant_greater{}, lhs, rhs);
#else
    return !lhs.valueless_by_exception() &&
           (rhs.valueless_by_exception() || lhs.index() > rhs.index() ||
            (lhs.index() == rhs.index() &&
             Variant::visit_value_at(lhs.index(), variant_greater{}, lhs, rhs)));
#endif
  }

  template <typename... Ts>
  inline constexpr bool operator<=(const Variant<Ts...> &lhs,
                                   const Variant<Ts...> &rhs) {
    using variant_detail::visitation::Variant;
    using variant_less_equal = variant_detail::convert_to_bool<variant_less_equal>;
#ifdef ARA_CORE_CPP14_CONSTEXPR
    if (lhs.valueless_by_exception()) return true;
    if (rhs.valueless_by_exception()) return false;
    if (lhs.index() < rhs.index()) return true;
    if (lhs.index() > rhs.index()) return false;
    return Variant::visit_value_at(lhs.index(), variant_less_equal{}, lhs, rhs);
#else
    return lhs.valueless_by_exception() ||
           (!rhs.valueless_by_exception() &&
            (lhs.index() < rhs.index() ||
             (lhs.index() == rhs.index() &&
              Variant::visit_value_at(lhs.index(), variant_less_equal{}, lhs, rhs))));
#endif
  }

  template <typename... Ts>
  inline constexpr bool operator>=(const Variant<Ts...> &lhs,
                                   const Variant<Ts...> &rhs) {
    using variant_detail::visitation::Variant;
    using variant_greater_equal = variant_detail::convert_to_bool<variant_greater_equal>;
#ifdef ARA_CORE_CPP14_CONSTEXPR
    if (rhs.valueless_by_exception()) return true;
    if (lhs.valueless_by_exception()) return false;
    if (lhs.index() > rhs.index()) return true;
    if (lhs.index() < rhs.index()) return false;
    return Variant::visit_value_at(lhs.index(), variant_greater_equal{}, lhs, rhs);
#else
    return rhs.valueless_by_exception() ||
           (!lhs.valueless_by_exception() &&
            (lhs.index() > rhs.index() ||
             (lhs.index() == rhs.index() &&
              Variant::visit_value_at(
                  lhs.index(), variant_greater_equal{}, lhs, rhs))));
#endif
  }

  struct Monostate {};

  inline constexpr bool operator<(Monostate, Monostate) noexcept {
    return false;
  }

  inline constexpr bool operator>(Monostate, Monostate) noexcept {
    return false;
  }

  inline constexpr bool operator<=(Monostate, Monostate) noexcept {
    return true;
  }

  inline constexpr bool operator>=(Monostate, Monostate) noexcept {
    return true;
  }

  inline constexpr bool operator==(Monostate, Monostate) noexcept {
    return true;
  }

  inline constexpr bool operator!=(Monostate, Monostate) noexcept {
    return false;
  }

#ifdef ARA_CORE_CPP14_CONSTEXPR
  namespace variant_detail {

    inline constexpr bool any(std::initializer_list<bool> bs) {
      for (bool b : bs) {
        if (b) {
          return true;
        }
      }
      return false;
    }

  }  // namespace variant_detail

  template <typename Visitor, typename... Vs>
  inline constexpr decltype(auto) visit(Visitor &&visitor, Vs &&... vs) {
    return (!variant_detail::any({vs.valueless_by_exception()...})
                ? (void)0
                : throw_bad_variant_access()),
           variant_detail::visitation::Variant::visit_value(
               variant_forward<Visitor>(visitor), variant_forward<Vs>(vs)...);
  }
#else
  namespace variant_detail {

    template <std::size_t N>
    inline constexpr bool all_impl(const variant_array<bool, N> &bs,
                                   std::size_t idx) {
      return idx >= N || (bs[idx] && all_impl(bs, idx + 1));
    }

    template <std::size_t N>
    inline constexpr bool variant_all(const variant_array<bool, N> &bs) {
      return all_impl(bs, 0);
    }

  }  // namespace variant_detail

  template <typename Visitor, typename... Vs>
  inline constexpr DECLTYPE_AUTO visit(Visitor &&visitor, Vs &&... vs)
    DECLTYPE_AUTO_RETURN(
        (variant_detail::variant_all(
             variant_array<bool, sizeof...(Vs)>{{!vs.valueless_by_exception()...}})
             ? (void)0
             : throw_bad_variant_access()),
        variant_detail::visitation::Variant::visit_value(variant_forward<Visitor>(visitor),
                                                 variant_forward<Vs>(vs)...))
#endif

  template <typename... Ts>
  inline auto swap(Variant<Ts...> &lhs,
                   Variant<Ts...> &rhs) noexcept(noexcept(lhs.swap(rhs)))
      -> decltype(lhs.swap(rhs)) {
    lhs.swap(rhs);
  }

  namespace variant_detail {

    template <typename T, typename...>
    using enabled_type = T;

    namespace hash {

      template <typename H, typename K>
      constexpr bool meets_requirements() noexcept {
        return std::is_copy_constructible<H>::value &&
               std::is_move_constructible<H>::value &&
               variant_is_invocable_r<std::size_t, H, const K &>::value;
      }

      template <typename K>
      constexpr bool is_enabled() noexcept {
        using H = std::hash<K>;
        return meets_requirements<H, K>() &&
               std::is_default_constructible<H>::value &&
               std::is_copy_assignable<H>::value &&
               std::is_move_assignable<H>::value;
      }

    }  // namespace hash

  }  // namespace variant_detail

#undef AUTO
#undef AUTO_RETURN

#undef AUTO_REFREF
#undef AUTO_REFREF_RETURN

#undef DECLTYPE_AUTO
#undef DECLTYPE_AUTO_RETURN

}  // namespace core
}  // namespace ara

namespace std {

  template <typename... Ts>
  struct hash<ara::core::variant_detail::enabled_type<
      ara::core::Variant<Ts...>,
      ara::core::variant_enable_if_t<ara::core::variant_all<ara::core::variant_detail::hash::is_enabled<
          ara::core::variant_remove_const_t<Ts>>()...>::value>>> {
    using argument_type = ara::core::Variant<Ts...>;
    using result_type = std::size_t;

    inline result_type operator()(const argument_type &v) const {
      using ara::core::variant_detail::visitation::Variant;
      std::size_t result =
          v.valueless_by_exception()
              ? 299792458  // Random value chosen by the universe upon creation
              : Variant::visit_alt(
#ifdef ARA_CORE_GENERIC_LAMBDAS
                    [](const auto &alt) {
                      using alt_type = ara::core::variant_decay_t<decltype(alt)>;
                      using value_type = ara::core::variant_remove_const_t<
                          typename alt_type::value_type>;
                      return hash<value_type>{}(alt.value);
                    }
#else
                    hasher{}
#endif
                    ,
                    v);
      return hash_combine(result, hash<std::size_t>{}(v.index()));
    }

    private:
#ifndef ARA_CORE_GENERIC_LAMBDAS
    struct hasher {
      template <typename Alt>
      inline std::size_t operator()(const Alt &alt) const {
        using alt_type = ara::core::variant_decay_t<Alt>;
        using value_type =
            ara::core::variant_remove_const_t<typename alt_type::value_type>;
        return hash<value_type>{}(alt.value);
      }
    };
#endif

    static std::size_t hash_combine(std::size_t lhs, std::size_t rhs) {
      return lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
    }
  };

  template <>
  struct hash<ara::core::Monostate> {
    using argument_type = ara::core::Monostate;
    using result_type = std::size_t;

    inline result_type operator()(const argument_type &) const noexcept {
      return 66740831;  // return a fundamentally attractive random value.
    }
  };

}  // namespace std

#endif  // TUSIMPLEAP_ARA_CORE_VARIANT_HPP_

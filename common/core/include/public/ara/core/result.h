

#ifndef TUSIMPLEAP_ARA_CORE_RESULT_H_
#define TUSIMPLEAP_ARA_CORE_RESULT_H_

//#include <boost/variant.hpp>
#include <iostream>
#include <memory>
#include <system_error>
#include <type_traits>
#include <utility>

#include "ara/core/abort.h"
#include "ara/core/error_code.h"
#include "ara/core/optional.h"

namespace ara {
namespace core {

template <typename T, typename E>
class Result;
// Re-implementation of C++14's std::enable_if_t
template <bool Condition, typename U = void>
using enable_if_t = typename std::enable_if<Condition, U>::type;

// Re-implementation of C++14's std::result_of_t
template <typename U>
using result_of_t = typename std::result_of<U>::type;

/// @brief Trait that detects whether a type is a Result<...>
template <typename U>
constexpr bool is_result = false;

template <typename U, typename G>
constexpr bool is_result<Result<U, G>> = true;

template <typename F>
using CallableVoidReturnsResult = enable_if_t<is_result<result_of_t<F()>>>;

template <typename F>
using CallableVoidReturnsNoResult = enable_if_t<!is_result<result_of_t<F()>>>;

template <typename F, typename T>
using CallableReturnsResult = enable_if_t<is_result<result_of_t<F(T const&)>>>;

template <typename F, typename T>
using CallableReturnsNoResult =
    enable_if_t<!is_result<result_of_t<F(T const&)>>>;

template <typename U, typename... Args>
constexpr bool has_as_first_checker = false;

template <typename U>
constexpr bool has_as_first_checker<U> = false;

template <typename U, typename... Args>
constexpr bool has_as_first_checker<U, U, Args...> = true;

template <typename U, typename A, typename... Args>
constexpr bool has_as_first_checker<U, A, Args...> = false;

template <typename U, typename... Args>
constexpr bool has_as_first = has_as_first_checker<
    typename std::remove_reference<U>::type,
    typename std::remove_reference<Args>::type...>;

/// @uptrace{SWS_CORE_00701}
template <typename T, typename E = ErrorCode>
class Result final {
  // boost::variant<T, E> mData;
  Optional<T> mData;
  Optional<E> mError;

public:
  /// @uptrace{SWS_CORE_00711}
  using value_type = T;

  /// @uptrace{SWS_CORE_00712}
  using error_type = E;

  /// @uptrace{SWS_CORE_00725}
  Result(const Result& other) :
      mData(other.mData), mError(other.mError) {}

  /// @uptrace{SWS_CORE_00726}
  Result(Result&& other) :
      mData(std::move(other.mData)), mError(std::move(other.mError)) {}

  /// @uptrace{SWS_CORE_00721}
  Result(const T& t) : mData(t), mError(nullopt) {}

  /// @uptrace{SWS_CORE_00722}
  Result(T&& t) : mData(std::move(t)), mError(nullopt) {}

  /// @uptrace{SWS_CORE_00723}
  explicit Result(const E& e) :mData(nullopt), mError(e) {}
  // explicit Result(const E& e) : mData(e) {}

  /// @uptrace{SWS_CORE_00724}
  // explicit Result(E&& e) noexcept(
  //    std::is_nothrow_move_constructible<T>::value&&
  //    std::is_nothrow_move_constructible<E>::value) : mData(std::move(e)) {}
  explicit Result(E&& e) noexcept(
      std::is_nothrow_move_constructible<T>::value&&
          std::is_nothrow_move_constructible<E>::value) :
      mData(nullopt),
      mError(std::move(e)) {
  }

  /// @uptrace{SWS_CORE_00727}
  ~Result() = default;

  /// @uptrace{SWS_CORE_00731}
  static Result FromValue(const T& t) { return Result(t); }

  /// @uptrace{SWS_CORE_00732}
  static Result FromValue(T&& t) { return Result(std::move(t)); }

  /// @uptrace{SWS_CORE_00733}
  template <
      typename... Args,
      typename = typename std::enable_if<
          true && std::is_constructible<T, Args&&...>::value &&
          !has_as_first<T, Args...> && !has_as_first<Result, Args...>>::type>
  static Result FromValue(Args&&... args) {
    return Result(T(std::forward<Args>(args)...));
  }

  /// @uptrace{SWS_CORE_00734}
  static Result FromError(const E& e) { return Result(e); }

  /// @uptrace{SWS_CORE_00735}
  static Result FromError(E&& e) { return Result(std::move(e)); }

  /// @uptrace{SWS_CORE_00736}
  template <
      typename... Args,
      typename = typename std::enable_if<
          true && std::is_constructible<E, Args&&...>::value &&
          !has_as_first<E, Args...> && !has_as_first<Result, Args...>>::type>
  static Result FromError(Args&&... args) {
    return Result(E(std::forward<Args>(args)...));
  }

  /// @uptrace{SWS_CORE_00741}
  Result& operator=(const Result& other) & {
    mData = other.mData;
    mError = other.mError;
    return *this;
  }

  /// @uptrace{SWS_CORE_00742}
  Result& operator=(Result&& other) & noexcept(
      std::is_nothrow_move_constructible<T>::value&&
          std::is_nothrow_move_assignable<T>::value&&
              std::is_nothrow_move_constructible<E>::value&&
                  std::is_nothrow_move_assignable<E>::value) {
    mData = std::move(other.mData);
    mError = std::move(other.mError);
    return *this;
  }
  // ----------------------------------------

  /// @uptrace{SWS_CORE_00743}
  template <typename... Args>
  void EmplaceValue(Args&&... args) {
    // Unfortunately, Boost.Variant does not support emplace(), so we fall
    // back to assignment here.
    mError.reset();
    mData = std::move(T(std::forward<Args>(args)...));
  }

  /// @uptrace{SWS_CORE_00744}
  template <typename... Args>
  void EmplaceError(Args&&... args) {
    // Unfortunately, Boost.Variant does not support emplace(), so we fall
    // back to assignment here.
    // mData = std::move(E(std::forward<Args>(args)...));
    mData.reset();
    mError = std::move(E(std::forward<Args>(args)...));
  }

  /// @uptrace{SWS_CORE_00745}
  void Swap(Result& other) noexcept(
      std::is_nothrow_move_constructible<T>::value&&
          std::is_nothrow_move_assignable<T>::value&&
              std::is_nothrow_move_constructible<E>::value&&
                  std::is_nothrow_move_assignable<E>::value) {
    std::swap(mData, other.mData);
    std::swap(mError, other.mError);
  }

  /// @uptrace{SWS_CORE_00752}
  explicit operator bool() const noexcept { return HasValue(); }

  /// @uptrace{SWS_CORE_00751}
  // bool HasValue() const noexcept { return mData.which() == 0; }
  bool HasValue() const noexcept { return mData.has_value(); }

  /// @uptrace{SWS_CORE_00753}
  const T& operator*() const& {
    // const T* ptr = boost::get<T>(&mData);
    // if (ptr != nullptr) { return *ptr; }
    // std::abort();
    return *mData;
  }

  T&& operator*() && {
    // T* ptr = boost::get<T>(&mData);
    // if (ptr != nullptr) { return std::move(*ptr); }
    // std::abort();
    return std::move(*mData);
  }

  /// @uptrace{SWS_CORE_00754}
  const T* operator->() const { return std::addressof(Value()); }

  /// @uptrace{SWS_CORE_00755}
  const T& Value() const& {
    // const T* ptr = boost::get<T>(&mData);
    // if (ptr != nullptr) { return *ptr; }
    // std::abort();
    // Abort("ara::core::Result value() called but NOT a value!");
    return *mData;
  }

  /// @uptrace{SWS_CORE_00756}
  T&& Value() && {
    // T* ptr = boost::get<T>(&mData);
    // if (ptr != nullptr) { return std::move(*ptr); }
    // std::abort();
    // Abort("ara::core::Result value() called but NOT a value!");
    return std::move(*mData);
  }

  /// @uptrace{SWS_CORE_00757}
  const E& Error() const& {
    // const E* ptr = boost::get<E>(&mData);
    // if (ptr != nullptr) { return *ptr; }
    // std::abort();
    // Abort("ara::core::Result error() called but NOT an error!");
    return *mError;
  }

  /// @uptrace{SWS_CORE_00758}
  E&& Error() && {
    // E* ptr = boost::get<E>(&mData);
    // if (ptr != nullptr) { return std::move(*ptr); }
    // std::abort();
    // Abort("ara::core::Result error() called but NOT an error!");
    return std::move(*mError);
  }

  /// @uptrace{SWS_CORE_00770}
  Optional<T> Ok() const& {
    // const T* ptr = boost::get<T>(&mData);
    // return (ptr != nullptr) ? Optional<T>(*ptr) : Optional<T>(nullopt);
    return mData;
  }
  /// @uptrace{SWS_CORE_00771}
  Optional<T> Ok() && {
    // T* ptr = boost::get<T>(&mData);
    // return (ptr != nullptr) ? Optional<T>(std::move(*ptr)) :
    // Optional<T>(nullopt);
    return mData;
  }
  /// @uptrace{SWS_CORE_00772}
  Optional<E> Err() const& {
    // const E* ptr = boost::get<E>(&mData);
    // return (ptr != nullptr) ? Optional<E>(*ptr) : Optional<E>(nullopt);
    return mError;
  }
  /// @uptrace{SWS_CORE_00773}
  Optional<E> Err() && {
    // E* ptr = boost::get<E>(&mData);
    // return (ptr != nullptr) ? Optional<E>(std::move(*ptr)) :
    // Optional<E>(nullopt);
    return mError;
  }

  /// @uptrace{SWS_CORE_00761}
  template <typename U>
  T ValueOr(U&& defaultValue) const& {
    // return HasValue() ? Value() :
    // static_cast<T>(std::forward<U>(defaultValue));
    return mData.value_or(defaultValue);
  }

  /// @uptrace{SWS_CORE_00762}
  template <typename U>
  T ValueOr(U&& defaultValue) && {
    // return HasValue() ? std::move(Value()) :
    // static_cast<T>(std::forward<U>(defaultValue));
    return std::move(mData).value_or(defaultValue);
  }

  /// @uptrace{SWS_CORE_00763}
  template <typename G>
  E ErrorOr(G&& defaultError) const {
    // return HasValue() ? static_cast<E>(std::forward<G>(defaultError)) :
    // Error();
    return mError.value_or(defaultError);
  }

  /// @uptrace{SWS_CORE_00765}
  template <typename G>
  bool CheckError(G&& e) const {
    return HasValue() ? false : (Error() == static_cast<E>(std::forward<G>(e)));
  }

#ifndef ARA_NO_EXCEPTIONS
  /// @uptrace{SWS_CORE_00766}
  const T& ValueOrThrow() const& noexcept(false) {
    if (HasValue()) {
      return *mData;
    }
    mError->ThrowAsException();
    // const T* ptr = boost::get<T>(&mData);
    // if (ptr != nullptr) { return *ptr; }

    //// TODO: check if mData is uninitialized (-> monostate)
    // const E* e = boost::get<E>(&mData);

    // e->ThrowAsException();
  }

  /// @uptrace{SWS_CORE_00769}
  T&& ValueOrThrow() && noexcept(false) {
    if (HasValue()) {
      return std::move(*mData);
    }
    mError->ThrowAsException();
    // T* ptr = boost::get<T>(&mData);
    // if (ptr != nullptr) { return std::move(*ptr); }

    //// TODO: check if mData is uninitialized (-> monostate)
    // E* e = boost::get<E>(&mData);

    // e->ThrowAsException();
  }
#endif
  /// @uptrace{SWS_CORE_00767}
  template <typename F>
  T Resolve(F&& f) const {
    return HasValue() ? Value() : std::forward<F>(f)(Error());
  }

public:
  /// @uptrace{SWS_CORE_00768}
  template <typename F, typename = CallableReturnsResult<F, T>>
  auto Bind(F&& f) const -> decltype(f(Value())) {
    using U = decltype(f(Value()));
    return HasValue() ? std::forward<F>(f)(Value()) : U(Error());
  }

  /// @uptrace{SWS_CORE_00768}
  template <typename F, typename = CallableReturnsNoResult<F, T>>
  auto Bind(F&& f) const -> Result<decltype(f(Value())), E> {
    using U = decltype(f(Value()));
    using R = Result<U, E>;
    return HasValue() ? std::forward<F>(f)(Value()) : R(Error());
  }
};

/// @uptrace{SWS_CORE_00796}
template <typename T, typename E>
inline void swap(Result<T, E>& lhs, Result<T, E>& rhs) noexcept(
    noexcept(lhs.Swap(rhs))) {
  lhs.Swap(rhs);
}

/// @uptrace{SWS_CORE_00780}
template <typename T, typename E>
bool operator==(const Result<T, E>& lhs, const T& rhs) {
  return lhs.HasValue() ? (lhs.Value() == rhs) : false;
}
/// @uptrace{SWS_CORE_00781}
template <typename T, typename E>
bool operator!=(const Result<T, E>& lhs, const T& rhs) {
  return !(lhs == rhs);
}

/// @uptrace{SWS_CORE_00782}
template <typename T, typename E>
bool operator==(const T& lhs, const Result<T, E>& rhs) {
  return rhs == lhs;
}
/// @uptrace{SWS_CORE_00783}
template <typename T, typename E>
bool operator!=(const T& lhs, const Result<T, E>& rhs) {
  return !(rhs == lhs);
}

/// @uptrace{SWS_CORE_00784}
template <typename T, typename E>
bool operator==(const Result<T, E>& lhs, const E& rhs) {
  return lhs.HasValue() ? false : lhs.Error() == rhs;
}
/// @uptrace{SWS_CORE_00785}
template <typename T, typename E>
bool operator!=(const Result<T, E>& lhs, const E& rhs) {
  return !(lhs == rhs);
}

/// @uptrace{SWS_CORE_00786}
template <typename T, typename E>
bool operator==(const E& lhs, const Result<T, E>& rhs) {
  return rhs == lhs;
}
/// @uptrace{SWS_CORE_00787}
template <typename T, typename E>
bool operator!=(const E& lhs, const Result<T, E>& rhs) {
  return !(rhs == lhs);
}

/// @uptrace{SWS_CORE_00788}
template <typename T, typename E>
bool operator==(const Result<T, E>& lhs, const Result<T, E>& rhs) {
  return lhs.HasValue() ? lhs.Value() == rhs : lhs.Error() == rhs;
}

/// @uptrace{SWS_CORE_00789}
template <typename T, typename E>
bool operator!=(const Result<T, E>& lhs, const Result<T, E>& rhs) {
  return !(lhs == rhs);
}

//
// Specialization for void
//

/// @uptrace{SWS_CORE_00801}
template <typename E>
class Result<void, E> final {
  // Cannot use 'void' for variant, so define our own (default-constructible)
  // dummy type instead.
  // struct dummy {};
  // using T = dummy;
  // boost::variant<T, E> mData;
  Optional<E> mError;

public:
  /// @uptrace{SWS_CORE_00811}
  using value_type = void;

  /// @uptrace{SWS_CORE_00812}
  using error_type = E;

  /// @uptrace{SWS_CORE_00821}
  // Result() noexcept : mData(T {}) {}
  Result() noexcept : mError(nullopt) {}

  /// @uptrace{SWS_CORE_00823}
  // explicit Result(const E& e) : mData(e) {}
  explicit Result(const E& e) : mError(e) {}

  /// @uptrace{SWS_CORE_00824}
  // explicit Result(E&& e) : mData(std::move(e)) {}
  explicit Result(E&& e) : mError(std::move(e)) {}

  /// @uptrace{SWS_CORE_00825}
  Result(const Result& other) : mError(other.mError) {}

  /// @uptrace{SWS_CORE_00826}
  Result(Result&& other) noexcept : mError(std::move(other.mError)) {}

  /// @uptrace{SWS_CORE_00827}
  ~Result() = default;

  /// @uptrace{SWS_CORE_00831}
  static Result FromValue() { return Result(); }

  /// @uptrace{SWS_CORE_00834}
  static Result FromError(const E& e) { return Result(e); }

  /// @uptrace{SWS_CORE_00835}
  static Result FromError(E&& e) { return Result(std::move(e)); }

  /// @uptrace{SWS_CORE_00836}
  template <
      typename... Args,
      typename = typename std::enable_if<
          true && std::is_constructible<E, Args&&...>::value &&
          !has_as_first<E, Args...> && !has_as_first<Result, Args...>>::type>
  static Result FromError(Args&&... args) {
    return Result(E {std::forward<Args>(args)...});
  }

  /// @uptrace{SWS_CORE_00841}
  Result& operator=(const Result& other) & {
    // mData = other.mData;
    mError = other.mError;
    return *this;
  }

  /// @uptrace{SWS_CORE_00842}
  Result& operator=(Result&& other) & noexcept(
      std::is_nothrow_move_constructible<E>::value&&
          std::is_nothrow_move_assignable<E>::value) {
    // mData = std::move(other.mData);
    mError = std::move(other.mError);
    return *this;
  }

  /// @uptrace{SWS_CORE_00843}
  template <typename... Args>
  void EmplaceValue(Args&&... args) noexcept {
    // mData = T(args...);
  }

  /// @uptrace{SWS_CORE_00844}
  template <typename... Args>
  void EmplaceError(Args&&... args) {
    // Unfortunately, Boost.Variant does not support emplace(), so we fall
    // back to assignment here.
    // mData = std::move(E(std::forward<Args>(args)...));
    mError = std::move(E(std::forward<Args>(args)...));
  }

  /// @uptrace{SWS_CORE_00845}
  void Swap(Result& other) noexcept(
      std::is_nothrow_move_constructible<E>::value&&
          std::is_nothrow_move_assignable<E>::value) {
    using std::swap;
    // swap(mData, other.mData);
    swap(mError, other.mError);
  }

  /// @uptrace{SWS_CORE_00852}
  explicit operator bool() const noexcept { return HasValue(); }

  /// @uptrace{SWS_CORE_00851}
  // bool HasValue() const noexcept { return mData.which() == 0; }
  bool HasValue() const noexcept { return !mError.has_value(); }

  /// @uptrace{SWS_CORE_00855}
  void Value() const {
    if (HasValue()) {
      return;
    }
    // const T* ptr = boost::get<T>(&mData);
    // if (ptr != nullptr) { return; }
    // std::abort();
    // Abort("ara::core::Result value() called but NOT a (void) value!");
  }

  /// @uptrace{SWS_CORE_00857}
  const E& Error() const& {
    return *mError;
    // const E* ptr = boost::get<E>(&mData);
    // if (ptr != nullptr) { return *ptr; }
    // std::abort();
    // Abort("ara::core::Result error() called but NOT an error!");
  }

  /// @uptrace{SWS_CORE_00858}
  E&& Error() && {
    return std::move(*mError);
    // E* ptr = boost::get<E>(&mData);
    // if (ptr != nullptr) { return std::move(*ptr); }
    // std::abort();
    // Abort("ara::core::Result error() called but NOT an error!");
  }

  /// @uptrace{SWS_CORE_00868}
  Optional<E> Err() const& {
    return mError;
    // const E* ptr = boost::get<E>(&mData);
    // return (ptr != nullptr) ? Optional<E>(*ptr) : Optional<E>(nullopt);
  }
  /// @uptrace{SWS_CORE_00869}
  Optional<E> Err() && {
    return mError;
    // E* ptr = boost::get<E>(&mData);
    // return (ptr != nullptr) ? Optional<E>(std::move(*ptr)) :
    // Optional<E>(nullopt);
  }

  /// @uptrace{SWS_CORE_00863}
  template <typename G>
  E ErrorOr(G&& defaultError) const {
    // return HasValue() ? static_cast<E>(std::forward<G>(defaultError)) :
    // Error();
    return mError.value_or(defaultError);
  }

  /// @uptrace{SWS_CORE_00865}
  template <typename G>
  bool CheckError(G&& e) const {
    return HasValue() ? false : (Error() == static_cast<E>(std::forward<G>(e)));
  }

#ifndef ARA_NO_EXCEPTIONS
  /// @uptrace{SWS_CORE_00866}
  void ValueOrThrow() noexcept(false) {
    if (HasValue()) {
      return;
    }
    mError->ThrowAsException();
    // const T* ptr = boost::get<T>(&mData);
    // if (ptr != nullptr) return;

    // E* e = boost::get<E>(&mData);
    // e->ThrowAsException();
  }
#endif
  /// @uptrace{SWS_CORE_00867}
  template <typename F>
  void Resolve(F&& f) const {
    return HasValue() ? Value() : std::forward<F>(f)(Error());
  }

public:
  /// @uptrace{SWS_CORE_00870}
  template <typename F, typename = CallableVoidReturnsResult<F>>
  auto Bind(F&& f) const -> decltype(f()) {
    using U = decltype(f(Value()));
    return HasValue() ? std::forward<F>(f)() : U(Error());
  }

  /// @uptrace{SWS_CORE_00870}
  template <typename F, typename = CallableVoidReturnsNoResult<F>>
  auto Bind(F&& f) const -> Result<decltype(f()), E> {
    using U = decltype(f());
    using R = Result<U, E>;
    return HasValue() ? std::forward<F>(f)() : R(Error());
  }
};

}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_RESULT_H_

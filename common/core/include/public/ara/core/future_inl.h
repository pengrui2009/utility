/**
 * @file
 * @brief Interface to class ara::core::Future-Inl
 *
 * ara::core::Future is a composition of basic features of std::future
 * and methods borrowed from the C++ proposal N3857.
 */

#ifndef TUSIMPLEAP_ARA_CORE_FUTURE_INL_HPP_
#define TUSIMPLEAP_ARA_CORE_FUTURE_INL_HPP_
#include <atomic>

#include "ara/core/result.h"
#include "ara/core/future_error_domain.h"

namespace ara {
namespace core {

/* Forward declaration */
template <typename T, typename E>
class Promise;

template <typename T, typename E>
class Future;

template <typename U>
constexpr bool is_future = false;

template <typename U, typename G>
constexpr bool is_future<Future<U, G>> = true;

template <typename F, typename R>
using FuncReturnType = typename std::result_of_t<F(const R&)>;

template <bool A, bool B, typename F = void, typename R = void>
struct ThenReturn {};

template <typename F, typename R>
struct ThenReturn<true, false, F, R> {
  using type = FuncReturnType<F, R>;
};

template <typename F, typename R>
struct ThenReturn<false, true, F, R> {
  using type = Future<typename FuncReturnType<F, R>::value_type, typename FuncReturnType<F, R>::error_type>;
};

template <typename F, typename R>
struct ThenReturn<false, false, F, R> {
  using type = Future<FuncReturnType<F, R>, ErrorCode>;
};

template <typename F, typename R>
using ThenReturnType =
    typename ThenReturn<is_future<FuncReturnType<F, R>>, is_result<FuncReturnType<F, R>>, F, R>::type;

template <bool A, bool B, bool C, typename V = void, typename E = void, typename F = void, typename R = void>
class PromiseForLambda {};

template <typename V, typename E, typename F, typename R>
class PromiseForLambda<true, false, false, V, E, F, R> final {
  using DF = std::decay_t<F>;

public:
  PromiseForLambda(Promise<V, E>&& p, F&& f) noexcept : _p(std::move(p)), _f(std::forward<F>(f)) {}

  PromiseForLambda(PromiseForLambda&& other) noexcept {
    if (other.PromiseCheck()) {
      new (&_f) DF(static_cast<F&&>(other._f));
      _p = other.stealPromise();
    }
  }

  PromiseForLambda& operator=(PromiseForLambda&&) = delete;
  Promise<V, E> stealPromise() noexcept {
    _f.~DF();
    return std::move(_p);
  }
  void SetResult(const R& r) {
    _p.SetResult(_f(r).GetResult());
    auto ready = ready_.load(std::memory_order_acquire);
    if (!ready) {
      ready_ = true;
    }
  }

  ~PromiseForLambda() {
    auto ready = ready_.load(std::memory_order_acquire);
    if (PromiseCheck() ||
        ready) {
      stealPromise();
    }
  }

private:
  bool PromiseCheck() {
    return !_p.FullFilled();
  }

  Promise<V, E> _p{Promise<V, E>::makeEmpty()};
  std::atomic<bool> ready_{false};
  union {
    F _f;
  };
};

template <typename V, typename E, typename F, typename R>
class PromiseForLambda<false, true, false, V, E, F, R> final {
  using DF = std::decay_t<F>;

public:
  PromiseForLambda(Promise<V, E>&& p, F&& f) noexcept : _p(std::move(p)), _f(std::forward<F>(f)) {}
  PromiseForLambda(PromiseForLambda&& other) noexcept {
    if (other.PromiseCheck()) {
      new (&_f) DF(static_cast<F&&>(other._f));
      _p = other.stealPromise();
    }
  }

  PromiseForLambda& operator=(PromiseForLambda&&) = delete;
  Promise<V, E> stealPromise() noexcept {
    _f.~DF();
    return std::move(_p);
  }

  ~PromiseForLambda() {
    auto ready = ready_.load(std::memory_order_acquire);
    if (PromiseCheck() ||
        ready) {
      stealPromise();
    }
  }

  void SetResult(const R& r) {
    _p.SetResult(_f(r));
    auto ready = ready_.load(std::memory_order_acquire);
    if (!ready) {
      ready_ = true;
    }
  }

private:
  bool PromiseCheck() {
    return !_p.FullFilled();
  }

  Promise<V, E> _p{Promise<V, E>::makeEmpty()};
  std::atomic<bool> ready_{false};
  union {
    F _f;
  };
};

template <typename V, typename E, typename F, typename R>
class PromiseForLambda<false, false, true, V, E, F, R> final {
  using DF = std::decay_t<F>;

public:
  PromiseForLambda(Promise<V, E>&& p, F&& f) noexcept : _p(std::move(p)), _f(std::forward<F>(f)) {}

  PromiseForLambda(PromiseForLambda&& other) noexcept {
    if (other.PromiseCheck()) {
      new (&_f) DF(static_cast<F&&>(other._f));
      _p = other.stealPromise();
    }
  }

  PromiseForLambda& operator=(PromiseForLambda&&) = delete;

  Promise<V, E> stealPromise() noexcept {
    _f.~DF();
    return std::move(_p);
  }

  void SetResult(const R& r) {
    _f(r);
    _p.SetResult(Result<void>());
    auto ready = ready_.load(std::memory_order_acquire);
    if (!ready) {
      ready_ = true;
    }
  }

  ~PromiseForLambda() {
    auto ready = ready_.load(std::memory_order_acquire);
    if (PromiseCheck() ||
        ready) {
      stealPromise();
    }
  }

private:
  bool PromiseCheck() {
    return !_p.FullFilled();
  }

  Promise<V, E> _p{Promise<V, E>::makeEmpty()};
  std::atomic<bool> ready_{false};
  union {
    F _f;
  };
};

template <typename V, typename E, typename F, typename R>
class PromiseForLambda<false, false, false, V, E, F, R> final {
  using DF = std::decay_t<F>;

public:
  PromiseForLambda(Promise<V, E>&& p, F&& f) noexcept : _p(std::move(p)), _f(std::forward<F>(f)) {}

  PromiseForLambda(PromiseForLambda&& other) noexcept {
    if (other.PromiseCheck()) {
      new (&_f) DF(static_cast<F&&>(other._f));
      _p = other.stealPromise();
    }
  }

  PromiseForLambda& operator=(PromiseForLambda&&) = delete;
  Promise<V, E> stealPromise() noexcept {
    _f.~DF();
    return std::move(_p);
  }
  void SetResult(const R& r) {
    _p.set_value(_f(r));
    auto ready = ready_.load(std::memory_order_acquire);
    if (!ready) {
      ready_ = true;
    }
  }

  ~PromiseForLambda() {
    auto ready = ready_.load(std::memory_order_acquire);
    if (PromiseCheck() ||
        ready) {
      stealPromise();
    }
  }

private:
  bool PromiseCheck() {
    return !_p.FullFilled();
  }

  Promise<V, E> _p{Promise<V, E>::makeEmpty()};
  std::atomic<bool> ready_{false};
  union {
    DF _f;
  };
};

template <typename V, typename E, typename F, typename R>
auto MakePromiseForLambda(Promise<V, E>&& p, F&& f) {
  return PromiseForLambda<is_future<FuncReturnType<F, R>>,
                          is_result<FuncReturnType<F, R>>,
                          std::is_void<FuncReturnType<F, R>>::value,
                          V,
                          E,
                          F,
                          R>(std::move(p), static_cast<F&&>(f));
}

}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_FUTURE_INL_HPP_

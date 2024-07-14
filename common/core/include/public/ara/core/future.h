

/**
 * @file
 * @brief Interface to class ara::core::Future
 *
 * ara::core::Future is a composition of basic features of std::future
 * and methods borrowed from the C++ proposal N3857.
 */

#ifndef TUSIMPLEAP_ARA_CORE_FUTURE_HPP_
#define TUSIMPLEAP_ARA_CORE_FUTURE_HPP_

#include <cassert>
#include <chrono>
#include <system_error>

#include "ara/core/core_error_domain.h"
#include "ara/core/error_code.h"
#include "ara/core/exception.h"
#include "ara/core/future_error_domain.h"
#include "ara/core/future_inl.h"
#include "ara/core/internal/core.h"
#include "ara/core/result.h"

namespace ara {
namespace core {

/**
 * @brief Provides ara::core specific Future operations to collect the results
 * of an asynchronous call.
 *
 * Much of its functionality is delegated to std::future and all methods that
 * resemble std::future are guaranteed to behave the same.
 *
 * If the valid() member function of an instance returns true, all other methods
 * are guaranteed to work on that instance. Otherwise, they may fail with or
 * without an exception. A thrown exception will be of type std::future_error.
 *
 * Having an invalid future will usually happen when the future was moved from
 * using the move constructor or move assignment.
 *
 * @uptrace{SWS_CORE_00321}
 */
template <typename T, typename E = ErrorCode>
class Future final {
public:
  /// Alias type for T
  using ValueType = T;
  using ErrorType = E;
  /// Alias type for the Promise type collaborating with this Future type
  using PromiseType = Promise<T, E>;
  /// Alias type for the FutureStatus type
  using R = Result<T, E>;

  /**
   * @brief Default constructor
   *
   * @uptrace{SWS_CORE_00322}
   */
  Future() noexcept = default;

  /**
   * @brief Destructor for Future objects
   *
   * This will also disable any callback that has been set.
   *
   * @uptrace{SWS_CORE_00333}
   */
  ~Future() noexcept {
    detach();
    if (destructor_callback_) {
      destructor_callback_();
    }
  }

  /**
   * @brief Copy constructor shall be disabled.
   *
   * @uptrace{SWS_CORE_00334}
   */
  Future(const Future& f) = delete;

  /**
   * @brief Copy assignment operator shall be disabled.
   *
   * @uptrace{SWS_CORE_00335}
   */
  Future& operator=(const Future& f) & = delete;

  /**
   * @brief Move construct from another instance.
   *
   * This function shall behave the same as
   * the corresponding std::future function.
   *
   * @uptrace{SWS_CORE_00323}
   */
  Future(Future&& other) noexcept :
      core_(std::exchange(other.core_, nullptr)) {}

  /**
   * @brief Move assign from another instance.
   *
   * @returns *this
   *
   * @uptrace{SWS_CORE_00325}
   */
  Future& operator=(Future&& other) & noexcept {
    if (this != &other) {
      detach();

      core_ = std::exchange(other.core_, nullptr);
    }
    return *this;
  }

  /**
   * @brief Get the result.
   *
   * Similar to get(), this call blocks until the value or an error is
   * available. However, this call will never throw an exception.
   *
   * @returns a Result with either a value or an error
   *
   * @uptrace{SWS_CORE_00336}
   */
  Result<T, E> GetResult() {
    if (this->core_ == nullptr) {
      return Result<T, E>(future_errc::no_state);
    }
    if (this->core_->GetCount() == 1 && !is_ready()) {
      return Result<T, E>(future_errc::broken_promise);
    }

    wait();

    if (this->core_->GetCount() == 1 && !is_ready()) {
      return Result<T, E>(future_errc::broken_promise);
    }

    auto fut = Future<T, E>(this->core_);
    this->core_ = nullptr;
    return std::move(std::move(fut).core_->GetResult());
  }

  /**
   * @brief Get the value.
   *
   * This call blocks until the result or an exception is available.
   *
   * @returns value of type T
   *
   * @uptrace{SWS_CORE_00326}
   */
#ifndef ARA_NO_EXCEPTIONS
  T get() { return GetResult().ValueOrThrow(); }
#endif

  /**
   * @brief Checks if the future is valid, i.e. if it has a shared state.
   *
   * @returns true if the future is usable, false otherwise
   *
   * @uptrace{SWS_CORE_00327}
   */
  bool valid() const noexcept { return this->core_ != nullptr; }

  /**
   * @brief Waits for a value or an exception to be available.
   *
   * After this method returns, get() is guaranteed to not block and
   * is_ready() will return true.
   *
   * @uptrace{SWS_CORE_00328}
   */
  void wait() const { core_->wait(); }

  /**
   * @brief Wait for the given period.
   *
   * If the Future becomes ready or the timeout is reached, the method
   * returns.
   *
   * @param timeout_duration maximal duration to wait for
   * @returns status that indicates whether the timeout hit or if a value is
   * available
   *
   * @uptrace{SWS_CORE_00329}
   */
  template <typename Rep, typename Period>
  future_status wait_for(
      const std::chrono::duration<Rep, Period>& timeoutDuration) const {
    return core_->wait_for(timeoutDuration);
  }

  /**
   * @brief Wait until the given time.
   *
   * If the Future becomes ready or the time is reached, the method returns.
   *
   * @param deadline latest point in time to wait
   * @returns status that indicates whether the time was reached or if a value
   * is available
   *
   * @uptrace{SWS_CORE_00330}
   */
  template <typename Clock, typename Duration>
  future_status wait_until(
      const std::chrono::time_point<Clock, Duration>& deadline) const {
    return core_->wait_until(deadline);
  }

  /**
   * @brief Register a function that gets called when the future becomes
   * ready.
   *
   * When @a func is called, it is guaranteed that get() will not block.
   *
   * @a func may be called in the context of this call or in the context of
   * Promise::set_value() or Promise::set_exception() or somewhere else.
   *
   * @param func a Callable to register to get the Future result or an
   * exception
   *
   * @returns a new Future instance for the result of the continuation
   *
   * @uptrace{SWS_CORE_00331}
   */
  template <typename F>
  auto then(F&& func) -> ThenReturnType<F, R> {
    using PromiseValueType = typename ThenReturnType<F, R>::ValueType;
    using PromiseErrorType = typename ThenReturnType<F, R>::ErrorType;

    Promise<PromiseValueType, PromiseErrorType> p;
    auto f = p.get_future();

    core_->SetCallback(
        [state = MakePromiseForLambda<PromiseValueType, PromiseErrorType, F, R>(
             std::move(p), static_cast<F&&>(func))](const R& result) mutable {
          state.SetResult(result);
        });
    return f;
  }

  /**
   * True when the future contains either a result or an exception.
   *
   * If is_ready() returns true, get() and the wait calls are guaranteed to
   * not block.
   *
   * @returns true if the Future contains data, false otherwise
   *
   * @uptrace{SWS_CORE_00332}
   */
  bool is_ready() const { return core_->is_ready(); }

private:
  /**
   * @brief Constructs a Future from a given std::future and a pointer to the
   * extra state.
   *
   * @param delegate_future std::future instance
   * @param extra_state state that is shared with the Promise
   */
  Future(Core<T, E>* core) : core_(core) {}

  Future(Core<T, E>* core, Function<void(void)>&& destructor_callback) :
      core_(core), destructor_callback_(std::move(destructor_callback)) {}

  /**
   * @brief use for free core_
   */
  void detach() noexcept {
    if (core_ != nullptr) {
      core_->detach_one();
      core_ = nullptr;
    }
  }

private:
  Core<T, E>* core_ = nullptr;
  Function<void(void)> destructor_callback_;

  template <typename, typename>
  friend class Core;
};

/**
 * @brief Provides ara::core specific Future operations to collect the results
 * of an asynchronous call.
 *
 * Much of its functionality is delegated to std::future and all methods that
 * resemble std::future are guaranteed to behave the same.
 *
 * If the valid() member function of an instance returns true, all other methods
 * are guaranteed to work on that instance. Otherwise, they may fail with or
 * without an exception. A thrown exception will be of type std::future_error.
 *
 * Having an invalid future will usually happen when the future was moved from
 * using the move constructor or move assignment.
 *
 * Specialization of class Future for "void" values.
 *
 * @uptrace{SWS_CORE_06221}
 */
template <typename E>
class Future<void, E> final {
public:
  /// Alias type for void
  using ValueType = void;
  using ErrorType = E;
  /// Alias type for the Promise type collaborating with this Future type
  using PromiseType = Promise<void, E>;
  /// Alias type for the FutureStatus type
  using R = Result<void, E>;

  /**
   * @brief Default constructor
   *
   * @uptrace{SWS_CORE_06222}
   */
  Future() noexcept = default;

  /**
   * @brief Destructor for Future objects
   *
   * This will also disable any callback that has been set.
   *
   * @uptrace{SWS_CORE_06233}
   */
  ~Future() noexcept {
    detach();
    if (destructor_callback_) {
      destructor_callback_();
    }
  }

  /**
   * @brief Copy constructor shall be disabled.
   *
   * @uptrace{SWS_CORE_06234}
   */
  Future(const Future& f) = delete;

  /**
   * @brief Copy assignment operator shall be disabled.
   *
   * @uptrace{SWS_CORE_06235}
   */
  Future& operator=(const Future& f) & = delete;

  /**
   * @brief Move construct from another instance.
   *
   * This function shall behave the same as the corresponding std::future
   * function
   *
   * @uptrace{SWS_CORE_06223}
   */
  Future(Future&& other) noexcept :
      core_(std::exchange(other.core_, nullptr)) {}

  /**
   * @brief Move assign from another instance.
   *
   * This function shall behave the same as the corresponding std::future
   * function.
   *
   * @uptrace{SWS_CORE_06225}
   */
  Future& operator=(Future&& other) & noexcept {
    if (this != &other) {
      detach();

      core_ = std::exchange(other.core_, nullptr);
    }
    return *this;
  }

  /**
   * @brief Get the result.
   *
   * Similar to get(), this call blocks until the value or an error is
   * available. However, this call will never throw an exception.
   *
   * @returns a Result with either a value or an error
   *
   * @uptrace{SWS_CORE_06236}
   */
  Result<void, E> GetResult() {
    if (this->core_ == nullptr) {
      return Result<void, E>(future_errc::no_state);
    }
    if (this->core_->GetCount() == 1 && !is_ready()) {
      return Result<void, E>(future_errc::broken_promise);
    }

    wait();

    if (this->core_->GetCount() == 1 && !is_ready()) {
      return Result<void, E>(future_errc::broken_promise);
    }

    auto fut = Future<void, E>(this->core_);
    this->core_ = nullptr;
    return std::move(std::move(fut).core_->GetResult());
  }

  /**
   * @brief Get the value.
   *
   * This call blocks until the result or an exception is available.
   *
   * @returns value of type void
   *
   * @uptrace{SWS_CORE_06226}
   */
#ifndef ARA_NO_EXCEPTIONS
  void get() { return GetResult().ValueOrThrow(); }
#endif

  /**
   * @brief Checks if the future is valid, i.e. if it has a shared state.
   *
   * @returns true if the future is usable, false otherwise
   *
   * @uptrace{SWS_CORE_06227}
   */
  bool valid() const noexcept { return this->core_ != nullptr; }

  /**
   * @brief Waits for a value or an exception to be available.
   *
   * After this method returns, get() is guaranteed to not block and
   * is_ready() will return true.
   *
   * @uptrace{SWS_CORE_06228}
   */
  void wait() const { core_->wait(); }

  /**
   * @brief Wait for the given period.
   *
   * If the Future becomes ready or the timeout is reached, the method
   * returns.
   *
   * @param timeout_duration maximal duration to wait for
   * @returns status that indicates whether the timeout hit or if a value is
   * available
   *
   * @uptrace{SWS_CORE_06229}
   */
  template <typename Rep, typename Period>
  future_status wait_for(
      const std::chrono::duration<Rep, Period>& timeoutDuration) const {
    return core_->wait_for(timeoutDuration);
  }

  /**
   * @brief Wait until the given time.
   *
   * If the Future becomes ready or the time is reached, the method returns.
   *
   * @param deadline latest point in time to wait
   * @returns status that indicates whether the time was reached or if a value
   * is available
   *
   * @uptrace{SWS_CORE_06230}
   */
  template <typename Clock, typename Duration>
  future_status wait_until(
      const std::chrono::time_point<Clock, Duration>& deadline) const {
    return core_->wait_until(deadline);
  }

  /**
   * @brief Register a function that gets called when the future becomes
   * ready.
   *
   * When @a func is called, it is guaranteed that get() will not block.
   *
   * @a func may be called in the context of this call or in the context of
   * Promise::set_value() or Promise::set_exception() or somewhere else.
   *
   * @returns a new Future instance for the result of the continuation
   *
   * @param func a Callable to register to get the Future result or an
   * exception
   *
   * @uptrace{SWS_CORE_06231}
   */
  template <typename F>
  auto then(F&& func) -> ThenReturnType<F, R> {
    using PromiseValueType = typename ThenReturnType<F, R>::ValueType;
    using PromiseErrorType = typename ThenReturnType<F, R>::ErrorType;

    Promise<PromiseValueType, PromiseErrorType> p;
    auto f = p.get_future();

    core_->SetCallback(
        [state = MakePromiseForLambda<PromiseValueType, PromiseErrorType, F, R>(
             std::move(p), static_cast<F&&>(func))](const R& result) mutable {
          state.SetResult(result);
        });
    return f;
  }

  /**
   * True when the future contains either a result or an exception.
   *
   * If is_ready() returns true, get() and the wait calls are guaranteed to
   * not block.
   *
   * @returns true if the Future contains data, false otherwise
   *
   * @uptrace{SWS_CORE_06232}
   */
  bool is_ready() const { return core_->is_ready(); }

private:
  /**
   * @brief Constructs a Future from a given std::future and a pointer to the
   * extra state.
   *
   * @param delegate_future std::future instance
   * @param extra_state state that is shared with the Promise
   */
  Future(Core<void, E>* core) : core_(core) {}

  Future(Core<void, E>* core, Function<void(void)>&& destructor_callback) :
      core_(core), destructor_callback_(std::move(destructor_callback)) {}

  /**
   * @brief use for free core_
   */
  void detach() {
    if (core_ != nullptr) {
      core_->detach_one();
      core_ = nullptr;
    }
  }

  Core<void, E>* core_ = nullptr;
  Function<void(void)> destructor_callback_;

  template <typename, typename>
  friend class Core;
};

}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_FUTURE_HPP_

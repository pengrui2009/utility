

/**
 * @file
 * @brief Interface to class ara::core::Future
 *
 * ara::core::Future is a composition of basic features of std::future
 * and methods borrowed from the C++ proposal N3857.
 */

#ifndef TUSIMPLEAP_ARA_CORE_INTERNAL_CORE_HPP_
#define TUSIMPLEAP_ARA_CORE_INTERNAL_CORE_HPP_

#include <atomic>
#include <cassert>
#include <chrono>
#include <exception>
#include <mutex>
#include <system_error>
#include <thread>

#include "ara/core/core_error_domain.h"
#include "ara/core/error_code.h"
#include "ara/core/exception.h"
#include "ara/core/functional.h"
#include "ara/core/future_error_domain.h"
#include "ara/core/future_inl.h"
#include "ara/core/result.h"
#include "ara/core/condition_variable.h"

namespace ara {
namespace core {

enum class State : uint8_t {
  Empty = 1 << 0,
  OnlyResult = 1 << 1,
  OnlyCallBack = 1 << 2,
  Done = 1 << 3,
};

constexpr State operator&(State a, State b) {
  return State(uint8_t(a) & uint8_t(b));
}
constexpr State operator|(State a, State b) {
  return State(uint8_t(a) | uint8_t(b));
}

template <typename T, typename E = ErrorCode>
class Core final {
public:
  using ValueType = T;
  using ErrorType = E;
  using PromiseType = Promise<T, E>;
  using Status = future_status;
  using R = Result<T, E>;
  using CallBack = Function<void(const R& t)>;

  /**
   * @brief Default constructor
   */
  Core() : state_(State::Empty), count_(1) {}

  /**
   * @brief Destructor for Core objects
   */
  ~Core() {
    if (callback_) {
      callback_.~CallBack();
      SetCallback(nullptr);
    }
  }

  /**
   * @brief Copy constructor shall be disabled.
   */
  Core(const Core&) = delete;

  /**
   * @brief Copy assignment operator shall be disabled.
   */
  Core& operator=(const Core&) = delete;

  /**
   * @brief Move constructor shall be disabled.
   */
  Core(Core&&) = delete;

  /**
   * @brief Move assignment operator shall be disabled.
   */
  Core& operator=(Core&&) = delete;

  /**
   * @brief Waits for a value or an exception to be available.
   *
   * After this method returns, get() is guaranteed to not block and
   * is_ready() will return true.
   */
  void wait() {
    while (!is_ready()) {
      std::this_thread::sleep_for(std::chrono::duration<double>(0.001));
    }
  }

  /**
   * @brief Wait for the given period.
   *
   * If the Future becomes ready or the timeout is reached, the method
   * returns.
   *
   * @param timeout_duration maximal duration to wait for
   * @returns status that indicates whether the timeout hit or if a value is
   * available
   */
  template <typename Rep, typename Period>
  future_status wait_for(
      const std::chrono::duration<Rep, Period>& timeout_duration) {
    if (count_ == 1) {
      if (is_ready()) {
        return future_status::ready;
      } else {
        return future_status::timeout;
      }
    }

    std::unique_lock<std::mutex> lck(mutex_);

    if (!cv_.wait_for(lck, timeout_duration, [&]() {
          return is_ready() || count_ == 1;
        })) {
      return future_status::timeout;
    }
    if (!is_ready()) {
      return future_status::timeout;
    }

    return future_status::ready;
  }

  /**
   * @brief Wait until the given time.
   *
   * If the Future becomes ready or the time is reached, the method returns.
   *
   * @param deadline latest point in time to wait
   * @returns status that indicates whether the time was reached or if a value
   * is available
   */
  template <typename Clock, typename Duration>
  future_status wait_until(
      const std::chrono::time_point<Clock, Duration>& deadline) {
    if (count_ == 1) {
      if (is_ready()) {
        return future_status::ready;
      } else {
        return future_status::timeout;
      }
    }

    std::unique_lock<std::mutex> lck(mutex_);

    if (!cv_.wait_until(
            lck, deadline, [&]() { return is_ready() || count_ == 1; })) {
      return future_status::timeout;
    }
    if (!is_ready()) {
      return future_status::timeout;
    }

    return future_status::ready;
  }

  /**
   * True when the future contains either a result or an exception.
   *
   * If is_ready() returns true, get() and the wait calls are guaranteed to
   * not block.
   *
   * @returns true if the Future contains data, false otherwise
   */
  bool is_ready() const {
    constexpr auto allowed = State::OnlyResult | State::Done;
    auto state = state_.load(std::memory_order_acquire);
    return state == (state & allowed);
  }

  /**
   *  @brief Returns an associated Future for type T.
   *
   * The returned Future is set as soon as this Promise receives the result
   * or an exception. This method must only be called once as it is not
   * allowed to have multiple Futures per Promise.
   *
   * @returns a Future for type T
   */
  Future<T, E> get_future() {
    ++count_;
    return Future<T, E>(this);
  }

  Future<T, E> get_future(Function<void(void)>&& callback) {
    ++count_;
    return Future<T, E>(this, std::move(callback));
  }

  /**
   * @brief Copy a Result into the shared state and make the state ready.
   */
  void SetResult(const Result<T, E>& result) {
    if (is_ready()) {
      ErrorCode(future_errc::promise_already_satisfied).ThrowAsException();
    }
    new (&this->result_) R(result);
    auto state = state_.load(std::memory_order_acquire);
    if (state == State::Empty) {
      if (state_.compare_exchange_strong(
              state,
              State::OnlyResult,
              std::memory_order_seq_cst)) {
        cv_.notify_all();
        return;
      }
    }
    if (state == State::OnlyCallBack) {
      state_.store(State::Done, std::memory_order_relaxed);
      callback_(result_);
    }
    cv_.notify_all();
  }

  /**
   * @brief Move a Result into the shared state and make the state ready
   */
  void SetResult(Result<T, E>&& result) {
    if (is_ready()) {
      ErrorCode(future_errc::promise_already_satisfied).ThrowAsException();
    }
    new (&this->result_) R(std::move(result));
    auto state = state_.load(std::memory_order_acquire);
    if (state == State::Empty) {
      if (state_.compare_exchange_strong(
              state,
              State::OnlyResult,
              std::memory_order_seq_cst)) {
        cv_.notify_all();
        return;
      }
    }
    if (state == State::OnlyCallBack) {
      state_.store(State::Done, std::memory_order_relaxed);
      callback_(result_);
    }
    cv_.notify_all();
  }

  void detach_one() {
    auto count = count_.fetch_sub(1, std::memory_order_acq_rel);
    if (count == 1) {
      delete this;
    }
  }

  void promise_detach() {
    auto count = count_.fetch_sub(1, std::memory_order_acq_rel);
    cv_.notify_all();
    if (count == 1) {
      delete this;
    }
  }

  /**
   * @brief use for free self
   */
  void detach() { delete this; }

  /**
   * @brief use for malloc self
   */
  static Core<T, E>* make() { return new (std::nothrow) Core<T, E>(); }

  /**
   * @brief Sets the callback.
   *
   * any callback that was set before is removed and replaced by the new
   * callback.
   *
   * note A mutex lock should be set before calling this function
   * @param callback The callback to be set.
   */
  void SetCallback(CallBack&& callback) {
    new (&callback_) CallBack(std::move(callback));
    auto state = state_.load(std::memory_order_acquire);
    if (state == State::Empty) {
      if (state_.compare_exchange_strong(
              state,
              State::OnlyCallBack,
              std::memory_order_seq_cst)) {
        return;
      }
    }

    if (state == State::OnlyResult) {
      state_.store(State::Done, std::memory_order_relaxed);
      callback_(result_);
      return;
    }
  }

  /**
   * @brief Get result
   */
  R& GetResult() & { return result_; }

  /**
   * @brief Get result
   */
  const R& GetResult() const& { return result_; }

  /**
   * @brief Get result
   */
  R&& GetResult() && { return std::move(result_); }

  /**
   * @brief Get result
   */
  const R&& GetResult() const&& { return std::move(result_); }

  /**
   * @brief Get count
   */
  std::uint8_t GetCount() { return count_; }

private:
  union {
    R result_;
  };
  std::atomic<State> state_;
  std::atomic<std::uint8_t> count_;
  CallBack callback_;

  ConditionVariable cv_;
  std::mutex mutex_;
};

}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_INTERNAL_CORE_HPP_

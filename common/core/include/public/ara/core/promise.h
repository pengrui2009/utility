

/// @file
/// @brief Interface to class ara::core::Promise
///
/// ara::core::Future is a composition of basic features of std::future
/// and methods borrowed from the C++ proposal N3857.
///
/// ara::core::Promise is the corresponding promise that returns an
/// ara::core::Future.

#ifndef TUSIMPLEAP_ARA_CORE_PROMISE_HPP_
#define TUSIMPLEAP_ARA_CORE_PROMISE_HPP_

#include <exception>
#include <system_error>

#include "ara/core/error_code.h"
#include "ara/core/future.h"
#include "ara/core/future_error_domain.h"
#include "ara/core/internal/core.h"
#include "ara/core/result.h"

namespace ara {
namespace core {
namespace internal {
struct EmptyConstruct {};
}  // namespace internal

template <typename T, typename E>
class Promise;

/**
 * @brief ara::core specific variant of std::promise class
 *
 * @uptrace{SWS_CORE_00340}
 */
template <typename T, typename E = ErrorCode>
class Promise final {
  using R = Result<T, E>;

public:
  /// Alias type for T
  using ValueType = T;

  /**
   * @brief Default constructor
   *
   * @uptrace{SWS_CORE_00341}
   */
  Promise() : retrieved_(false), core_(Core<T, E>::make()) {}

  Promise(internal::EmptyConstruct) noexcept :
      retrieved_(false), core_(nullptr) {}

  /**
   * @brief Destructor for Promise objects.
   *
   * This function shall behave the same as the corresponding std::promise
   * function.
   *
   * @uptrace{SWS_CORE_00349}
   */
  ~Promise() noexcept { detach(); }

  /**
   * @brief Copy constructor shall be disabled.
   *
   * @uptrace{SWS_CORE_00350}
   */
  Promise(const Promise&) = delete;

  /**
   * @brief Copy assignment operator shall be disabled.
   *
   * @uptrace{SWS_CORE_00351}
   */
  Promise& operator=(const Promise&) = delete;

  /**
   * @brief Move constructor.
   *
   * This function shall behave the same as the corresponding std::promise
   * function.
   *
   * @uptrace{SWS_CORE_00342}
   */
  Promise(Promise&& other) noexcept :
      retrieved_(std::exchange(other.retrieved_, false)),
      core_(std::exchange(other.core_, nullptr)) {}

  /**
   * @brief Move assignment.
   *
   * This function shall behave the same as the corresponding std::promise
   * function
   *
   * @uptrace{SWS_CORE_00343}
   */
  Promise& operator=(Promise&& other) noexcept {
    if (this != &other) {
      detach();

      retrieved_ = std::exchange(other.retrieved_, false);
      core_ = std::exchange(other.core_, nullptr);
    }
    return *this;
  }

  /**
   * @brief Create empty promise
   */
  static Promise<T, E> makeEmpty() noexcept {
    return Promise<T, E>(internal::EmptyConstruct {});
  }

  /**
   * @brief Swap the contents of this instance with another one’s.
   *
   * This function shall behave the same as the corresponding std::promise
   * function.
   *
   * @uptrace{SWS_CORE_00352}
   */
  void swap(Promise& other) noexcept {
    std::swap(retrieved_, other.retrieved_);
    std::swap(core_, other.core_);
  }

  /**
   *  @brief Returns an associated Future for type T.
   *
   * The returned Future is set as soon as this Promise receives the result
   * or an exception. This method must only be called once as it is not
   * allowed to have multiple Futures per Promise.
   *
   * @returns a Future for type T
   *
   * @uptrace{SWS_CORE_00344}
   */
  Future<T, E> get_future() {
    if (retrieved_) {
      ErrorCode(future_errc::future_already_retrieved).ThrowAsException();
    }
    retrieved_ = true;
    return core_->get_future();
  }

  Future<T, E> get_future(Function<void(void)>&& callback) {
    if (retrieved_) {
      ErrorCode(future_errc::future_already_retrieved).ThrowAsException();
    }
    retrieved_ = true;
    return core_->get_future(std::move(callback));
  }
  /**
   * @brief Copy a Result into the shared state and make the state ready.
   *
   * @uptrace{SWS_CORE_00355}
   */
  void SetResult(const Result<T, E>& result) { core_->SetResult(result); }

  /**
   * @brief Move a Result into the shared state and make the state ready
   *
   * @uptrace{SWS_CORE_00356}
   */
  void SetResult(Result<T, E>&& result) { core_->SetResult(std::move(result)); }

  /**
   * @brief Move an error into the shared state and make the state ready.
   *
   * @uptrace{SWS_CORE_00353}
   */
  void SetError(E&& err) {
    R r = R::FromError(std::move(err));
    core_->SetResult(std::move(r));
  }

  /**
   * @brief Copy an error into the shared state and make the state ready
   *
   * @uptrace{SWS_CORE_00354}
   */
  void SetError(const E& err) {
    R r = R::FromError(err);
    core_->SetResult(r);
  }

  /**
   * @brief Move the result into the future.
   *
   * @param value the value to store
   *
   * @uptrace{SWS_CORE_00346}
   */
  void set_value(T&& value) {
    R r = std::move(value);
    core_->SetResult(std::move(r));
  }

  /**
   * @brief Copy result into the future.
   *
   * @param value the value to store
   *
   * @uptrace{SWS_CORE_00345}
   */
  void set_value(const T& value) {
    R r = value;
    core_->SetResult(r);
  }

  bool FullFilled() {
    if (core_) {
      return core_->is_ready();
    }
    return true;
  }

private:
  /**
   * @brief use for free core_
   */
  void detach() {
    if (core_) {
      if (!retrieved_) {
        core_->detach();
      } else {
        core_->promise_detach();
      }
      core_ = nullptr;
    }
  }

private:
  bool retrieved_;
  Core<T, E>* core_;
};

/**
 * @brief Specialization of class Promise for "void" values
 *
 * @uptrace{SWS_CORE_06340}
 */
template <typename E>
class Promise<void, E> final {
  using R = Result<void, E>;

public:
  /// Alias type for void
  using ValueType = void;

  /**
   * @brief Default constructor
   *
   * @uptrace{SWS_CORE_06341}
   */
  Promise() : retrieved_(false), core_(Core<void, E>::make()) {}

  Promise(internal::EmptyConstruct) noexcept :
      retrieved_(false), core_(nullptr) {}

  /**
   * @brief Destructor for Promise objects.
   *
   * This function shall behave the same as the corresponding std::promise
   * function.
   *
   * @uptrace{SWS_CORE_06349}
   */
  ~Promise() { detach(); }

  /**
   * @brief Copy constructor shall be disabled.
   *
   * @uptrace{SWS_CORE_06350}
   */
  Promise(const Promise&) = delete;

  /**
   * @brief Copy assignment operator shall be disabled.
   *
   * @uptrace{SWS_CORE_06351}
   */
  Promise& operator=(const Promise&) = delete;

  /**
   * @brief Move constructor.
   *
   * This function shall behave the same as the corresponding std::promise
   * function.
   *
   * @uptrace{SWS_CORE_06342}
   */
  Promise(Promise&& other) noexcept :
      retrieved_(std::exchange(other.retrieved_, false)),
      core_(std::exchange(other.core_, nullptr)) {}

  /**
   * @brief Move assignment.
   *
   * This function shall behave the same as the corresponding std::promise
   * function
   *
   * @uptrace{SWS_CORE_06343}
   */
  Promise& operator=(Promise&& other) noexcept {
    if (this != &other) {
      detach();

      retrieved_ = std::exchange(other.retrieved_, false);
      core_ = std::exchange(other.core_, nullptr);
    }
    return *this;
  }

  /**
   * @brief Create empty promise
   */
  static Promise<void, E> makeEmpty() noexcept {
    return Promise<void, E>(internal::EmptyConstruct {});
  }

  /**
   * @brief Swap the contents of this instance with another one’s.
   *
   * This function shall behave the same as the corresponding std::promise
   * function.
   *
   * @uptrace{SWS_CORE_06352}
   */
  void swap(Promise& other) noexcept {
    std::swap(retrieved_, other.retrieved_);
    std::swap(core_, other.core_);
  }

  /**
   *  @brief Returns an associated Future.
   *
   * The returned Future is set as soon as this Promise receives the result
   * or an exception. This method must only be called once as it is not
   * allowed to have multiple Futures per Promise.
   *
   * @returns a Future
   *
   * @uptrace{SWS_CORE_06344}
   */
  Future<void, E> get_future() {
    if (retrieved_) {
      ErrorCode(future_errc::future_already_retrieved).ThrowAsException();
    }
    retrieved_ = true;
    return core_->get_future();
  }

  Future<void, E> get_future(Function<void(void)>&& callback) {
    if (retrieved_) {
      ErrorCode(future_errc::future_already_retrieved).ThrowAsException();
    }
    retrieved_ = true;
    return core_->get_future(std::move(callback));
  }
  /**
   * @brief Copy a Result into the shared state and make the state ready.
   *
   * @uptrace{SWS_CORE_06355}
   */
  void SetResult(const Result<void, E>& result) { core_->SetResult(result); }

  /**
   * @brief Move a Result into the shared state and make the state ready
   *
   * @uptrace{SWS_CORE_06356}
   */
  void SetResult(Result<void, E>&& result) { core_->SetResult(result); }

  /**
   * @brief Move an error into the shared state and make the state ready.
   *
   * @uptrace{SWS_CORE_06353}
   */
  void SetError(E&& err) {
    R r = R::FromError(std::move(err));
    core_->SetResult(r);
  }

  /**
   * @brief Copy an error into the shared state and make the state ready
   *
   * @uptrace{SWS_CORE_06354}
   */
  void SetError(const E& err) {
    R r = R::FromError(err);
    core_->SetResult(r);
  }

  /**
   * @brief Make the shared state ready
   *
   * @uptrace{SWS_CORE_06345}
   */
  void set_value() { core_->SetResult(R::FromValue()); }

  bool FullFilled() {
    if (core_) {
      return core_->is_ready();
    }
    return true;
  }

private:
  /**
   * @brief use for free core_
   */
  void detach() {
    if (core_) {
      if (!retrieved_) {
        core_->promise_detach();
      } else {
        core_->detach_one();
      }
      core_ = nullptr;
    }
  }

private:
  bool retrieved_;
  Core<void, E>* core_;
};

}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_PROMISE_HPP_

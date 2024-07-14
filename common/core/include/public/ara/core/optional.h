

#ifndef TUSIMPLEAP_ARA_CORE_OPTIONAL_H_
#define TUSIMPLEAP_ARA_CORE_OPTIONAL_H_

#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>
#include <cassert>

#include "ara/core/exception.h"
#include "ara/core/utility.h"
#include "ara/core/internal/enable_special_members.h"

namespace ara {
namespace core {

// optional for object types
template <typename T>
class Optional;

class bad_optional_access final : public std::exception {
public:
  bad_optional_access() = default;
  ~bad_optional_access() final = default;

  const char* what() const noexcept final { return "bad optional access"; }
};

// Disengaged state indicator
class nullopt_t final {
public:
  enum class Construct : std::uint8_t { Token };
  explicit constexpr nullopt_t(Construct) noexcept {};
};
constexpr nullopt_t nullopt {nullopt_t::Construct::Token};

template <typename T>
constexpr bool is_optional = false;
template <typename T>
constexpr bool is_optional<Optional<T>> = true;

template <typename U, typename T>
constexpr bool EnableIfConvertible = std::is_convertible<U, T>::value;
template <typename T>
constexpr bool EnableIfCopyConstructible = std::is_copy_constructible<T>::value;
template <typename T>
constexpr bool EnableIfMoveConstructible = std::is_move_constructible<T>::value;
template <typename T>
using EnableIfNotOptional =
    typename std::enable_if<!is_optional<typename std::decay<T>::type>>::type;
template <typename T>
using EnableIfLValueReference =
    typename std::enable_if<std::is_lvalue_reference<T>::value>::type;
template <typename T>
using EnableIfNotLValueReference =
    typename std::enable_if<!std::is_lvalue_reference<T>::value>::type;
template <typename T, typename... Args>
using EnableIfConstructible =
    typename std::enable_if<std::is_constructible<T, Args...>::value>::type;
template <typename T, typename... Args>
using EnableIfNotConstructible =
    typename std::enable_if<!std::is_constructible<T, Args...>::value>::type;

template <typename Tp>
struct OptionalPayloadBase {
  using StoredType = std::remove_const_t<Tp>;

  OptionalPayloadBase() = default;
  ~OptionalPayloadBase() = default;

  template <typename... Args>
  constexpr OptionalPayloadBase(in_place_t tag, Args&&... args) :
      _payload(tag, std::forward<Args>(args)...), _engaged(true) {}

  template <typename Up, typename... Args>
  constexpr OptionalPayloadBase(std::initializer_list<Up> il, Args&&... args) :
      _payload(il, std::forward<Args>(args)...), _engaged(true) {}

  constexpr OptionalPayloadBase(
      bool engaged, const OptionalPayloadBase& other) {
    if (other._engaged) {
      this->construct(other.get());
    }
  }

  constexpr OptionalPayloadBase(bool engaged, OptionalPayloadBase&& other) {
    if (other._engaged) {
      this->construct(std::move(other.get()));
    }
  }

  OptionalPayloadBase(const OptionalPayloadBase&) = default;

  OptionalPayloadBase(OptionalPayloadBase&&) = default;

  OptionalPayloadBase& operator=(const OptionalPayloadBase&) = default;

  OptionalPayloadBase& operator=(OptionalPayloadBase&&) = default;

  constexpr void copy_assign(const OptionalPayloadBase& other) {
    if (this->_engaged && other._engaged) {
      this->get() = other.get();
    } else {
      if (other._engaged) {
        this->construct(other.get());
      } else {
        this->reset();
      }
    }
  }

  constexpr void move_assign(OptionalPayloadBase&& other) noexcept(
      std::is_nothrow_move_constructible<Tp>::value&&
          std::is_nothrow_move_assignable<Tp>::value) {
    if (this->_engaged && other._engaged) {
      this->get() = std::move(other.get());
    } else {
      if (other._engaged) {
        this->construct(std::move(other.get()));
      } else {
        this->reset();
      }
    }
  }

  struct EmptyByte {};
  template <typename Up, bool = std::is_trivially_destructible<Up>::value>
  union Storage {
    constexpr Storage() noexcept : _empty() {}

    template <typename... Args>
    constexpr Storage(in_place_t, Args&&... args) :
        _value(std::forward<Args>(args)...) {}

    template <typename Vp, typename... Args>
    constexpr Storage(std::initializer_list<Vp> il, Args&&... args) :
        _value(il, std::forward<Args>(args)...) {}
    EmptyByte _empty;
    Up _value;
  };

  template <typename Up>
  union Storage<Up, false> {
    constexpr Storage() noexcept : _empty() {}

    template <typename... Args>
    constexpr Storage(in_place_t, Args&&... args) :
        _value(std::forward<Args>(args)...) {}

    template <typename Vp, typename... Args>
    constexpr Storage(std::initializer_list<Vp> il, Args&&... args) :
        _value(il, std::forward<Args>(args)...) {}

    ~Storage() {}

    EmptyByte _empty;
    Up _value;
  };

  constexpr Tp& get() noexcept { return this->_payload._value; }

  constexpr const Tp& get() const noexcept { return this->_payload._value; }

  constexpr void destroy() noexcept {
    this->_engaged = false;
    this->get().~StoredType();
  }
  template <typename... Args>
  constexpr void construct(Args&&... args) noexcept(
      std::is_nothrow_constructible<StoredType, Args...>::value) {
    // std::_Construct(std::addressof(this->get()),
    // std::forward<Args>(args)...);
    new (std::addressof(this->get())) Tp(std::forward<Args>(args)...);
    this->_engaged = true;
  }
  void reset() noexcept {
    if (this->_engaged) {
      this->destroy();
    }
  }

  constexpr bool is_engaged() const noexcept { return _engaged; }

  Storage<StoredType> _payload;
  bool _engaged = false;
};

template <
    typename Tp,
    bool /*_HasTrivialDestructor*/ = std::is_trivially_destructible<Tp>::value,
    bool /*_HasTrivialCopy */ = std::is_trivially_copy_assignable<Tp>::value&&
        std::is_trivially_copy_constructible<Tp>::value,
    bool /*_HasTrivialMove */ = std::is_trivially_move_assignable<Tp>::value&&
        std::is_trivially_move_constructible<Tp>::value>
struct OptionalPayload;

template <typename Tp>
struct OptionalPayload<Tp, true, true, true> : OptionalPayloadBase<Tp> {
  using OptionalPayloadBase<Tp>::OptionalPayloadBase;

  OptionalPayload() = default;
};

// Payload for optionals with non-trivial copy construction/assignment.
template <typename Tp>
struct OptionalPayload<Tp, true, false, true> : OptionalPayloadBase<Tp> {
  using OptionalPayloadBase<Tp>::OptionalPayloadBase;

  OptionalPayload() = default;
  ~OptionalPayload() = default;
  OptionalPayload(const OptionalPayload&) = default;
  OptionalPayload(OptionalPayload&&) = default;
  OptionalPayload& operator=(OptionalPayload&&) = default;

  constexpr OptionalPayload& operator=(const OptionalPayload& other) {
    this->copy_assign(other);
    return *this;
  }
};

// Payload for optionals with non-trivial move construction/assignment.
template <typename Tp>
struct OptionalPayload<Tp, true, true, false> : OptionalPayloadBase<Tp> {
  using OptionalPayloadBase<Tp>::OptionalPayloadBase;

  OptionalPayload() = default;
  ~OptionalPayload() = default;
  OptionalPayload(const OptionalPayload&) = default;
  OptionalPayload(OptionalPayload&&) = default;
  OptionalPayload& operator=(const OptionalPayload&) = default;
  constexpr OptionalPayload& operator=(OptionalPayload&& other) noexcept(
      std::is_nothrow_move_constructible<Tp>::value&&
          std::is_nothrow_move_assignable<Tp>::value) {
    this->move_assign(std::move(other));
    return *this;
  }
};

// Payload for optionals with non-trivial copy and move assignment.
template <typename Tp>
struct OptionalPayload<Tp, true, false, false> : OptionalPayloadBase<Tp> {
  using OptionalPayloadBase<Tp>::OptionalPayloadBase;

  OptionalPayload() = default;
  ~OptionalPayload() = default;
  OptionalPayload(const OptionalPayload&) = default;
  OptionalPayload(OptionalPayload&&) = default;

  // Non-trivial copy assignment.
  constexpr OptionalPayload& operator=(const OptionalPayload& other) {
    this->copy_assign(other);
    return *this;
  }

  // Non-trivial move assignment.
  constexpr OptionalPayload& operator=(OptionalPayload&& other) noexcept(
      std::is_nothrow_move_constructible<Tp>::value&&
          std::is_nothrow_move_assignable<Tp>::value) {
    this->move_assign(std::move(other));
    return *this;
  }
};

template <typename Tp, bool Copy, bool Move>
struct OptionalPayload<Tp, false, Copy, Move>
    : OptionalPayload<Tp, true, false, false> {
  using OptionalPayload<Tp, true, false, false>::OptionalPayload;
  OptionalPayload() = default;
  OptionalPayload(const OptionalPayload&) = default;
  OptionalPayload(OptionalPayload&&) = default;
  OptionalPayload& operator=(const OptionalPayload&) = default;
  OptionalPayload& operator=(OptionalPayload&&) = default;
  ~OptionalPayload() { this->reset(); }
};

template <typename Tp, typename Dp>
struct OptionalBaseImpl {
  using StoredType = std::remove_const_t<Tp>;

  template <typename... Args>
  constexpr void construct(Args&&... args) noexcept(
      std::is_nothrow_constructible<StoredType, Args...>::value) {
    static_cast<Dp*>(this)->_payload.construct(std::forward<Args>(args)...);
  }

  constexpr void destruct() noexcept {
    static_cast<Dp*>(this)->_payload.destroy();
  }

  constexpr void reset() noexcept { static_cast<Dp*>(this)->_payload.reset(); }

  constexpr bool is_engaged() const noexcept {
    return static_cast<const Dp*>(this)->_payload._engaged;
  }

  constexpr Tp& get() noexcept {
    assert(this->is_engaged());
    return static_cast<Dp*>(this)->_payload.get();
  }

  constexpr const Tp& get() const noexcept {
    assert(this->is_engaged());
    return static_cast<const Dp*>(this)->_payload.get();
  }
};

template <
    typename Tp,
    bool = std::is_trivially_copy_constructible<Tp>::value,
    bool = std::is_trivially_move_constructible<Tp>::value>
struct OptionalBase : OptionalBaseImpl<Tp, OptionalBase<Tp>> {
  constexpr OptionalBase() = default;

  template <
      typename... Args,
      std::enable_if_t<std::is_constructible<Tp, Args...>::value, bool> = false>
  constexpr explicit OptionalBase(in_place_t, Args&&... args) :
      _payload(in_place, std::forward<Args>(args)...) {}

  template <
      typename Up,
      typename... Args,
      std::enable_if_t<
          std::is_constructible<Tp, std::initializer_list<Up>&, Args...>::value,
          bool> = false>
  constexpr explicit OptionalBase(
      in_place_t, std::initializer_list<Up> il, Args&&... args) :
      _payload(in_place, il, std::forward<Args>(args)...) {}

  constexpr OptionalBase(const OptionalBase& other) :
      _payload(other._payload._engaged, other._payload) {}

  constexpr OptionalBase(OptionalBase&& other) noexcept(
      std::is_nothrow_move_constructible<Tp>::value) :
      _payload(other._payload._engaged, std::move(other._payload)) {}

  OptionalBase& operator=(const OptionalBase&) = default;
  OptionalBase& operator=(OptionalBase&&) = default;

  OptionalPayload<Tp> _payload;
};

template <typename Tp>
struct OptionalBase<Tp, false, true> : OptionalBaseImpl<Tp, OptionalBase<Tp>> {
  // Constructors for disengaged optionals.
  constexpr OptionalBase() = default;

  // Constructors for engaged optionals.
  template <
      typename... Args,
      std::enable_if_t<std::is_constructible<Tp, Args...>::value, bool> = false>
  constexpr explicit OptionalBase(in_place_t, Args&&... args) :
      _payload(in_place, std::forward<Args>(args)...) {}

  template <
      typename Up,
      typename... Args,
      std::enable_if_t<
          std::is_constructible<Tp, std::initializer_list<Up>&, Args...>::value,
          bool> = false>
  constexpr explicit OptionalBase(
      in_place_t, std::initializer_list<Up> il, Args... args) :
      _payload(in_place, il, std::forward<Args>(args)...) {}

  // Copy and move constructors.
  constexpr OptionalBase(const OptionalBase& other) :
      _payload(other._payload.engaged, other._payload) {}

  constexpr OptionalBase(OptionalBase&& other) = default;

  // Assignment operators.
  OptionalBase& operator=(const OptionalBase&) = default;
  OptionalBase& operator=(OptionalBase&&) = default;

  OptionalPayload<Tp> _payload;
};

template <typename Tp>
struct OptionalBase<Tp, true, false> : OptionalBaseImpl<Tp, OptionalBase<Tp>> {
  // Constructors for disengaged optionals.
  constexpr OptionalBase() = default;

  // Constructors for engaged optionals.
  template <
      typename... Args,
      std::enable_if_t<std::is_constructible<Tp, Args...>::value, bool> = false>
  constexpr explicit OptionalBase(in_place_t, Args&&... args) :
      _payload(in_place, std::forward<Args>(args)...) {}

  template <
      typename Up,
      typename... Args,
      std::enable_if_t<
          std::is_constructible<Tp, std::initializer_list<Up>&, Args...>::value,
          bool> = false>
  constexpr explicit OptionalBase(
      in_place_t, std::initializer_list<Up> il, Args&&... args) :
      _payload(in_place, il, std::forward<Args>(args)...) {}

  // Copy and move constructors.
  constexpr OptionalBase(const OptionalBase& other) = default;

  constexpr OptionalBase(OptionalBase&& other) noexcept(
      std::is_nothrow_move_constructible<Tp>::value) :
      _payload(other._payload.engaged, std::move(other._payload)) {}

  // Assignment operators.
  OptionalBase& operator=(const OptionalBase&) = default;
  OptionalBase& operator=(OptionalBase&&) = default;

  OptionalPayload<Tp> _payload;
};

template <typename Tp>
struct OptionalBase<Tp, true, true> : OptionalBaseImpl<Tp, OptionalBase<Tp>> {
  // Constructors for disengaged optionals.
  constexpr OptionalBase() = default;

  // Constructors for engaged optionals.
  template <
      typename... Args,
      std::enable_if_t<std::is_constructible<Tp, Args...>::value, bool> = false>
  constexpr explicit OptionalBase(in_place_t, Args&&... args) :
      _payload(in_place, std::forward<Args>(args)...) {}

  template <
      typename Up,
      typename... Args,
      std::enable_if_t<
          std::is_constructible<Tp, std::initializer_list<Up>&, Args...>::value,
          bool> = false>
  constexpr explicit OptionalBase(
      in_place_t, std::initializer_list<Up> il, Args&&... args) :
      _payload(in_place, il, std::forward<Args>(args)...) {}

  // Copy and move constructors.
  constexpr OptionalBase(const OptionalBase& other) = default;
  constexpr OptionalBase(OptionalBase&& other) = default;

  // Assignment operators.
  OptionalBase& operator=(const OptionalBase&) = default;
  OptionalBase& operator=(OptionalBase&&) = default;

  OptionalPayload<Tp> _payload;
};

/**
 * @brief A container that contains key-value pairs with unique keys
 *
 * @uptrace{SWS_CORE_01033}
 */

template <typename T>
class Optional final : private OptionalBase<T>,
                 private internal::EnableCopyMove<
                     // Copy constructor.
                     std::is_copy_constructible<T>::value,
                     // Copy assignment.
                     (std::is_copy_constructible<T>::value && std::is_copy_assignable<T>::value),
                     // Move constructor.
                     std::is_move_constructible<T>::value,
                     // Move assignment.
                     (std::is_move_constructible<T>::value && std::is_move_assignable<T>::value),
                     // Unique tag type.
                     Optional<T>> {
  static_assert(!std::is_same<std::remove_cv_t<T>, nullopt_t>::value);
  static_assert(!std::is_same<std::remove_cv_t<T>, in_place_t>::value);
  static_assert(!std::is_reference<T>::value);
private:
  using Base = OptionalBase<T>;
public:
  // X.Y.4.1, constructors
  /**
   * @brief Defaulconst Tructor
   *
   * @uptrace{SWS_CORE_01034}
   */
  constexpr Optional() noexcept {};
  constexpr Optional(nullopt_t) noexcept {}

  //constexpr Optional(const Optional<T>& other) : val(other.val) {}
  //constexpr Optional(Optional<T>&& other) noexcept(
  //    std::is_nothrow_constructible<T>::value) :
  //    val(std::move(other.val)) {};

  template <typename U = T, typename = EnableIfNotOptional<U>>
  constexpr Optional(U&& u) : Base(in_place, std::forward<U>(u)) {}

  /**
   * @uptrace{SWS_CORE_01036}
   */
  template <typename U>
  Optional(const Optional<U>& other) noexcept(
      std::is_nothrow_constructible<T, const U&>::value) {
    if (other) {
      emplace(*other);
    }
  }
  /**
   * @uptrace{SWS_CORE_01035}
   */
  template <typename U>
  constexpr Optional(Optional<U>&& other) noexcept(
      std::is_nothrow_constructible<T, U>::value) {
    if (other) {
      emplace(std::move(*other));
    }
  }

  template <typename... Args, typename = EnableIfConstructible<T, Args...>>
  constexpr explicit Optional(in_place_t, Args&&... args) noexcept(
      std::is_nothrow_constructible<T, Args...>::value) :
      Base(in_place, std::forward<Args>(args)...) {}

  template <
      typename U,
      typename... Args,
      typename = EnableIfConstructible<T, std::initializer_list<U>&, Args...>>
  constexpr explicit Optional(
      in_place_t,
      std::initializer_list<U> il,
      Args&&... args) noexcept(std::
                                   is_nothrow_constructible<
                                       T,
                                       std::initializer_list<U>&,
                                       Args...>::value) :
      Base(in_place, il, std::forward<Args>(args)...) {}
  // X.Y.4.2, destructor
  /**
   * @uptrace{SWS_CORE_01037}
   */

//private:
//  constexpr T& get() noexcept { return this->val._value; }
//
//  constexpr const T& get() const noexcept { return this->val._value; }
//
//  constexpr void destroy() noexcept {
//    this->is_engaged = false;
//    this->get().~StoredType();
//  }
//  template <typename... Args>
//  constexpr void construct(Args&&... args) noexcept(
//      std::is_nothrow_constructible<StoredType, Args...>::value) {
//    // std::_Construct(std::addressof(this->get()),
//    // std::forward<Args>(args)...);
//    new (std::addressof(this->get())) T(std::forward<Args>(args)...);
//    this->is_engaged = true;
//  }
//
public:
  // X.Y.4.3, assignment

  //constexpr Optional<T>& operator=(const Optional<T>& other) {
  //  this->reset();
  //  engaged = other.engaged;
  //  val = other.val;
  //  return *this;
  //}

  //constexpr Optional<T>& operator=(Optional<T>&& other) noexcept(
  //    std::is_nothrow_constructible<Optional<T>>::value) {
  //  this->reset();
  //  engaged = std::move(other.engaged);
  //  val = std::move(other.val);
  //  return *this;
  //}

  Optional<T>& operator=(nullopt_t) noexcept {
    this->reset();
    return *this;
  }
  /**
   * @uptrace{SWS_CORE_01038}
   */

  template <typename U = T>
  typename std::enable_if<!std::is_same<U, Optional<T>&>::value, Optional>::
      type&
      operator=(U&& u) noexcept(std::is_nothrow_constructible<T, U>::value&&
                                    std::is_nothrow_assignable<T&, U>::value) {
    if (this->is_engaged()) {
      this->get() = std::forward<U>(u);
    } else {
      this->construct(std::forward<U>(u));
    }
    return *this;
  }
  /**
   * @uptrace{SWS_CORE_01039}
   */
  template <typename U>
  constexpr typename std::
      enable_if<!std::is_same<U, Optional<T>&>::value, Optional>::type&
      operator=(const Optional<U>& u) noexcept(
          std::is_nothrow_constructible<T, const U&>::value&&
              std::is_nothrow_assignable<T&, const U&>::value) {
    if (u) {
      if (this->is_engaged()) {
        this->get() = *u;
      } else {
        this->construct(*u);
      }
    } else {
      this->reset();
    }
    return *this;
  }
  template <typename U>
  typename std::enable_if<!std::is_same<U, Optional<T>&>::value, Optional>::
      type&
      operator=(Optional<U>&& u) noexcept(
          std::is_nothrow_constructible<T, U>::value&&
              std::is_nothrow_assignable<T&, U>::value) {
    if (u) {
      if (this->is_engaged()) {
        this->get() = std::move(*u);
      } else {
        this->construct(std::move(*u));
      }
    } else {
      this->reset();
    }
    return *this;
  }

  template <typename... Args>
  typename std::enable_if<std::is_constructible<T, Args...>::value, T&>::type&
  emplace(Args&&... args) noexcept(
      std::is_nothrow_constructible<T, Args...>::value) {
    this->reset();
    this->construct(std::forward<Args>(args)...);
    return this->get();
  }
  template <typename U, typename... Args>
  typename std::enable_if<
      std::is_constructible<T, std::initializer_list<U>&, Args...>::value,
      T&>::type&
  emplace(std::initializer_list<U> il, Args&&... args) noexcept(
      std::is_nothrow_constructible<T, std::initializer_list<U>&, Args...>::
          value) {
    this->reset();
    this->construct(il, std::forward<Args>(args)...);
    return this->get();
  }
  /**
   * @uptrace{SWS_CORE_01043}
   */
  //void reset() noexcept {
  //  if (this->is_engaged()) {
  //    this->destroy();
  //  }
  //}

  // X.Y.4.4, swap
  template <typename T_ = T>
  void swap(Optional<T>& rhs) noexcept(
      std::is_nothrow_move_constructible<T>::value&& noexcept(
          std::swap(std::declval<T&>(), std::declval<T&>()))) {
    static_assert(EnableIfMoveConstructible<T_>);
    if (this->is_engaged() && rhs.is_engaged()) {
      std::swap(this->get(), rhs.get());
    } else if (this->is_engaged()) {
      rhs.construct(std::move(this->get()));
      this->destruct();
    } else if (rhs.is_engaged()) {
      this->construct(std::move(rhs.get()));
      rhs.destruct();
    }
  }

  // X.Y.4.5, observers
  constexpr const T* operator->() const noexcept {
    return std::addressof(this->get());
  }
  constexpr T* operator->() { return std::addressof(this->get()); }
  constexpr const T& operator*() const& noexcept { return this->get(); }
  constexpr T& operator*() & { return this->get(); }
  constexpr const T&& operator*() const&& noexcept {
    return std::move(this->get());
  }
  constexpr T&& operator*() && { return std::move(this->get()); }
  /**
   * @uptrace{SWS_CORE_01041}
   */
  constexpr bool has_value() const noexcept { return this->is_engaged(); }
  /**
   * @uptrace{SWS_CORE_01042}
   */
  constexpr explicit operator bool() const noexcept { return this->is_engaged(); }

  constexpr T& value() & {
    if (this->is_engaged()) {
      return this->get();
    }
    throw bad_optional_access();
  }

  constexpr const T& value() const& {
    if (this->is_engaged()) {
      return this->get();
    }
    throw bad_optional_access();
  }

  constexpr T&& value() && {
    if (this->is_engaged()) {
      return std::move(this->get());
    }
    throw bad_optional_access();
  }

  constexpr const T&& value() const&& {
    if (this->is_engaged()) {
      return std::move(this->get());
    }
    throw bad_optional_access();
  }

  template <typename U>
  constexpr T value_or(U&& v) const& {
    static_assert(EnableIfCopyConstructible<T>);
    static_assert(EnableIfConvertible<U&&, T>);
    return this->is_engaged() ? this->get() : static_cast<T>(std::forward<U>(v));
  }
  template <typename U>
  constexpr T value_or(U&& v) && {
    static_assert(EnableIfMoveConstructible<T>);
    static_assert(EnableIfConvertible<U&&, T>);
    return this->is_engaged() ? this->get() : static_cast<T>(std::forward<U>(v));
  }

  void reset() noexcept { this->Base::reset(); }
};

template <typename T>
using optional_relop_t =
    typename std::enable_if<std::is_convertible<T, bool>::value, bool>::type;

template <typename T, typename U>
using optional_eq_t = optional_relop_t<decltype(
    std::declval<const T&>() == std::declval<const U&>())>;

template <typename T, typename U>
using optional_ne_t = optional_relop_t<decltype(
    std::declval<const T&>() != std::declval<const U&>())>;

template <typename T, typename U>
using optional_lt_t = optional_relop_t<decltype(
    std::declval<const T&>() < std::declval<const U&>())>;

template <typename T, typename U>
using optional_gt_t = optional_relop_t<decltype(
    std::declval<const T&>() > std::declval<const U&>())>;

template <typename T, typename U>
using optional_le_t = optional_relop_t<decltype(
    std::declval<const T&>() <= std::declval<const U&>())>;

template <typename T, typename U>
using optional_ge_t = optional_relop_t<decltype(
    std::declval<const T&>() >= std::declval<const U&>())>;

// Relational operators
template <typename T, typename U>
constexpr auto operator==(const Optional<T>& x, const Optional<U>& y)
    -> optional_eq_t<T, U> {
  return (
      (static_cast<bool>(x) == static_cast<bool>(y)) &&
      ((!x) || ((*x) == (*y))));
}
template <typename T, typename U>
constexpr auto operator!=(const Optional<T>& x, const Optional<U>& y)
    -> optional_ne_t<T, U> {
  return (
      (static_cast<bool>(x) == static_cast<bool>(y)) &&
      ((!x) || ((*x) != (*y))));
}
template <typename T, typename U>
constexpr auto operator<(const Optional<T>& x, const Optional<U>& y)
    -> optional_lt_t<T, U> {
  return (static_cast<bool>(y) && ((!x) || ((*x) < (*y))));
}
template <typename T, typename U>
constexpr auto operator>(const Optional<T>& x, const Optional<U>& y)
    -> optional_gt_t<T, U> {
  return (static_cast<bool>(x) && ((!y) || ((*x) > (*y))));
}
template <typename T, typename U>
constexpr auto operator<=(const Optional<T>& x, const Optional<U>& y)
    -> optional_le_t<T, U> {
  return (static_cast<bool>(x) && ((!y) || ((*x) <= (*y))));
}
template <typename T, typename U>
constexpr auto operator>=(const Optional<T>& x, const Optional<U>& y)
    -> optional_ge_t<T, U> {
  return (static_cast<bool>(y) && ((!x) || ((*x) >= (*y))));
}

// Comparison with nullopt
template <typename T>
constexpr bool operator==(const Optional<T>& x, nullopt_t) noexcept {
  return !x;
}
template <typename T>
constexpr bool operator==(nullopt_t, const Optional<T>& x) noexcept {
  return !x;
}
template <typename T>
constexpr bool operator!=(const Optional<T>& x, nullopt_t) noexcept {
  return static_cast<bool>(x);
}
template <typename T>
constexpr bool operator!=(nullopt_t, const Optional<T>& x) noexcept {
  return static_cast<bool>(x);
}
template <typename T>
constexpr bool operator<(const Optional<T>&, nullopt_t) noexcept {
  return false;
}
template <typename T>
constexpr bool operator<(nullopt_t, const Optional<T>& x) noexcept {
  return static_cast<bool>(x);
}
template <typename T>
constexpr bool operator<=(const Optional<T>& x, nullopt_t) noexcept {
  return !x;
}
template <typename T>
constexpr bool operator<=(nullopt_t, const Optional<T>&) noexcept {
  return true;
}
template <typename T>
constexpr bool operator>(const Optional<T>& x, nullopt_t) noexcept {
  return static_cast<bool>(x);
}
template <typename T>
constexpr bool operator>(nullopt_t, const Optional<T>&) noexcept {
  return false;
}
template <typename T>
constexpr bool operator>=(const Optional<T>&, nullopt_t) noexcept {
  return true;
}
template <typename T>
constexpr bool operator>=(nullopt_t, const Optional<T>& x) noexcept {
  return !x;
}

// Comparison with T
template <typename T, typename U>
constexpr auto operator==(const Optional<T>& x, const U& v)
    -> optional_eq_t<T, U> {
  return (static_cast<bool>(x) && ((*x) == v));
}
template <typename T, typename U>
constexpr auto operator==(const U& v, const Optional<T>& x)
    -> optional_eq_t<U, T> {
  return (static_cast<bool>(x) && (v == (*x)));
}
template <typename T, typename U>
constexpr auto operator!=(const Optional<T>& x, const U& v)
    -> optional_ne_t<T, U> {
  return !(x == v);
}
template <typename T, typename U>
constexpr auto operator!=(const U& v, const Optional<T>& x)
    -> optional_ne_t<U, T> {
  return !(v == x);
}
template <typename T, typename U>
constexpr auto operator<(const Optional<T>& x, const U& v)
    -> optional_lt_t<T, U> {
  return (static_cast<bool>(x) && ((*x) < v));
}
template <typename T, typename U>
constexpr auto operator<(const U& v, const Optional<T>& x)
    -> optional_lt_t<U, T> {
  return (static_cast<bool>(x) && (v < (*x)));
}
template <typename T, typename U>
constexpr auto operator<=(const Optional<T>& x, const U& v)
    -> optional_le_t<T, U> {
  return (static_cast<bool>(x) && ((*x) <= v));
}
template <typename T, typename U>
constexpr auto operator<=(const U& v, const Optional<T>& x)
    -> optional_le_t<U, T> {
  return (static_cast<bool>(x) && (v <= (*x)));
}
template <typename T, typename U>
constexpr auto operator>(const Optional<T>& x, const U& v)
    -> optional_gt_t<T, U> {
  return (static_cast<bool>(x) && ((*x) > v));
}
template <typename T, typename U>
constexpr auto operator>(const U& v, const Optional<T>& x)
    -> optional_gt_t<U, T> {
  return (static_cast<bool>(x) && (v > (*x)));
}
template <typename T, typename U>
constexpr auto operator>=(const Optional<T>& x, const U& v)
    -> optional_ge_t<T, U> {
  return (static_cast<bool>(x) && ((*x) >= v));
}
template <typename T, typename U>
constexpr auto operator>=(const U& v, const Optional<T>& x)
    -> optional_ge_t<U, T> {
  return (static_cast<bool>(x) && (v >= (*x)));
}

// Specialized algorithms
template <typename T>
void swap(Optional<T>& x, Optional<T>& y) noexcept(noexcept(x.swap(y))) {
  x.swap(y);
}

template <typename T>
typename std::enable_if<
    std::is_constructible<typename std::decay<T>::type, T>::value,
    Optional<typename std::decay<T>::type>>::type
make_optional(T&& t) noexcept(
    std::is_nothrow_constructible<typename std::decay<T>::type, T>::value) {
  return Optional<typename std::decay<T>::type>(std::forward<T>(t));
}
template <typename T, typename... Args>
typename std::enable_if<std::is_constructible<T, Args...>::value, Optional<T>>::
    type
    make_optional(Args&&... args) noexcept(
        std::is_nothrow_constructible<T, Args...>::value) {
  return Optional<T> {in_place, std::forward<Args>(args)...};
}
template <typename T, typename U, typename... Args>
typename std::enable_if<
    std::is_constructible<T, std::initializer_list<U>&, Args...>::value,
    Optional<T>>::type
make_optional(std::initializer_list<U> il, Args&&... args) noexcept(
    std::is_nothrow_constructible<T, std::initializer_list<U>&, Args...>::
        value) {
  return Optional<T> {in_place, il, std::forward<Args>(args)...};
}

}  // namespace core
}  // namespace ara

namespace std {
// hash support
template <typename T>
class hash<ara::core::Optional<T>> final {
public:
  std::size_t operator()(const ara::core::Optional<T>& o) const noexcept {
    if (o) {
      return std::hash<T>()(*o);
    }
    return 0;
  }
};
}  // namespace std

#endif  // TUSIMPLEAP_ARA_CORE_OPTIONAL_H_

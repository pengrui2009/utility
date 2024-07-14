/// @file
/// @brief Interface to ara::core::function

#ifndef TUSIMPLEAP_ARA_CORE_FUNCTIONAL_H_
#define TUSIMPLEAP_ARA_CORE_FUNCTIONAL_H_

#include <functional>
#include <type_traits>
#include "ara/core/utility.h"

namespace ara {
namespace core {

template <typename F, typename... Args>
inline auto invoke(F&& f, Args&&... args) -> decltype(std::forward<F>(f)(std::forward<Args>(args)...)) {
  return std::forward<F>(f)(std::forward<Args>(args)...);
}

template <typename M, typename C, typename... Args>
inline auto invoke(M(C::*d), Args&&... args) -> decltype(std::mem_fn(d)(std::forward<Args>(args)...)) {
  return std::mem_fn(d)(std::forward<Args>(args)...);
}

class FunctionStorage final {
public:
  void* addr() noexcept { return &bytes[0]; }
  class Delegate final {
  public:
    void (FunctionStorage::*tv)();
    FunctionStorage* obj;
  };
  union {
    void* pointer;
    alignas(Delegate) alignas(void (*)()) unsigned char bytes[sizeof(Delegate)];
  };
};

template <typename T>
static constexpr bool stored_locally =
    sizeof(T) <= sizeof(FunctionStorage) &&
    alignof(T) <= alignof(FunctionStorage) && std::is_nothrow_move_constructible<T>::value;

template <typename FuncType>
bool function_manager(FunctionStorage* target, FunctionStorage* src) noexcept {
  if (stored_locally<FuncType>) {
    if (src != nullptr) {
      FuncType* rval = static_cast<FuncType*>(src->addr());
      new (target->addr()) FuncType(std::move(*rval));
      rval->~FuncType();
    } else {
      static_cast<FuncType*>(target->addr())->~FuncType();
    }
  } else {
    if (src != nullptr) {
      target->pointer = src->pointer;
    } else {
      delete static_cast<FuncType*>(target->pointer);
    }
  }
  return true;
}

inline bool empty_manager(FunctionStorage* lhs, FunctionStorage* rhs) {
  return false;
}

template <typename... Signature>
class Function;

template <typename R, typename... ArgTypes>
class Function<R(ArgTypes...)> final {
public:
  using ReturnType = R;
  using Manager = bool (*)(FunctionStorage*, FunctionStorage*);
  Function() noexcept : _manager(empty_manager) {}
  Function(std::nullptr_t) noexcept : _manager(empty_manager) {}
  Function(const Function&) = delete;
  Function& operator=(const Function&) = delete;

  template <typename FuncType, typename... Args>
  void init(Args&&... args) noexcept {
    if (stored_locally<FuncType>) {
      new (_storage.addr()) FuncType(std::forward<Args>(args)...);
    } else {
      _storage.pointer = new FuncType(std::forward<Args>(args)...);
    }
    _manager = &function_manager<FuncType>;
  }

  template <typename FuncType, typename DFuncType = std::decay_t<FuncType>>
  Function(FuncType&& func) noexcept {
    init<DFuncType>(std::forward<FuncType>(func));
    _invoke = &InvokeHelper<DFuncType>;
  }

  template <typename FuncType, typename... Args>
  explicit Function(in_place_type_t<FuncType>, Args&&... args) noexcept : _invoke(&InvokeHelper<FuncType>) {
    static_assert(std::is_same<std::decay_t<FuncType>, FuncType>::value);
    init<FuncType>(std::forward<Args>(args)...);
  }

  template <typename FuncType, typename T, typename... Args>
  explicit Function(in_place_type_t<FuncType>, std::initializer_list<T> il, Args&&... args) noexcept :
      _invoke(&InvokeHelper<FuncType>) {
    static_assert(std::is_same<std::decay_t<FuncType>, FuncType>::value);
    init<FuncType>(il, std::forward<Args>(args)...);
  }

  Function(Function&& other) noexcept {
    _manager = std::exchange(other._manager, empty_manager);
    _manager(&_storage, &other._storage);
    _invoke = std::exchange(other._invoke, nullptr);
  }

  Function& operator=(Function&& other) noexcept {
    _manager(&_storage, nullptr);
    _manager = std::exchange(other._manager, empty_manager);
    _manager(&_storage, &other._storage);
    _invoke = std::exchange(other._invoke, nullptr);
    return *this;
  }

  Function& operator=(std::nullptr_t) noexcept {
    _manager(&_storage, nullptr);
    _manager = empty_manager;
    _invoke = nullptr;
    return *this;
  }

  template <typename FuncType>
  Function& operator=(FuncType&& func) noexcept {
    Function(std::forward<FuncType>(func)).swap(*this);
    return *this;
  }

  ~Function() { _manager(&_storage, nullptr); }

  explicit operator bool() const noexcept { return _invoke != nullptr; }

  ReturnType operator()(ArgTypes... args) { return _invoke(this, std::forward<ArgTypes>(args)...); }

  void swap(Function& other) noexcept {
    FunctionStorage s;
    other._manager(&s, &other._storage);
    _manager(&other._storage, &_storage);
    other._manager(&_storage, &s);
    std::swap(_manager, other._manager);
    std::swap(_invoke, other._invoke);
  }

  friend void swap(Function& lhs, Function& ths) noexcept { lhs.swap(ths); }

private:
  template <typename T>
  using param_t = std::conditional_t<std::is_trivially_copyable<T>::value && sizeof(T) <= sizeof(long), T, T&&>;

  using Invoke = ReturnType (*)(Function*, param_t<ArgTypes>...);

  template <typename T, typename Self>
  static T* access(Self* self) noexcept {
    if (stored_locally<std::remove_const_t<T>>) {
      return static_cast<T*>(self->_storage.addr());
    } else {
      return static_cast<T*>(self->_storage.pointer);
    }
  }

  template <typename Func>
  static ReturnType InvokeHelper(Function* self, param_t<ArgTypes>... args) noexcept {
    return static_cast<ReturnType>(invoke(*static_cast<Func*>(access<Func>(self)), static_cast<ArgTypes&&>(args)...));
  }

  mutable FunctionStorage _storage;
  Manager _manager;
  Invoke _invoke = nullptr;
};

}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_FUNCTIONAL_H_

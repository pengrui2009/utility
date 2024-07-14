#ifndef AEG_ADAPTIVE_AUTOSAR_MODULES_ADAPTIVE_AUTOSAR_ARA_API_CORE_INCLUDE_PUBLIC_ARA_CORE_NV_ARRAY_H_
#define AEG_ADAPTIVE_AUTOSAR_MODULES_ADAPTIVE_AUTOSAR_ARA_API_CORE_INCLUDE_PUBLIC_ARA_CORE_NV_ARRAY_H_

#include <stdexcept>
#include <utility>
#include <mutex>
#include <string.h>
#include <algorithm>
#include <atomic>

#include "ara/core/core_error_domain.h"

namespace ara {
namespace core {

template <typename T>
class RefCount {
  public:
  typedef std::atomic<T> value_type;

  RefCount():cntrl_ptr{new value_type()}{
    cntrl_ptr->store(1);
  }
  RefCount(const RefCount<T>& rhs){
    *this = rhs;
  }
  RefCount(RefCount<T>&& rhs){
    *this = std::move(rhs);
  }
  RefCount& operator=(const RefCount<T>& rhs){
    if( this == &rhs )
      return *this;
    this->SubCount();
    this->cntrl_ptr = rhs.cntrl_ptr;
    this->AddCount();
    return *this;
  }
  RefCount& operator=(RefCount<T>&& rhs){
    if( this == &rhs )
      return *this;
    this->SubCount();
    this->cntrl_ptr = rhs.cntrl_ptr;
    rhs.cntrl_ptr = nullptr;
    return *this;
  }

  T UseCount(){
    return cntrl_ptr->load();
  }
  void AddCount(){
    cntrl_ptr->fetch_add(1);
  }
  void SubCount(){
    if( !Release() )
      cntrl_ptr->fetch_sub(1);
  }
  void Reset(){
    SubCount();
    cntrl_ptr = nullptr;
  }
  void Allocate(){
    cntrl_ptr = new value_type();
    cntrl_ptr->store(1);
  }

  ~RefCount(){
    if(cntrl_ptr)
      Reset();
  }

  private:
  value_type* cntrl_ptr{nullptr};

  bool Release(){
    if( cntrl_ptr && cntrl_ptr->load() == 1 ){
      delete cntrl_ptr;
      cntrl_ptr = nullptr;
      return true;
    }
    return false;
  }
};

template <typename T, std::size_t N>
class NvArray {
 public:
  typedef T                                     value_type;
  typedef value_type&                           reference;
  typedef const value_type&                     const_reference;
  typedef value_type*                           iterator;
  typedef const value_type*                     const_iterator;
  typedef value_type*                           pointer;
  typedef const value_type*                     const_pointer;
  typedef std::reverse_iterator<iterator>       reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  NvArray(T *cpu_data_ptr = nullptr, T *gpu_data_ptr = nullptr) 
      : cpu_ptr_(cpu_data_ptr), gpu_ptr_(gpu_data_ptr), length_(N) {}

  NvArray(const NvArray& rhs) {
    *this = rhs;
  }
  NvArray& operator=(const NvArray& rhs) {
    if (this == &rhs) {
      return *this;
    }
    // if (this->gpu_ptr_ !=  nullptr && rhs.gpu_ptr_ == nullptr) {
    //   ErrorCode(CoreErrc::invalid_argument).ThrowAsException();
    // }
    if(this->cpu_ptr_ == rhs.cpu_ptr_ && this->gpu_ptr_ == rhs.gpu_ptr_) {
      this->length_ = rhs.length_;
      this->count_ = rhs.count_;
      return *this;
    }

    if(this->gpu_ptr_ !=  nullptr && rhs.gpu_ptr_ == nullptr){
      if( this->length_ != rhs.length_ ){
        ErrorCode(CoreErrc::invalid_argument).ThrowAsException();
      }

      T *tmp = this->cpu_ptr_;
      Free(tmp);

      memcpy((void*)this->cpu_ptr_, (void*)rhs.data(), rhs.length_*sizeof(T));
    }
    else{
      this->cpu_ptr_ = rhs.cpu_ptr_;
      this->gpu_ptr_ = rhs.gpu_ptr_;
      this->length_ = rhs.length_;
      this->count_ = rhs.count_;
    }

    return *this;
  }

  NvArray(NvArray&& rhs) {
    *this = std::move(rhs);
  }
  NvArray& operator=(NvArray&& rhs) {
    if (this == &rhs) {
      return *this;
    }

    if (this->gpu_ptr_ !=  nullptr && rhs.gpu_ptr_ == nullptr) {
      if( this->length_ != rhs.length_ ){
        ErrorCode(CoreErrc::invalid_argument).ThrowAsException();
      }

      // ErrorCode(CoreErrc::invalid_argument).ThrowAsException();
      T* tmp = this->cpu_ptr_;
      Free(tmp);

      memcpy((void*)this->cpu_ptr_, (void*)rhs.data(), rhs.length_*sizeof(T));

      rhs.cpu_ptr_ = nullptr;
      rhs.length_ = 0;
      rhs.count_.Reset();

      return *this;
    }

    T *tmp = this->cpu_ptr_;
    Free(tmp);

    this->cpu_ptr_ = rhs.cpu_ptr_;
    this->gpu_ptr_ = rhs.gpu_ptr_;
    this->length_ = rhs.length_;
    this->count_ = rhs.count_;
    rhs.cpu_ptr_ = nullptr;
    rhs.gpu_ptr_ = nullptr;
    rhs.length_ = 0;
    rhs.count_.Reset();

    return *this;
  }

  ~NvArray() {
    Free(this->cpu_ptr_);
  }

  T* gpu_ptr() {return gpu_ptr_;}
  const T* gpu_ptr() const {return gpu_ptr_;}
  
  void Init(void *cpu_data_ptr, void *gpu_data_ptr) {
    cpu_ptr_ = static_cast<T*>(cpu_data_ptr);
    new (cpu_ptr_) T[length_];
    gpu_ptr_ = static_cast<T*>(gpu_data_ptr);
  }

  void UpdateCount() {
    count_.Allocate();
  }

  // iterators
  iterator begin() {return iterator(data());}
  const_iterator begin() const {return const_iterator(data());}
  iterator end() {return iterator(data() + length_);}
  const_iterator end() const {return const_iterator(data() + length_);}

  reverse_iterator rbegin() {return reverse_iterator(end());}
  const_reverse_iterator rbegin() const {return const_reverse_iterator(end());}
  reverse_iterator rend() {return reverse_iterator(begin());}
  const_reverse_iterator rend() const {return const_reverse_iterator(begin());}

  const_iterator cbegin() const {return begin();}
  const_iterator cend() const {return end();}
  const_reverse_iterator crbegin() const {return rbegin();}
  const_reverse_iterator crend() const {return rend();}

  void fill(const value_type& value) {
    const_cast<NvArray<T, N>*>(this)->Allocate();
    std::fill_n(data(), size(), value);
  }

  T& at(const std::size_t index) {
    return const_cast<T&>(static_cast<const NvArray<T, N>&>(*this).at(index));
  } 
  const T& at(const std::size_t index) const {
    const_cast<NvArray<T, N>*>(this)->Allocate();
    if (index >= length_) {
      ErrorCode(CoreErrc::invalid_argument).ThrowAsException();
    }
    return cpu_ptr_[index];
  }

  T& operator[](const std::size_t index) {
    return at(index);
  }
  const T& operator[](const std::size_t index) const {
    return at(index);
  }

  T* data() {
    return &(*this)[0];
  }
  const T* data() const {
    return &(*this)[0];
  }

  T& front() {
    return at(0);
  }
  const T& front() const {
    return at(0);
  }

  T& back() {
    return at(length_ - 1);
  }
  const T& back() const {
    return at(length_ - 1);
  }

  bool empty() const {return size() == 0;}
  //return length_，is N
  std::size_t size() const { return length_;}
  //same as size()
  uint64_t max_size() const {return size();}

 private: 
  inline void Allocate() {
    // std::lock_guard<std::mutex> lock(allocate_lock_);
    if (cpu_ptr_ == nullptr) {
      cpu_ptr_ = new T[length_];
    }
  }

  void Free(T *&ptr) {
    if (gpu_ptr_ == nullptr && ptr != nullptr && count_.UseCount() == 1) {
      delete [] ptr;
      ptr = nullptr;
    }
  }

  T *cpu_ptr_{nullptr};
  T *gpu_ptr_{nullptr};
  std::size_t length_;
  RefCount<std::uint8_t> count_{};
  // std::mutex allocate_lock_;
};

} // namespace core
} // namespace ara

#endif  // AEG_ADAPTIVE_AUTOSAR_MODULES_ADAPTIVE_AUTOSAR_ARA_API_CORE_INCLUDE_PUBLIC_ARA_CORE_NV_ARRAY_H_
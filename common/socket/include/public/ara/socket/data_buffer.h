/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2023-01-16 13:33:04
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-03-07 11:36:16
 * @Description:
 *
 * Copyright (c) 2023 by Tusimple, All Rights Reserved.
 */

#pragma once

#include <cstring>
#include <memory>
#include <vector>

namespace ara {
namespace socket {

using DefaultBufferType = uint8_t;
template <typename T>
using default_allocator = std::allocator<T>;

template <typename T, typename _Alloc = default_allocator<T>,
          typename =
              std::enable_if_t<std::is_same<T, DefaultBufferType>::value, void>>
class Buffer {
 private:
  typedef _Alloc allocator_type;

  void* data_ = nullptr;
  std::size_t size_ = 0;
  allocator_type alloc_;
  friend Buffer<DefaultBufferType, default_allocator<DefaultBufferType>>
  make_buffer(std::size_t size);

  Buffer(std::size_t size, const allocator_type& alloc)
      : size_(size), alloc_(alloc) {
    data_ = static_cast<void*>(alloc_.allocate(size));
    // std::cout << "Buffer() this:" << this << " data:" << data_ << std::endl;
  };

 public:

  Buffer(const Buffer& rhs) {
    if (this == &rhs) return;
    if (this->data_ != nullptr) {
      alloc_.deallocate(static_cast<T*>(this->data_), size_);
    }
    this->size_ = rhs.size_;
    this->alloc_ = rhs.alloc_;
    this->data_ = static_cast<void*>(alloc_.allocate(size_));
    std::memcpy(this->data_, rhs.data_, size_);
    // std::cout << "Buffer(const Buffer& rhs) this:" << this << " data:" <<
    // data_
    //           << std::endl;
  };

  Buffer(Buffer&& rhs) {
    if (this == &rhs) return;
    if (data_ != nullptr) {
      alloc_.deallocate(static_cast<T*>(data_), size_);
    }
    data_ = rhs.data_;
    size_ = rhs.size_;
    alloc_ = std::move(rhs.alloc_);
    rhs.data_ = nullptr;
    rhs.size_ = 0;
    // std::cout << "Buffer(Buffer&& rhs) this:" << this << " data:" << data_
    //           << std::endl;
  };

  Buffer& operator=(const Buffer& rhs) {
    if (this == &rhs) return *this;
    if (data_ != nullptr) {
      alloc_.deallocate(static_cast<T*>(data_), size_);
    }
    size_ = rhs.size_;
    alloc_ = rhs.alloc_;
    data_ = static_cast<void*>(alloc_.allocate(size_));
    std::memcpy(this->data_, rhs.data_, size_);
    // std::cout << "Buffer& operator=(const Buffer& rhs) this:" << this
    //           << " data:" << data_ << std::endl;
    return *this;
  };

  Buffer& operator=(Buffer&& rhs) {
    if (this == &rhs) {
      return *this;
    }
    if (data_ != nullptr) {
      alloc_.deallocate(static_cast<T*>(data_), size_);
    }
    data_ = rhs.data_;
    size_ = rhs.size_;
    alloc_ = std::move(rhs.alloc_);
    rhs.data_ = nullptr;
    rhs.size_ = 0;
    // std::cout << "Buffer& operator=(Buffer&& rhs) this:" << this
    //           << " data:" << data_ << std::endl;
    return *this;
  };

  /**
   * @brief Copy data from source data address with this Buffer's size
   *
   * @param src Source data address
   */
  void CopyFrom(const void* src) { std::memcpy(data_, src, size_); }

  /**
   * @brief Resize the buffer. This operation will reallocate memory.
   *
   * @param new_size
   */
  void Resize(std::size_t new_size) {
    if (new_size > size_) {
      auto new_data = static_cast<void*>(alloc_.allocate(new_size));
      if (data_ != nullptr) {
        std::memcpy(new_data, data_, size_);
        // std::cout << "deallocate this:" << this << " data:" << data_
        //           << std::endl;
        alloc_.deallocate(static_cast<T*>(data_), size_);
      }
      data_ = new_data;
    } else if (new_size < size_) {
      auto new_data = static_cast<void*>(alloc_.allocate(new_size));
      if (data_ != nullptr) {
        std::memcpy(new_data, data_, new_size);
        // std::cout << "deallocate this:" << this << " data:" << data_
        //           << std::endl;
        alloc_.deallocate(static_cast<T*>(data_), size_);
      }
      data_ = new_data;
    } else {
    }
    size_ = new_size;
  }

  ~Buffer() {
    // std::cout << "~Buffer() this:" << this << " data:" << data_ << std::endl;
    if (data_ != nullptr) {
      alloc_.deallocate(static_cast<T*>(data_), size_);
    }
  }

  T* Data() { return static_cast<T*>(data_); }

  const T* Data() const { return static_cast<T*>(data_); }

  const std::size_t Size() const { return size_; }
};

/**
 * @brief Get a default allocator:std::allocator
 *
 * @return default_allocator<DefaultBufferType>
 */
inline default_allocator<DefaultBufferType> get_default_alloc() {
  return default_allocator<DefaultBufferType>();
}

/**
 * @brief Make a buffer use a specific allocator
 *
 * @tparam _Alloc
 * @param size Buffer size
 * @param alloc The specific allocator
 * @return Buffer<DefaultBufferType, _Alloc>
 */
template <typename _Alloc>
inline Buffer<DefaultBufferType, _Alloc> make_buffer(std::size_t size,
                                                     const _Alloc& alloc) {
  return Buffer<DefaultBufferType, _Alloc>(size, alloc);
}

/**
 * @brief Make a buffer use std::allocator
 *
 * @param size Buffer size
 * @return Buffer<DefaultBufferType, default_allocator<DefaultBufferType>>
 */
inline Buffer<DefaultBufferType, default_allocator<DefaultBufferType>>
make_buffer(std::size_t size) {
  return Buffer<DefaultBufferType, default_allocator<DefaultBufferType>>(
      size, get_default_alloc());
}

using SocketBufferType = Buffer<DefaultBufferType>;

}  // namespace socket
}  // namespace ara

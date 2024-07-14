/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-11-17 17:41:14
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-06-13 10:44:17
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include "ara/signal/SignalManager.h"

#include <ara/log/logger.h>
#include <pthread.h>

#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <condition_variable>

namespace ara {
namespace signal {

using io_context = boost::asio::io_context;
using strand_type = boost::asio::strand<io_context::executor_type>;
using thread_pool = boost::asio::thread_pool;
using signal_set = boost::asio::signal_set;

class SignalManager::Impl {
 private:
  ara::core::StringView ctxId_ = "CSIG";
  ara::core::StringView ctxDesc_ = "ara-signal logger";
  io_context io_;
  ara::log::Logger& logger_;
  strand_type strand_;
  thread_pool pool_;
  signal_set signals_;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
      work_;

  std::atomic_uint32_t exit_nums_{0};
  std::atomic_bool call_cancle_{false};
  std::atomic_bool registered_repeatly_handler_{false};
  std::atomic_bool has_repeat_handler_{false};
  uint32_t thread_nums_ = 1U;

 public:
  Impl(uint32_t thread_nums)
      : logger_(ara::log::CreateLogger(ctxId_, ctxDesc_)),
        strand_(boost::asio::make_strand(io_)),
        pool_(thread_nums),
        signals_(io_),
        work_(boost::asio::make_work_guard(io_)),
        thread_nums_(thread_nums) {
    for (uint32_t i = 0; i < thread_nums_; i++) {
      boost::asio::post(pool_, [this, i]() {
        pthread_setname_np(
            pthread_self(),
            std::string("ara-signal-" + std::to_string(i)).c_str());
        io_.run();
        exit_nums_++;
      });
    }
  }

  ~Impl() {
    Cancle();

    if (!io_.stopped()) {
      // Indicate that the work is no longer outstanding.
      work_.reset();
    }
    // Wait all outstanding work to be finished.
    pool_.join();
  }

  void Stop() {
    io_.stop();
    pool_.stop();
    pool_.join();
  }

  void Cancle() {
    call_cancle_ = true;
    if (registered_repeatly_handler_) {
      while (call_cancle_) {
        signals_.cancel();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    } else {
      signals_.cancel();
    }
  }

  void Clear() { signals_.clear(); }

  void Remove(const int32_t& signal_number) { signals_.remove(signal_number); }

  ara::log::Logger& GetLogger() { return logger_; }

  void async_wait(
      std::function<void(const std::error_code&, int signal_number)> handle) {
    // signals_.async_wait(handle);
    signals_.async_wait(boost::asio::bind_executor(strand_, handle));
  }

  void async_wait(std::function<void(int signal_number)> handle) {
    signals_.async_wait(boost::asio::bind_executor(
        strand_, [&, handle](const std::error_code& ec, int signal_number) {
          if (ec.value() == 0) {
            handle(signal_number);
          } else {
            logger_.LogWarn() << "async_wait:" << ec.message();
          }
        }));
  }

  void async_wait_repeatly(
      std::function<void(const std::error_code&, int signal_number)> handle) {
    registered_repeatly_handler_ = true;
    signals_.async_wait(boost::asio::bind_executor(
        strand_, [&, handle](const std::error_code& ec, int signal_number) {
          handle(ec, signal_number);
          if (ec.value() == 0) {
            this->async_wait_repeatly(handle);
            logger_.LogDebug() << "add another async wait...";
          } else if (ec == std::errc::operation_canceled) {
            logger_.LogWarn() << "async_wait_repeatly:"
                              << "Repeat async wait handler was cancled.";
            call_cancle_ = false;
            registered_repeatly_handler_ = false;
          } else {
            logger_.LogWarn() << "async_wait_repeatly:" << ec.message();
            this->async_wait_repeatly(handle);
          }
        }));
  }

  void async_wait_repeatly(std::function<void(int signal_number)> handle) {
    registered_repeatly_handler_ = true;
    signals_.async_wait(boost::asio::bind_executor(
        strand_, [&, handle](const std::error_code& ec, int signal_number) {
          if (ec.value() == 0) {
            handle(signal_number);
            this->async_wait_repeatly(handle);
            logger_.LogDebug() << "add another async wait...";
          } else if (ec == std::errc::operation_canceled) {
            logger_.LogWarn() << "async_wait_repeatly:"
                              << "Repeat async wait hadler was cancled.";
            call_cancle_ = false;
            registered_repeatly_handler_ = false;
          } else {
            logger_.LogWarn() << "async_wait_repeatly:" << ec.message();
            this->async_wait_repeatly(handle);
          }
        }));
  }

  void add_signal(int32_t signal_number) { signals_.add(signal_number); }
};

SignalManager::SignalManager(uint32_t thread_nums)
    : pImpl_{std::make_unique<Impl>(thread_nums)} {}

SignalManager::~SignalManager() {}

void SignalManager::AddSignal(const int32_t& signal_number) {
  pImpl_->GetLogger().LogDebug() << "add signal" << signal_number;
  pImpl_->add_signal(signal_number);
}

void SignalManager::AsyncWait(
    std::function<void(const std::error_code&, int32_t signal_number)> handle) {
  pImpl_->async_wait(handle);
};

void SignalManager::AsyncWait(
    std::function<void(int32_t signal_number)> handle) {
  pImpl_->async_wait(handle);
};

void SignalManager::AsyncWaitRepeatly(
    std::function<void(int32_t signal_number)> handle) {
  pImpl_->async_wait_repeatly(handle);
};

void SignalManager::AsyncWaitRepeatly(
    std::function<void(const std::error_code&, int32_t signal_number)> handle) {
  pImpl_->async_wait_repeatly(handle);
};

void SignalManager::Cancle() { pImpl_->Cancle(); };

void SignalManager::Clear() { pImpl_->Clear(); };

void SignalManager::Remove(const int32_t& signal_number) {
  pImpl_->Remove(signal_number);
};

}  // namespace signal
}  // namespace ara
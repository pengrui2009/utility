/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-11-17 17:43:48
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-01-10 13:44:54
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#include <functional>
#include <memory>
#include <system_error>

namespace ara {
namespace signal {

class SignalManager {
 private:
  class Impl;
  std::unique_ptr<Impl> pImpl_;
  SignalManager(const SignalManager&) = delete;
  SignalManager(SignalManager&&) = delete;
  SignalManager& operator=(const SignalManager&) = delete;
  SignalManager& operator=(SignalManager&&) = delete;

 public:
  /**
   * @brief Construct a new Signal Manager object
   *
   * @param thread_nums Thread nums of running user handlers
   */
  SignalManager(uint32_t thread_nums = 1);
  ~SignalManager();

  /**
   * @brief Add several signals to signal set
   *
   * @tparam T Int type
   * @tparam Args Arg list, int type
   * @param signal_number signal number
   * @param args signal number list
   */
  template <typename T, typename... Args>
  void AddSignal(const T& signal_number, const Args&... args) {
    static_assert(std::is_integral<T>::value,
                  "Type of signal param must can be converted to integral.");
    AddSignal(static_cast<int32_t>(signal_number));
    AddSignal(args...);
  };

  /**
   * @brief Add one signal to signal set
   *
   * @param signal_number signal number
   */
  void AddSignal(const int32_t& signal_number);

  /**
   * @brief Add ONE handler to process ONE signal notification. If multi signals
   * were notified, multi handlers need to be added. You can also call this
   * multi times to  add multi handlers for one signal notification just before
   * the signal notification coming. Any error occurs, a error code
   * will be passed to the handler
   *
   * @param handle
   */
  void AsyncWait(
      std::function<void(const std::error_code&, int32_t signal_number)>
          handle);
  /**
   * @brief Add ONE handler to process ONE signal notification. If multi signals
   * were notified, multi handlers need to be added. You can also call this
   * multi times to  add multi handlers for one signal notification just before
   * the signal notification coming. The error code will not be passed to user
   * handler.
   *
   * @param handle
   */
  void AsyncWait(std::function<void(int32_t signal_number)> handle);
  /**
   * @brief Add ONE repeat handler process ONE multi notifications. One
   * SignalManager can only add one repeay handler.
   *
   * @param handle Async handler with no error code.
   */
  void AsyncWaitRepeatly(std::function<void(int32_t signal_number)> handle);
  /**
   * @brief Add ONE repeat handler process ONE multi notifications. One
   * SignalManager can only add one repeay handler.
   *
   * @param handle Async handler with error code.
   */
  void AsyncWaitRepeatly(
      std::function<void(const std::error_code&, int32_t signal_number)>
          handle);

  /**
   * @brief Cancle all waiting handlers.
   *
   */
  void Cancle();

  /**
   * @brief Clear all signals. If some signal was been notificed(queued) but has
   * not been processed by handler(dequeued), the signal notification will be
   * removed from the waiting queue and handler will never process this signal
   * notification any more.
   *
   */
  void Clear();

  /**
   * @brief Clear all signals. If this signal was been notificed(queued) but has
   * not been processed by handler(dequeued), this signal notification will be
   * removed from the waiting queue and handler will never process this signal
   * notification any more.
   *
   */
  void Remove(const int32_t& signal_number);
};

}  // namespace signal
}  // namespace ara

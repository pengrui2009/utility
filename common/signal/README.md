<!--
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2023-01-04 15:22:06
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-03 16:57:22
 * @Description: 
 * 
 * Copyright (c) 2023 by Tusimple, All Rights Reserved. 
-->
# **ara-signal Getting started**
## **Overview**
设计文档参见：[here](https://tusen.feishu.cn/wiki/wikcnqs6e4UG3oUY9mdeN0yUh6f)<br>
ara-signal是ap common组件的其中之一，用于提供qnx/linux平台下信号事件注册及回调功能。

## **Design**
ara-signal基于`boost::asio::Signal`开发，因此其底层逻辑与`boost::asio::Signal`一致。
### **Signal Handler**
用户通过ara-Signal的`AsyncWait`或`AsyncWaitRepeatly`接口注册的handler（后续称为user handler）,并非向system注册的handler。实际上向system注册的handler为`boost::asio::Signal`中实现的handler(后续称为system handler)。

收到信号通知时，操作系统触发system handler。system handler在触发调用过程中，**会屏蔽所有信号**，同时中断原有程序调用，当system handler执行完成后，会继续执行原有中断位置代码。

user handler是用户通过`AsyncWait`或`AsyncWaitRepeatly`注册的handler。通过前者，**每次调用只注册一个handler**，后续一次调用可以注册一个可重复触发的handler，其实现原理为在user handler触发后，再自动注册该user handler。

调用`Cancle`接口，将取消所有等待信号通知的user handler，正在调用的user handler无法被取消。

### **Signal Queue**
system handler执行完成后，会将该信号通知添加至内部队列(signal queue)。若此时有等待的user handler，则一次取出一个信号通知，交由一个user handler处理。多个信号通知触发时，会按照信号值升序（优先添加信号值较小的信号通知）将信号通知添加至signal queue。

调用`Remove`或`Clear`接口时，若取消订阅的信号已经有对应的信号通知在signal queue中，则该信号通知会被从signal queue中移除。

# **Quick Start**
## **Samples**
[one_shot_signal](../../../samples/signal/oneshot_signal.cc),[repeat_signal](../../../samples/signal/repeat_signal.cc)
## **SignalManager Api**
```c++
  /**
   * @brief Construct a new Signal Manager object
   *
   * @param thread_nums Thread nums of running user handlers
   */
  SignalManager(uint32_t thread_nums = 1);

  /**
   * @brief Add several signals to signal set
   *
   * @tparam T Int type
   * @tparam Args Arg list, int type
   * @param signal_number signal number
   * @param args signal number list
   */
  template <typename T, typename... Args>
  void AddSignal(const T& signal_number, const Args&... args);


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
```

# **注意事项**
1. 每个`SignalManager`仅允许使用`AsyncWaitRepeatly()`接口注册一个重复调用的回调函数。若要重新注册该回调函数，需要先调用`Cancle()`接口，再调用`AsyncWaitRepeatly()`。
2. 每个`SignalManager`可多次调用`AsyncWait()`接口注册多个的回调函数，注册的多个回调函数将在收到一次信号通知时全部被调用，可以利用次特性为一个信号注册不同的回调函数。
3. 避免在一个`SignalManager`对象中同时调用`AsyncWaitRepeatly`和`AsyncWait`接口。如需要切换不同类型的handler，首先调用`Cancle()`接口取消已注册的所有user handler，再调用相应的注册handler接口。
4. 通过ara-signal注册的回调函数，是在`SignalManager`内置的线程池中被执行。注册的回调函数的执行时机与信号中断位置无关。
5. 多线程条件下，同一个`SignalManager`注册的user handler是线程安全的。尽量避免在user handler中进行耗时计算或操作。
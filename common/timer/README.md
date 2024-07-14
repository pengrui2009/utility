<!--
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-09-19 14:45:57
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-01-09 17:48:02
 * @FilePath: /adaptive_autosar/tools/dlt-receiver-sdk/README.md
 * @Description: 
 * 
 * Copyright (c) 2022 by Tusimple, All Rights Reserved. 
-->
# ara-Timer Getting start

## **Overview**
ara-Timer是ap common组件的其中之一，用于提供常见的阻塞定时、回调定时等功能。底层依赖boost::asio实现。
定时器内部使用的是标准库时钟：`std::chrono::system_clock`，因此定时精度可能受到系统时钟的影响。

基本功能包括：
1. 同步阻塞定时器；
2. 异步回调定时器；
3. 线程安全的多线程回调函数；
4. 手动停止未到时的定时任务；
详细需求参见[ara-timer需求分析](https://tusen.feishu.cn/wiki/wikcnwJf1dxF6fs6xA1XQ0q5c3f#YoygdmeqOoIeUKxQF4Zcmtk8nic)
***
## **Quick Start**
参见samples:<br>
[regular timer](../../../samples/timer/regular_timer.cc), [repeat timer](../../../samples/timer/repeat_timer.cc)
***
## **API**
ara-timer主要对外暴露两个类的接口，分别`class Timer`和`class TimerManager`：
* Timer:定时器类，提供定时器的相关操作；
* TimerManager:定时器管理类，初始化线程池、执行器等内部对象，创建定时器。
### Timer
```c++
  /**
   * @brief Cancel any asynchronous operations that are waiting on the timer.
   *
   * This function forces the completion of any pending asynchronous wait
   * operations against the timer. The handler for each cancelled operation will
   * be invoked with the std::errc::operation_canceled error code.
   *
   * Cancelling the timer does not change the expiry time.
   *
   * @return The number of asynchronous operations that were cancelled.
   *
   * @note If the timer has already expired when cancel() is called, then the
   * handlers for asynchronous wait operations will:
   *
   * @li have already been invoked; or
   *
   * @li have been queued for invocation in the near future.
   *
   * These handlers can no longer be cancelled, and therefore are passed an
   * error code that indicates the successful completion of the wait operation.
   */
  std::size_t Cancle();
  /**
   * @brief Get the timer's expiry time as an absolute time.
   *
   * This function may be used to obtain the timer's current expiry time.
   * Whether the timer has expired or not does not affect this value.
   */
  std::chrono::system_clock::time_point Expiry();
```
### TimerManager
```c++
/**
   * @brief Add a timer.
   *
   * @param time_point The expire time poit.
   *
   * @param handle The callback function.
   *
   * The function signature of the handler must be:
   * @code void handler(
   *   const std::error_code& error // Result of operation.
   * ); @endcode
   *
   * @return TimerPtr The created timer instance.
   */
  TimerPtr AddTimer(const time_point& time_point,
                    std::function<void(const std::error_code&)> handle);
  /**
   * @brief Add a timer, only set it's expiry time.
   *
   * @param expiry_time_from_now
   *
   * @return * TimerPtr The created timer instance.
   */
  TimerPtr AddTimer(const time_point& time_point);
  /**
   * @brief Add a timer.
   *
   * @param time_point The expire time poit.
   *
   * @param handle The callback function.
   *
   * The function signature of the handler must be:
   * @code void handler(
   *   const std::error_code& error // Result of operation.
   * ); @endcode
   *
   * @return TimerPtr The created timer instance.
   */
  TimerPtr AddTimer(const duration& expiry_time_from_now,
                    std::function<void(const std::error_code&)> handle);
  /**
   * @brief Add a timer, only set it's expiry time.
   *
   * @param expiry_time_from_now
   *
   * @return * TimerPtr The created timer instance.
   */
  TimerPtr AddTimer(const duration& expiry_time_from_now);

  /**
   * @brief Set the Callback Handle object for a timer
   *
   * @param p_timer
   * @param handle
   */
  void SetCallbackHandle(const TimerPtr& p_timer,
                         std::function<void(const std::error_code&)> handle);
  /**
   * @brief This function sets the expiry time. Any pending asynchronous wait
   * operations will be cancelled.
   *
   * @return The number of asynchronous operations that were cancelled.
   */
  std::size_t SetTimer(const TimerPtr& p_timer, const time_point& time_point,
                       std::function<void(const std::error_code&)> handle);
  /**
   * @brief This function sets the expiry time. Any pending asynchronous wait
   * operations will be cancelled.
   *
   * @return The number of asynchronous operations that were cancelled.
   */
  std::size_t SetTimer(const TimerPtr& p_timer,
                       const duration& expiry_time_from_now,
                       std::function<void(const std::error_code&)> handle);
  /**
   * @brief Perform a blocking wait on the timer.
   *
   * This function is used to wait for the timer to expire. This function
   * blocks and does not return until the timer has expired.
   *
   */
  void AddDelayObject(const duration& duration);
  /**
   * @brief Perform a blocking wait on the timer.
   *
   * This function is used to wait for the timer to expire. This function
   * blocks and does not return until the timer has expired.
   *
   */
  void AddDelayObject(const time_point& time_point);
  /**
   * @brief This equals to Timer.Cancle()
   */
  void RemoveTimer(const TimerPtr& p_timer);
  /**
   * @brief Shutdown all timers.
   *
   * Callback handlers may not be invoked. If you want the TimerManager instance
   * to exit immediately, call Stop(), or the destructor of TimerManager will
   * wait for all the pending handlers to stop and then exit.
   */
  void Stop();
```
***
## **注意事项**
* 取消未到时的异步定时任务，**仍然会调用设置的回调函数**，但传入的error code将大于0.用户可根据error code的值来判断回调函数是正常调用还是取消调用。
* **同一个TimerManager**所创建的异步定时任务是**线程安全**的，用户无需为回调函数内的操作加锁，但应从设计上应减少回调函数内的逻辑处理，减少回调处理时间。
* 如果**timer溢出的时间长于timer对象自身的生命周期**，那么在timer被析构时，会调用该timer的Cancle()接口，而不是等待定时器溢出后调用回调函数。
* TimerManager对象被析构时，**会等待所有未完成的任务执行完成后再退出**。如果想直接退出，需要调用TimerManager.Stop()接口。

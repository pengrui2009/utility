/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-10-27 14:47:11
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-10-31 14:34:48
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#ifndef AEG_ADAPTIVE_AUTOSAR_ARA_API_COMMON_TIMER_TIMER_H_
#define AEG_ADAPTIVE_AUTOSAR_ARA_API_COMMON_TIMER_TIMER_H_

#include <chrono>
#include <memory>

namespace ara
{
    namespace timer
    {

        using time_point = std::chrono::system_clock::time_point;

        class TimerManager;

        class Timer
        {
        private:
            class Impl;
            std::unique_ptr<Impl> pImpl_;
            friend class TimerManager;

        public:
            Timer();
            ~Timer();
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
            time_point Expiry();
        };

    } // namespace timer
} // namespace ara

#endif
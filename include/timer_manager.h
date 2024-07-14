/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-10-27 14:59:40
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-11-09 10:48:33
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#ifndef AEG_ADAPTIVE_AUTOSAR_ARA_API_COMMON_TIMER_TIMER_MANAGER_H_
#define AEG_ADAPTIVE_AUTOSAR_ARA_API_COMMON_TIMER_TIMER_MANAGER_H_

#include <functional>
#include <system_error>

#include "timer.h"

namespace ara
{
    namespace timer
    {

        using TimerPtr = std::shared_ptr<Timer>;
        using duration = std::chrono::system_clock::duration;

        class TimerManager
        {
        private:
            class Impl;
            std::unique_ptr<Impl> pImpl_;

        public:
            TimerManager(int32_t n_threads = 1);
            ~TimerManager();
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
            TimerPtr AddTimer(const time_point &time_point,
                              std::function<void(const std::error_code &)> handle);
            /**
             * @brief Add a timer, only set it's expiry time.
             *
             * @param expiry_time_from_now
             *
             * @return * TimerPtr The created timer instance.
             */
            TimerPtr AddTimer(const time_point &time_point);
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
            TimerPtr AddTimer(const duration &expiry_time_from_now,
                              std::function<void(const std::error_code &)> handle);
            /**
             * @brief Add a timer, only set it's expiry time.
             *
             * @param expiry_time_from_now
             *
             * @return * TimerPtr The created timer instance.
             */
            TimerPtr AddTimer(const duration &expiry_time_from_now);

            /**
             * @brief Set the Callback Handle object for a timer
             *
             * @param p_timer
             * @param handle
             */
            void SetCallbackHandle(const TimerPtr &p_timer,
                                   std::function<void(const std::error_code &)> handle);
            /**
             * @brief This function sets the expiry time. Any pending asynchronous wait
             * operations will be cancelled.
             *
             * @return The number of asynchronous operations that were cancelled.
             */
            std::size_t SetTimer(const TimerPtr &p_timer, const time_point &time_point,
                                 std::function<void(const std::error_code &)> handle);
            /**
             * @brief This function sets the expiry time. Any pending asynchronous wait
             * operations will be cancelled.
             *
             * @return The number of asynchronous operations that were cancelled.
             */
            std::size_t SetTimer(const TimerPtr &p_timer,
                                 const duration &expiry_time_from_now,
                                 std::function<void(const std::error_code &)> handle);
            /**
             * @brief Perform a blocking wait on the timer.
             *
             * This function is used to wait for the timer to expire. This function
             * blocks and does not return until the timer has expired.
             *
             */
            void AddDelayObject(const duration &duration);
            /**
             * @brief Perform a blocking wait on the timer.
             *
             * This function is used to wait for the timer to expire. This function
             * blocks and does not return until the timer has expired.
             *
             */
            void AddDelayObject(const time_point &time_point);
            /**
             * @brief This equals to Timer.Cancle()
             */
            void RemoveTimer(const TimerPtr &p_timer);
            /**
             * @brief Shutdown all timers.
             *
             * Callback handlers may not be invoked. If you want the TimerManager instance
             * to exit immediately, call Stop(), or the destructor of TimerManager will
             * wait for all the pending handlers to stop and then exit.
             */
            void Stop();
        };

    } // namespace timer
} // namespace ara

#endif
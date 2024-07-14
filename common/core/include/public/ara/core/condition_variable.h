/*
 * @Description: 
 * @Author: an.huang@tusen.ai
 * @Date: 2023-11-06 06:00:42
 * @LastEditors: an.huang
 * @LastEditTime: 2023-11-14 05:54:36
 * @FilePath: /root/adas-monorepo/modules/adaptive_autosar/ara-api/core/include/public/ara/core/condition_variable.h
 * @version: 
 *  
 * Copyright (c) 2023 by Tusen, All Rights Reserved.
 */
#ifndef ARA_CORE_CONDITION_VARIABLE_H_
#define ARA_CORE_CONDITION_VARIABLE_H_

#include <cstdint>
#include <mutex>

#include <ara/core/pthread_helper.h>

namespace ara{
namespace core{

enum class cv_status: std::uint8_t {
    no_timeout,
    timeout
};

class ConditionVariable {
    private:
    using internal_clock = SteadyClock;
    
    public:
    ConditionVariable() noexcept {
        _pthread_cond_init(&(_cond));
    }

    ~ConditionVariable() noexcept {
        _pthread_cond_destroy(&(_cond));
    }

    ConditionVariable(const ConditionVariable&) = delete;
    ConditionVariable(ConditionVariable&&) = delete;

    void wait(std::unique_lock<std::mutex>& lock) noexcept {
        _pthread_cond_wait(&_cond, lock.mutex()->native_handle());
    }

    template <typename Predicate>
    void wait(std::unique_lock<std::mutex>& lock, Predicate p) {
        while( !p() )
            wait(lock);
    }

    template <typename Rep, typename Period>
    cv_status wait_for(std::unique_lock<std::mutex>& lock,
                    const std::chrono::duration<Rep,Period>& dur) {
        auto tp = convert_to_timepoint(dur);
        return wait_until_impl(lock, tp);
    }    

    template <typename Rep, typename Period, typename Predicate>
    bool wait_for(std::unique_lock<std::mutex>& lock,
                const std::chrono::duration<Rep,Period>& dur,
                Predicate p) {
        if( wait_for(lock, dur) == cv_status::timeout )
            return false;
        return static_cast<bool>(p());
    }
 
    template <typename Duration>
    cv_status wait_until(std::unique_lock<std::mutex>& lock,
                        const std::chrono::time_point<internal_clock, Duration>& time) {
        return wait_until_impl(lock, time);
    }

    template <typename Duration, typename Predicate>
    bool wait_until(std::unique_lock<std::mutex>& lock,
                    const std::chrono::time_point<internal_clock, Duration>& time,
                    Predicate p) {
        if( wait_until(lock, time) == cv_status::timeout )
            return false;
        return static_cast<bool>(p());
    }
     
    void notify_one() noexcept {
        _pthread_cond_signal(&_cond);
    }

    void notify_all() noexcept {
        _pthread_cond_broadcast(&_cond);
    }

    typedef pthread_cond_t* native_handle_type;
    native_handle_type native_handle() {
        return &_cond;
    }

    private:
    pthread_cond_t _cond;

    template <typename Rep, typename Period>
    std::chrono::time_point<internal_clock, internal_clock::duration> convert_to_timepoint(const std::chrono::duration<Rep,Period>& dur) {
        auto rel_time = std::chrono::duration_cast<internal_clock::duration>(dur);
        if(rel_time < dur)
            ++rel_time;
        return internal_clock::now() + rel_time;
    }

    template <typename Duration>
    timespec convert_to_abstime(const std::chrono::time_point<internal_clock, Duration>& tp) {
        auto sec = std::chrono::time_point_cast<std::chrono::seconds>(tp);
        auto nano_sec = std::chrono::duration_cast<std::chrono::nanoseconds>(tp-sec);

        timespec ts{
            static_cast<std::time_t>(sec.time_since_epoch().count()),
            static_cast<long>(nano_sec.count())
        };

        return ts;
    }

    template <typename Duration>
    cv_status wait_until_impl(
        std::unique_lock<std::mutex>& lock, 
        const std::chrono::time_point<internal_clock, Duration>& time) {

        auto ts = convert_to_abstime(time);
        _pthread_cond_timedwait( &_cond, lock.mutex()->native_handle(), &ts );
        return (internal_clock::now() < time ? cv_status::no_timeout : cv_status::timeout);     
    } 
};

inline namespace extend{

class ConditionVariableAny {
    private:
    using internal_clock = SteadyClock;

    template <typename mutex_type>
    class lock_on_exit {
        private:
        mutex_type* _mutex{nullptr};

        public:
        lock_on_exit() = default;

        void activate(mutex_type& m) {
            m.unlock();
            _mutex = &m;
        }

        void deactivate() {
            if( _mutex )
                _mutex->lock();
            _mutex = nullptr;
        }

        ~lock_on_exit() {
            if( _mutex )
                _mutex->lock();
        }
    };

    public:
    ConditionVariableAny() noexcept {
        _pthread_cond_init(&_cond);
        _pthread_mutex_init(&_mutex);
    }
    ~ConditionVariableAny() noexcept {
        _pthread_cond_destroy(&_cond);
        _pthread_mutex_destroy(&_mutex);
    }

    ConditionVariableAny(const ConditionVariableAny&) = delete;
    ConditionVariableAny(ConditionVariableAny&&) = delete;

    template <typename Lock>
    void wait(Lock& lock) noexcept {
        lock_on_exit<Lock> lck{};
        lck.activate(lock);
        _pthread_mutex_lock(&_mutex);
        _pthread_cond_wait(&_cond, &_mutex);
        lck.deactivate();
        _pthread_mutex_unlock(&_mutex);
    }

    template <typename Lock, typename Predicate>
    void wait(Lock& lock, Predicate p) {
        while( !p() )
            wait(lock);
    }

    template <typename Lock, typename Rep, typename Period>
    cv_status wait_for(Lock& lock,
                    const std::chrono::duration<Rep,Period>& dur) {
        auto tp = convert_to_timepoint(dur);
        return wait_until_impl(lock, tp);
    }

    template <typename Lock, typename Rep, typename Period, typename Predicate>
    bool wait_for(Lock& lock,
                const std::chrono::duration<Rep,Period>& dur,
                Predicate p) {
        if( wait_for(lock, dur) == cv_status::timeout )
            return false;
        return static_cast<bool>(p());
    }
                
    template <typename Lock, typename Duration>
    cv_status wait_until(Lock& lock,
                        const std::chrono::time_point<internal_clock, Duration>& time) {
        return wait_until_impl(lock, time);
    }
                        
    template <typename Lock, typename Duration, typename Predicate>
    bool wait_until(Lock& lock,
                    const std::chrono::time_point<internal_clock, Duration>& time,
                    Predicate p) {
        if( wait_until(lock, time) == cv_status::timeout )
            return false;
        return static_cast<bool>(p());
    }
                    
    void notify_one() noexcept {
        _pthread_cond_signal(&_cond);
    }

    void notify_all() noexcept {
        _pthread_cond_broadcast(&_cond);
    }

    private:
    pthread_cond_t _cond;
    pthread_mutex_t _mutex;

    template <typename Rep, typename Period>
    std::chrono::time_point<internal_clock, internal_clock::duration> convert_to_timepoint(const std::chrono::duration<Rep,Period>& dur) {
        auto rel_time = std::chrono::duration_cast<internal_clock::duration>(dur);
        if(rel_time < dur)
            ++rel_time;
        return internal_clock::now() + rel_time;
    }

    template <typename Duration>
    timespec convert_to_abstime(const std::chrono::time_point<internal_clock, Duration>& tp) {
        auto sec = std::chrono::time_point_cast<std::chrono::seconds>(tp);
        auto nano_sec = std::chrono::duration_cast<std::chrono::nanoseconds>(tp-sec);

        timespec ts{
            static_cast<std::time_t>(sec.time_since_epoch().count()),
            static_cast<long>(nano_sec.count())
        };

        return ts;
    }

    template <typename Lock, typename Duration>
    cv_status wait_until_impl(
        Lock& lock,
        const std::chrono::time_point<internal_clock, Duration>& time) {

        lock_on_exit<Lock> lck{};
        lck.activate(lock);
        _pthread_mutex_lock(&_mutex);
        auto ts = convert_to_abstime(time);
        _pthread_cond_timedwait( &_cond, &_mutex, &ts );
        auto re =  internal_clock::now() < time ? cv_status::no_timeout : cv_status::timeout;
        lck.deactivate();
        _pthread_mutex_unlock(&_mutex);
        return re;
    }

};
} // inline namespace extend

} // namespace core
} // namespace ara


#endif

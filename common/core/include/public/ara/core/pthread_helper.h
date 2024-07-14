/*
 * @Description: 
 * @Author: an.huang@tusen.ai
 * @Date: 2023-11-06 06:03:26
 * @LastEditors: an.huang
 * @LastEditTime: 2023-11-07 06:15:52
 * @FilePath: /root/adas-monorepo/modules/adaptive_autosar/ara-api/core/include/public/ara/core/pthread_helper.h
 * @version: 
 *  
 * Copyright (c) 2023 by Tusen, All Rights Reserved.
 */
#ifndef ARA_CORE_PTHREAD_HELPER_H_
#define ARA_CORE_PTHREAD_HELPER_H_

#include <pthread.h>
#include <chrono>

namespace ara{
namespace core{

typedef std::chrono::steady_clock SteadyClock;
typedef std::chrono::system_clock SystemClock;

inline namespace posix{

inline int _pthread_cond_init(
        pthread_cond_t *cond){

    pthread_condattr_t attr;
    ::pthread_condattr_init(&attr);
    ::pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    
    int re = ::pthread_cond_init(cond, &attr);
    ::pthread_condattr_destroy(&attr);

    return re;
}

inline int _pthread_cond_destroy(pthread_cond_t *cond){
    return ::pthread_cond_destroy(cond);
}

inline int _pthread_cond_broadcast(pthread_cond_t *cond){
    return ::pthread_cond_broadcast(cond);
}

inline int _pthread_cond_signal(pthread_cond_t *cond){
    return ::pthread_cond_signal(cond);
}

inline int _pthread_cond_timedwait(
        pthread_cond_t *cond, 
        pthread_mutex_t *mutex, 
        const struct timespec *abstime){
    return ::pthread_cond_timedwait(cond, mutex, abstime);
}

inline int _pthread_cond_wait(
        pthread_cond_t *cond, 
        pthread_mutex_t *mutex){
    return ::pthread_cond_wait(cond, mutex);
}

inline int _pthread_mutex_init(
    pthread_mutex_t* mutex,
    pthread_mutexattr_t* attr = NULL){
    return ::pthread_mutex_init(mutex, attr);
}

inline int _pthread_mutex_destroy(
    pthread_mutex_t* mutex){
    return ::pthread_mutex_destroy(mutex);
}

inline int _pthread_mutex_lock(
    pthread_mutex_t* mutex){
        return ::pthread_mutex_lock(mutex);
}

inline int _pthread_mutex_unlock(
    pthread_mutex_t* mutex){
        return ::pthread_mutex_unlock(mutex);
}

} //inline namespace posix
} //namespace core
} //namespace ara


#endif
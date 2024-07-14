#pragma once

#include <iostream>
#include <pthread.h>
#include <functional>

#include <memory>
#include <mutex>
#include <condition_variable>

#include <list>
#include <deque>
#include "sal_utils.hpp"
#include "hal_log/hal_log.h"

using namespace std;

template <class Obj, class Func, class... Argumts>
static inline function<void()> CREATE_FUNCTION_OBJ(Obj *obj, Func fun, Argumts... arg)
{
    function<void()> ptr = nullptr;

    if constexpr (!std::is_same<Obj, void>::value)
    {
        ptr = std::bind(std::mem_fn(fun), obj, arg...);
    }
    else
    {
        ptr = std::bind(fun, arg...);
    }

    return ptr;
}

class Thread
{
public:
    Thread() {}
    ~Thread()
    {
        if (_pid > 0)
        {
            pthread_detach(_pid);
        }
        //pthread_join(_pid, NULL);
    }
    int start(function<void()> fRun, const std::string &name)
    {
        _runb = fRun;
        _name = name;
        int ret = pthread_create(&_pid, NULL, handleTaskQueue, this);
        HAL_LOG_INFO_FMT("Thread::start  create:{} ,runObj:{}", _name.c_str(), fmt::ptr(_runb.target<void (*)()>()));
        return ret;
    }

private:
    static void *handleTaskQueue(void *s)
    {
        Thread *tkq = (Thread *)s;
        pthread_setname_np(pthread_self(), tkq->_name.c_str());
        if (tkq->_runb != nullptr)
        {
            tkq->_runb();
        }
        else
        {
            HAL_LOG_ERROR_FMT("error Thread:{} ,runb is nullptr", tkq->_name.c_str());
        }

        return nullptr;
    }

private:
    function<void()> _runb = nullptr;
    pthread_t _pid;
    std::string _name;
};

class Timer
{
public:
    Timer(function<void()> &o, uint64_t i, bool r, const std::string &funcN) : obj(o),
                                                                               interval(i),
                                                                               nextTime(SalUtils::getTimeStampMilliSecond()),
                                                                               repeat(r),
                                                                               funcName(funcN)
    {
        updateNextTime();
        HAL_LOG_INFO_FMT("Timer funcName:{}, nextTime:{},interval:{},repeat:{}", funcName.c_str(), nextTime, interval, repeat);
    }
    ~Timer() {}

    void updateNextTime()
    {
        nextTime += interval;
    }

    void resetNextTime()
    {
        nextTime = SalUtils::getTimeStampMilliSecond() + interval;
    }

    bool operator<(const Timer &timer) const
    {
        return nextTime < timer.nextTime;
    }

public:
    function<void()> obj;
    uint64_t interval;
    uint64_t nextTime;
    bool repeat;
    std::string funcName;
};

class TimerQueue
{
public:
    typedef function<void()> FunObj;
    TimerQueue()
    {
        _isStop = true;
        _condi.isRouse = false;
    };
    ~TimerQueue()
    {
        release();
    }

    int start(const std::string &name)
    {
        if (!_isStop)
        {
            return -1;
        }

        _isStop = false;
        FunObj j = CREATE_FUNCTION_OBJ(this, &TimerQueue::timerProc);
        int ret = _thread.start(j, "TQ_" + name);
        return ret;
    }

    void stop()
    {
        _isStop = true;
        release();
        notify();
    }

    int64_t addTimer(FunObj obj, uint64_t msecInterval, bool repeat, const std::string &fuName)
    {
        if (_isStop)
        {
            return 0;
        }

        Timer timer(obj, msecInterval, repeat, fuName);
        int64_t tId = reinterpret_cast<int64_t>(&timer);
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _taskQ.emplace_back(timer);
            _taskQ.sort();
            size_t tSiz = _taskQ.size();
            if (tSiz > 20)
            {
                HAL_LOG_WARN_FMT("timer queue size:{}", (int)tSiz);
            }
        }

        notify();
        return tId;
    }

    void stopTimer(int64_t timerID)
    {
        if (_isStop)
        {
            return;
        }

        {
            std::lock_guard<std::mutex> lock(_mutex);
            std::list<Timer>::iterator ite = _taskQ.begin();
            for (; ite != _taskQ.end(); ++ite)
            {
                if (timerID == reinterpret_cast<int64_t>(&(*ite)))
                {
                    _taskQ.erase(ite);
                    break;
                }
            }
        }

        /* stop run timer*/
        {
            std::lock_guard<std::mutex> runLock(_runningMutex);
            std::deque<Timer>::iterator iter = _runningTimers.begin();
            for (; iter != _runningTimers.end(); ++iter)
            {
                if (timerID == reinterpret_cast<int64_t>(&(*iter)))
                {
                    _runningTimers.erase(iter);
                    break;
                }
            }
        }
    }

    void timerProc()
    {
        while (!_isStop)
        {
            _mutex.lock();
            uint64_t microse = getWaitTimeMillisecond(SalUtils::getTimeStampMilliSecond()) * 1000;
            _mutex.unlock();

            waitMicro(microse);

            {
                std::lock_guard<std::mutex> lock(_mutex);
                std::lock_guard<std::mutex> runLock(_runningMutex);

                _runningTimers.clear();
                std::list<Timer>::iterator ite = _taskQ.begin();
                while (ite != _taskQ.end())
                {
                    Timer &timer = *ite;
                    uint64_t current = SalUtils::getTimeStampMilliSecond();

                    if (current < timer.nextTime)
                    {
                        break;
                    }

                    _runningTimers.push_back(timer);

                    if (timer.repeat)
                    {
                        // timer.updateNextTime();
                        timer.resetNextTime();
                        ++ite;
                    }
                    else
                    {
                        ite = _taskQ.erase(ite);
                    }
                }

                _taskQ.sort();
            }

            /* run timer*/
            {
                std::lock_guard<std::mutex> runLock(_runningMutex);
                while (!_runningTimers.empty())
                {
                    Timer timer = _runningTimers.front();

                    if (timer.obj != nullptr)
                    {
                        timer.obj();
                    }
                    else
                    {
                        HAL_LOG_ERROR_FMT("error [{}] run timer obj is nullptr", timer.funcName.c_str());
                    }
                    _runningTimers.pop_front();
                }

                _runningTimers.clear();
            }
        }
    }

private:
    void release()
    {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            while (!_taskQ.empty())
            {
                _taskQ.pop_front();
            }
            _taskQ.clear();
        }

        {
            std::lock_guard<std::mutex> runLock(_runningMutex);
            while (!_runningTimers.empty())
            {
                _runningTimers.pop_front();
            }
            _runningTimers.clear();
        }
    }

    uint64_t getWaitTimeMillisecond(uint64_t current /*millisecond*/) const
    {
        static const unsigned int MAX_WAIT_TIME = 60 * 1000;

        if (_taskQ.empty())
        {
            return MAX_WAIT_TIME;
        }

        const Timer &nextTimer = _taskQ.front();
        uint64_t waitTime = 0;

        if (nextTimer.nextTime > current)
        {
            waitTime = nextTimer.nextTime - current;
        }

        if (waitTime > MAX_WAIT_TIME)
        {
            waitTime = MAX_WAIT_TIME;
        }

        return waitTime;
    }

    void waitMicro(uint64_t time) //microseconds
    {
        if (time < 1)
        {
            return;
        }
        std::unique_lock<std::mutex> lock(_condi.mutexCond);
        std::chrono::microseconds timeout(time);
        _condi.condVar.wait_for(lock, timeout, [this]() { return _condi.isRouse; });
        _condi.isRouse = false;
    }

    void notify()
    {
        {
            std::lock_guard<std::mutex> lock(_condi.mutexCond);
            _condi.isRouse = true;
        }
        _condi.condVar.notify_one();
    }

    void waitMultiThread(uint64_t time) //microseconds
    {
        if (time < 1)
        {
            return;
        }

        bool isTimeout = false;
        uint64_t startWaitTime = SalUtils::getTimeStampMilliSecond();
        uint64_t currentTime = startWaitTime;

        std::unique_lock<std::mutex> lock(_condi.mutexCond);

        while (!isTimeout && !_condi.isRouse)
        {
            uint64_t passTime = (currentTime - startWaitTime) * 1000;
            if (time <= passTime)
            {
                isTimeout = true;
                break;
            }

            std::chrono::microseconds timeout(time);
            _condi.condVar.wait_for(lock, timeout, [this]() { return _condi.isRouse; });

            currentTime = SalUtils::getTimeStampMilliSecond();
        }

        _condi.isRouse = false;
    }

private:
    Thread _thread;
    std::atomic<bool> _isStop;

    list<Timer> _taskQ;
    std::mutex _mutex;

    std::deque<Timer> _runningTimers;
    std::mutex _runningMutex;

    struct Condition
    {
        std::mutex mutexCond;
        std::condition_variable condVar;
        bool isRouse;
    };
    Condition _condi;
};

template <typename T>
class SafetyQueue
{
public:
    SafetyQueue(int size) : _size(size) { HAL_LOG_INFO_FMT("SafetyQueue::SafetyQueue this:{},size:{}", fmt::ptr(this), _size); }
    ~SafetyQueue()
    {
        std::lock_guard<std::mutex> lock(_mutexB);
        for (typename std::deque<T>::iterator ite = _bufferQ.begin(); ite != _bufferQ.end();)
        {
            ite = _bufferQ.erase(ite);
        }
        _bufferQ.clear();
    }

    bool get(T &f, size_t &queueSiz, int timeout_us = 0)
    {
        bool ret = false;
        std::unique_lock<std::mutex> lock(_mutexB);

        if (timeout_us)
        {
            std::chrono::microseconds timeout(timeout_us);
            ret = _condB.wait_for(lock, timeout, [this]() { return !_bufferQ.empty(); });
        }
        else
        {
            _condB.wait(lock, [this]() { return !_bufferQ.empty(); });
            ret = true;
        }

        if (ret)
        {
            f = std::move(_bufferQ.front());
            _bufferQ.pop_front();
            queueSiz = _bufferQ.size();
        }
        return ret;
    }

    void push(const T &f)
    {
        {
            std::lock_guard<std::mutex> lock(_mutexB);

            if ((_size > 0) && ((int)_bufferQ.size() >= _size))
            {
                HAL_LOG_ERROR_FMT("SafetyQueue::push fail, {} queue full, size {}", typeid(f).name(), _bufferQ.size());
                return;
            }

            _bufferQ.emplace_back(std::move(f));
        }
        _condB.notify_one();
    }

    size_t Size()
    {
        std::lock_guard<std::mutex> lock(_mutexB);
        return _bufferQ.size();
    }

private:
    std::deque<T> _bufferQ;
    std::mutex _mutexB;
    std::condition_variable _condB;
    int _size = 0;
};
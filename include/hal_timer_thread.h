#pragma once

#include <thread>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <algorithm>
// #include "hal_log/hal_log.h"

#define HAL_PRINT_INFO(x...) fprintf(stdout, x)
#define HAL_PRINT_DEBUG(x...) fprintf(stdout, x)
#define HAL_PRINT_ERROR(x...) fprintf(stdout, x)

#define MIN_INTERVAL 10
using TimeoutProcessFun = std::function<void(void)>;

//HalTimer对应的单线程模型
//实例化之后 会起一个独立的线程
//线程的执行间隔为 timeSpace ms
namespace hal
{
    class HalTimerThread : public std::enable_shared_from_this<HalTimerThread>
    {
    private:
        std::mutex m_mutex;
        bool is_end = true;
        bool is_loop = false;
        int timeSpace = MIN_INTERVAL;
        uint64_t timer_id = 0;
        TimeoutProcessFun process_fun;
        std::thread *timer_thread = nullptr;
        std::unordered_map<std::thread *, uint64_t> t_thread_s;

        void timer_thread_fun()
        {
            pthread_setname_np(pthread_self(), "timer_td_f");
            timer_id++;
            while (timer_thread == nullptr)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                HAL_PRINT_DEBUG("timer_thread_fun %p\n", timer_thread);
            }
            HAL_PRINT_INFO("timer_thread_fun %p timer_id:%d\n", timer_thread, timer_id);
            std::unique_lock<std::mutex> lock(m_mutex);
            t_thread_s[timer_thread] = timer_id;
            lock.unlock();
            thread_local uint64_t _timer_id = timer_id;
            do
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(timeSpace));
                lock.lock();
                auto thread_Ptr = std::find_if(t_thread_s.begin(), t_thread_s.end(),
                                               [this](const std::unordered_map<std::thread *, uint64_t>::value_type item) {
                                                   // HAL_PRINT_DEBUG("item.second: %lu %lu", item.second, _timer_id);
                                                   return item.second == _timer_id;
                                               });
                if (thread_Ptr == t_thread_s.end())
                {
                    lock.unlock();
                    return;
                }
                lock.unlock();
                if (process_fun != nullptr)
                {
                    process_fun();
                }
            } while (is_loop);
            is_end = true;
        }

    public:
        HalTimerThread()
        {
        }

        void stop()
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (timer_thread != nullptr)
            {
                while (!t_thread_s.count(timer_thread))
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    HAL_PRINT_DEBUG("wait t_thread_s ...\n");
                }
                std::size_t size = t_thread_s.erase(timer_thread);
                HAL_PRINT_INFO("remove timer_thread %p remove size: %zu\n", timer_thread, size);
                timer_thread->detach();
                delete timer_thread;
                timer_thread = nullptr;
                is_end = true;
                HAL_PRINT_INFO("remove timer_thread end\n");
            }
        }

        void start()
        {
            stop();
            is_end = false;

            try
            {
                timer_thread = new std::thread(&HalTimerThread::timer_thread_fun, this);
                HAL_PRINT_INFO("timer start ... %p\n", timer_thread);
            }
            catch (std::exception &ex)
            {
                HAL_PRINT_ERROR("%s\n", ex.what());
            }
        }

        virtual ~HalTimerThread() { stop(); }

        void setCallFun(TimeoutProcessFun fun)
        {
            process_fun = fun;
        }

        //设置间隔
        void setTimeSpace(const int &space)
        {
            timeSpace = space;
        }

        //设置是否是循环执行
        void setLoop(const bool &loop)
        {
            is_loop = loop;
        }

        //返回是否已经结束
        bool isEnd()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return is_end;
        }
    };
} // namespace hal

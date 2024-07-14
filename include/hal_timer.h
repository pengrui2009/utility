#pragma once

#include <string>
#include <mutex>
#include <unordered_map>
#include "hal_timer_thread.hpp"

namespace hal
{
    class HalTimer
    {
    private:
        struct CallBackFunInfo
        {
            int count = 0;
            int spaceTime = 0;
            bool is_loop = false;
            bool is_reset = false;
            bool is_remove = false;
            TimeoutProcessFun fun;
        };

        uint32_t all_id = 0;
        std::mutex m_mutex;
        std::unordered_map<int, CallBackFunInfo> call_back_funs;
        std::unique_ptr<HalTimerThread> hal_timer_thread = nullptr;

    public:
        HalTimer()
        {
            if (hal_timer_thread == nullptr)
                hal_timer_thread = std::make_unique<HalTimerThread>();
            hal_timer_thread->setCallFun(std::bind(&HalTimer::time_out, this));
            hal_timer_thread->setTimeSpace(MIN_INTERVAL);
            hal_timer_thread->setLoop(true);
            hal_timer_thread->start();
        }

        virtual ~HalTimer()
        {
            hal_timer_thread->stop();
            hal_timer_thread.reset();
        }

        /* GLOBAL FUNCTIONS */
        /**
         * @brief 添加定时处理任务
         *
         * @param [in] firstTime 第一次执行timer处理函数的等待时间 单位ms.
         * @param [in] fun timer 处理函数 std::function<void(void)>.
         * @param [in] spaceTime 周期执行timer处理函数的周期 单位ms, 大于0有效 否则不会周期执行.
         *
         * @return 返回新创建的timer_id.
         */

        int add(int firstTime, TimeoutProcessFun fun, int spaceTime = 0)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            CallBackFunInfo info;
            info.is_loop = (spaceTime > 0);
            info.spaceTime = info.is_loop ? spaceTime : firstTime;
            info.fun = fun;
            all_id++;
            call_back_funs[all_id] = info;
            if (firstTime == 0)
            {
                fun();
            }
            return all_id;
        }

        /* GLOBAL FUNCTIONS */
        /**
         * @brief 移除定时处理任务
         *
         * @param [in] id 定时处理任务的标识.
         *
         */
        void remove(int id)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (call_back_funs.count(id) != 0)
            {
                call_back_funs[id].is_loop = false;
                call_back_funs[id].is_remove = true;
            }
        }

        /**
         * @brief 判断定时处理任务标识是否存在并且正在运行
         *
         * @param [in] id 定时处理任务的标识.
         *
         */
        bool is_exist(int id)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return call_back_funs.count(id) != 0;
        }

        /**
         * @brief 重置定时器
         *
         * @param [in] id 定时处理任务的标识.
         *
         */
        void reset(int id)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (call_back_funs.count(id) != 0)
                call_back_funs[id].is_reset = true;
        }

    private:
        void time_out()
        {
            m_mutex.lock();
            auto call_back_funT = call_back_funs;
            m_mutex.unlock();
            for (auto iter = call_back_funT.begin(); iter != call_back_funT.end(); iter++)
            {
                auto info = (*iter).second;
                auto id = (*iter).first;
                info.count++;
                if (info.count * MIN_INTERVAL >= info.spaceTime)
                {
                    if (!info.is_remove)
                    {
                        info.fun();
                    }
                    if (!info.is_loop)
                    {
                        call_back_funs.erase(id);
                        continue;
                    }
                    else
                    {
                        info.count = 0;
                    }
                }
                m_mutex.lock();
                if (call_back_funs[id].is_reset)
                {
                    call_back_funs[id].count = 0;
                    call_back_funs[id].is_reset = false;
                }
                else
                {
                    call_back_funs[id] = info;
                }
                m_mutex.unlock();
            }
        }
    };
} // namespace hal
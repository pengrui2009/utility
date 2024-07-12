/**
 * @file executor.h
 * @author rui.peng (pengrui_2009@163.com)
 * @brief the executor of ExecutorHandler, which ExecutorHandler is template
 * @version 0.1
 * @date 2024-07-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef UTILITY_EXECUTOR_H
#define UTILITY_EXECUTOR_H

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <atomic>

namespace utility
{

    template <typename ExecutorHandler>
    class Executor
    {
    public:
        // ctor
        Executor() : exit_request_{false}, running_{false}
        {
            thread_ = std::thread([&]() {
                std::unique_lock<std::mutex> exit_lck(exit_mutex_lock_);
                while (!exit_request_.load())
                {
                    if (!running_.load())
                    {
                        cond_var_.wait(exit_lck);
                    }
                    if (running_)
                    {
                        std::unique_lock<std::mutex> lck(mutex_lock_);
                        while (!queue_.empty())
                        {
                            auto func = queue_.front();
                            lck.unlock();
                            func();
                            lck.lock();
                            queue_.pop();
                        }
                        running_.store(false);
                    }
                }
            });
        }

        // dtor
        ~Executor()
        {
            exit_request_ = true;
            running_ = false;
            cond_var_.notify_one();
            thread_.join();
        }

        void Suspend()
        {
        }

        void Resume()
        {
        }

        void Cancel()
        {
            // thread_.;
        }
        // function to add job to executor
        void AddExecute(ExecutorHandler executor_handler)
        {
            std::unique_lock<std::mutex> lck(mutex_lock_);
            queue_.push(std::move(executor_handler));
            running_.store(true);
            cond_var_.notify_one();
        }

    private:
        // queue to store the elements
        std::queue<ExecutorHandler> queue_;
        // mutex to lock the critical section
        std::mutex mutex_lock_;
        // mutex to lock the critical section
        std::mutex exit_mutex_lock_;
        // threading var
        std::thread thread_;
        // conditional variable to block the thread
        std::condition_variable cond_var_;
        // flag to terminate the thread
        std::atomic<bool> exit_request_;
        // flag th start the thread
        std::atomic<bool> running_;
    };
} // namespace utility
#endif // UTILITY_EXECUTOR_H

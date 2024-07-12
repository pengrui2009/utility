/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_UTILITY_UTILITY_EXECUTOR_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_UTILITY_UTILITY_EXECUTOR_H

#include <iostream>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <atomic>

#include <unistd.h>

namespace utility
{

    class Task
    {
    public:
        // ctor
        Task() : exit_request_{false}, running_{false}
        {
            thread_ = std::thread([&]() {
                std::unique_lock<std::mutex> exit_lck(exit_mutex_lock_);
                while (!exit_request_.load())
                {
                    if (!running_.load())
                    {
                        std::cout << "suspend" << std::endl;
                        cond_var_.wait(exit_lck);
                        std::cout << "resume" << std::endl;
                    }
                    if (running_)
                    {
                        std::unique_lock<std::mutex> lck(mutex_lock_);
                        // while (!queue_.empty())
                        // {
                        //     auto func = queue_.front();
                        //     lck.unlock();
                        //     func();
                        //     lck.lock();
                        //     queue_.pop();
                        // }
                        std::cout << "Tasking is running..." << std::endl;
                        sleep(1);
                        // running_.store(false);
                    }
                }
                std::cout << "Task exit" << std::endl;
            });
        }

        // dtor
        ~Task()
        {
            exit_request_ = true;
            running_ = false;
            cond_var_.notify_one();
            thread_.join();
        }

        void Start()
        {
            running_.store(true, std::memory_order_release);
            cond_var_.notify_one();
        }

        void Suspend()
        {
            running_.store(false, std::memory_order_release);
        }

        void Resume()
        {
            running_.store(true, std::memory_order_release);
            cond_var_.notify_one();
        }

        void Cancel()
        {
            exit_request_.store(true, std::memory_order_release);
            running_.store(false, std::memory_order_release);
            cond_var_.notify_one();
        }
        // function to add job to executor
        // void AddExecute(ExecutorHandler executor_handler)
        // {
        //     std::unique_lock<std::mutex> lck(mutex_lock_);
        //     queue_.push(std::move(executor_handler));
        //     running_.store(true);
        //     cond_var_.notify_one();
        // }

    private:
        // queue to store the elements
        // std::queue<ExecutorHandler> queue_;
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
#endif // DIAGNOSTIC_CLIENT_LIB_LIB_UTILITY_UTILITY_EXECUTOR_H

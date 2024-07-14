/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-10-27 15:12:51
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-06-13 10:31:23
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include "timer_manager.h"

#include <pthread.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <condition_variable>
#include <iostream>
#include <vector>

#include "timer.h"

namespace ara
{
    namespace timer
    {

        using io_context = boost::asio::io_context;
        using boost::asio::thread_pool;
        using timer = boost::asio::system_timer;
        using strand_type = boost::asio::strand<io_context::executor_type>;

        /****************************Timer Impl*********************************/
        class Timer::Impl
        {
        private:
            std::unique_ptr<timer> tm_;

        public:
            Impl() {}

            ~Impl() { tm_->cancel(); }

            void SetIoContext(io_context &io)
            {
                tm_ = std::make_unique<timer>(timer(io));
            }

            std::size_t ExpiresAt(const time_point &tp) { return tm_->expires_at(tp); }

            std::size_t ExpiresFromNow(const duration &dua)
            {
                return tm_->expires_after(dua);
            }

            void async_wait(const strand_type &strand,
                            std::function<void(boost::system::error_code)> handler)
            {
                tm_->async_wait(boost::asio::bind_executor(
                    strand, boost::bind(handler, boost::asio::placeholders::error)));
            }

            void wait() { tm_->wait(); };

            std::size_t cancel() { return tm_->cancel(); }

            time_point expiry() { return tm_->expiry(); }
        };

        /****************************Timer*********************************/
        Timer::Timer() : pImpl_{std::make_unique<Impl>()} {};

        Timer::~Timer() {}

        std::size_t Timer::Cancle() { return pImpl_->cancel(); }

        time_point Timer::Expiry() { return pImpl_->expiry(); }

        /****************************TimerManager Impl*********************************/
        class TimerManager::Impl
        {
        private:
            io_context io_;
            std::atomic_uint32_t exit_nums_{0};
            uint32_t thread_nums_;
            thread_pool pool_;
            strand_type strand_;
            boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
                work_;

        public:
            Impl(int32_t n_threads)
                : thread_nums_(n_threads),
                  pool_(n_threads),
                  strand_(boost::asio::make_strand(io_)),
                  work_(boost::asio::make_work_guard(io_))
            {
                for (uint32_t i = 0; i < thread_nums_; i++)
                {
                    boost::asio::post(pool_, [this, i]() {
                        pthread_setname_np(
                            pthread_self(),
                            std::string("ara-timer-" + std::to_string(i)).c_str());

                        io_.run();
                        exit_nums_++;
                    });
                }
            };

            ~Impl()
            {
                if (!io_.stopped())
                {
                    // Indicate that the work is no longer outstanding.
                    work_.reset();
                }
                // Wait all outstanding work to be finished.
                pool_.join();
            }

            io_context &GetContext() { return io_; }

            thread_pool &GetPool() { return pool_; }

            strand_type &GetStrand() { return strand_; }

            void Stop()
            {
                io_.stop();
                pool_.stop();
                pool_.join();
            }
        };

        /****************************TimerManager*********************************/
        TimerManager::TimerManager(int32_t n_threads)
            : pImpl_{std::make_unique<Impl>(n_threads)} {};

        TimerManager::~TimerManager() {}

        TimerPtr TimerManager::AddTimer(
            const time_point &time_point,
            std::function<void(const std::error_code &)> handle)
        {
            std::shared_ptr<Timer> t = std::make_shared<Timer>();
            t->pImpl_->SetIoContext(pImpl_->GetContext());
            t->pImpl_->ExpiresAt(time_point);
            t->pImpl_->async_wait(pImpl_->GetStrand(), handle);
            return t;
        }

        TimerPtr TimerManager::AddTimer(const time_point &time_point)
        {
            std::shared_ptr<Timer> t = std::make_shared<Timer>();
            t->pImpl_->SetIoContext(pImpl_->GetContext());
            t->pImpl_->ExpiresAt(time_point);
            return t;
        }

        TimerPtr TimerManager::AddTimer(
            const duration &expiry_time_from_now,
            std::function<void(const std::error_code &)> handle)
        {
            std::shared_ptr<Timer> t = std::make_shared<Timer>();
            t->pImpl_->SetIoContext(pImpl_->GetContext());
            t->pImpl_->ExpiresFromNow(expiry_time_from_now);
            t->pImpl_->async_wait(pImpl_->GetStrand(), handle);
            return t;
        }

        TimerPtr TimerManager::AddTimer(const duration &expiry_time_from_now)
        {
            std::shared_ptr<Timer> t = std::make_shared<Timer>();
            t->pImpl_->SetIoContext(pImpl_->GetContext());
            t->pImpl_->ExpiresFromNow(expiry_time_from_now);
            return t;
        }

        void TimerManager::SetCallbackHandle(
            const TimerPtr &p_timer,
            std::function<void(const std::error_code &)> handle)
        {
            p_timer->pImpl_->async_wait(pImpl_->GetStrand(), handle);
        }

        std::size_t TimerManager::SetTimer(
            const TimerPtr &p_timer, const time_point &time_point,
            std::function<void(const std::error_code &)> handle)
        {
            auto n = p_timer->pImpl_->ExpiresAt(time_point);
            p_timer->pImpl_->async_wait(pImpl_->GetStrand(), handle);
            return n;
        }

        std::size_t TimerManager::SetTimer(
            const TimerPtr &p_timer, const duration &expiry_time_from_now,
            std::function<void(const std::error_code &)> handle)
        {
            auto n = p_timer->pImpl_->ExpiresFromNow(expiry_time_from_now);
            p_timer->pImpl_->async_wait(pImpl_->GetStrand(), handle);
            return n;
        }

        void TimerManager::AddDelayObject(const duration &duration)
        {
            std::shared_ptr<Timer> t = std::make_shared<Timer>();
            t->pImpl_->SetIoContext(pImpl_->GetContext());
            t->pImpl_->ExpiresFromNow(duration);
            t->pImpl_->wait();
        }

        void TimerManager::AddDelayObject(const time_point &time_point)
        {
            std::shared_ptr<Timer> t = std::make_shared<Timer>();
            t->pImpl_->SetIoContext(pImpl_->GetContext());
            t->pImpl_->ExpiresAt(time_point);
            t->pImpl_->wait();
        }

        void TimerManager::RemoveTimer(const TimerPtr &p_timer) { p_timer->Cancle(); }

        void TimerManager::Stop() { pImpl_->Stop(); }

    } // namespace timer
} // namespace ara
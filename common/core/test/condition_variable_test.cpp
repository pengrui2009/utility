/*
 * @Description: 
 * @Author: an.huang@tusen.ai
 * @Date: 2023-11-07 02:06:03
 * @LastEditors: an.huang
 * @LastEditTime: 2023-11-07 04:05:45
 * @FilePath: /root/adas-monorepo/modules/adaptive_autosar/ara-api/core/test/condition_variable_test.cpp
 * @version: 
 *  
 * Copyright (c) 2023 by Tusen, All Rights Reserved.
 */
#include <thread>
#include <cstdlib>
#include <gtest/gtest.h>

#include "ara/core/condition_variable.h"

using namespace ara::core;
using namespace std::literals::chrono_literals;

TEST(ConditionVariableTest, ConvertToTimepointTest) {
    ConditionVariable cv{};
    auto now = std::chrono::steady_clock::now();
    auto dur = 15min;
    auto time = cv.convert_to_timepoint(dur);
    auto sec_1 = std::chrono::time_point_cast<std::chrono::seconds>(now + dur);
    auto sec_2 = std::chrono::time_point_cast<std::chrono::seconds>(time);

    auto msec_1 = std::chrono::duration_cast<std::chrono::milliseconds>(now+dur-sec_1);
    auto msec_2 = std::chrono::duration_cast<std::chrono::milliseconds>(time-sec_2);

    ASSERT_EQ(sec_1.time_since_epoch().count(), sec_2.time_since_epoch().count());
    ASSERT_LT(abs(msec_1.count()-msec_2.count()), 1);

    now = std::chrono::steady_clock::now();
    auto dur_2 = 10ms;
    time = cv.convert_to_timepoint(dur_2);
    sec_1 = std::chrono::time_point_cast<std::chrono::seconds>(now + dur_2);
    sec_2 = std::chrono::time_point_cast<std::chrono::seconds>(time);

    msec_1 = std::chrono::duration_cast<std::chrono::milliseconds>(now+dur_2-sec_1);
    msec_2 = std::chrono::duration_cast<std::chrono::milliseconds>(time-sec_2);

    ASSERT_EQ(sec_1.time_since_epoch().count(), sec_2.time_since_epoch().count());
    ASSERT_LT(abs(msec_1.count()-msec_2.count()), 1);
}

TEST(ConditionVariableTest, WaitTest) {
    std::mutex mutex_{};
    ConditionVariable cv_{};
    std::atomic<bool> flag;
    flag.store(false);
    auto notify_time = std::chrono::steady_clock::now();
    auto wake_time = std::chrono::steady_clock::now();
    std::thread sub1([&](){
        std::unique_lock<std::mutex> lck(mutex_);
        cv_.wait(lck);
        flag.store(true);
        wake_time = std::chrono::steady_clock::now();
    });
    std::thread sub2([&](){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::unique_lock<std::mutex> lck(mutex_);
        lck.unlock();
        cv_.notify_one();
        notify_time = std::chrono::steady_clock::now();
    });

    sub1.detach();
    sub2.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    ASSERT_TRUE(flag.load());

    auto time_1 = std::chrono::time_point_cast<std::chrono::milliseconds>(notify_time);
    auto time_2 = std::chrono::time_point_cast<std::chrono::milliseconds>(wake_time);
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(time_2-time_1);

    ASSERT_LT(dur.count(),2);
}

TEST(ConditionVariableTest, WaitForTest) {
    std::mutex mutex_{};
    ConditionVariable cv_{};
    cv_status re1,re2;
    auto notify_time = std::chrono::steady_clock::now();
    auto wake_time = std::chrono::steady_clock::now();
    std::thread sub1([&](){
        std::unique_lock<std::mutex> lck(mutex_);
        re1 = cv_.wait_for(lck,std::chrono::milliseconds(100));
    });

    std::thread sub2([&](){
        std::unique_lock<std::mutex> lck(mutex_);
        re2 = cv_.wait_for(lck,std::chrono::seconds(2));
        wake_time = std::chrono::steady_clock::now();
    });

    std::thread sub3([&](){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::unique_lock<std::mutex> lck(mutex_);
        lck.unlock();
        cv_.notify_all();
        notify_time = std::chrono::steady_clock::now();
    });

    sub1.detach();
    sub2.detach();
    sub3.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    ASSERT_EQ(re1,cv_status::timeout);
    ASSERT_EQ(re2,cv_status::no_timeout);

    auto time_1 = std::chrono::time_point_cast<std::chrono::milliseconds>(notify_time);
    auto time_2 = std::chrono::time_point_cast<std::chrono::milliseconds>(wake_time);
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(time_2-time_1);
    
    ASSERT_LT(dur.count(),2);
}

TEST(ConditionVariableTest, WaitForWithPreTest) {
    std::mutex mutex_{};
    ConditionVariable cv_{};
    bool re1,re2;
    bool flag = true;
    auto notify_time = std::chrono::steady_clock::now();
    auto wake_time = std::chrono::steady_clock::now();
    std::thread sub1([&](){
        std::unique_lock<std::mutex> lck(mutex_);
        re1 = cv_.wait_for(lck,std::chrono::milliseconds(100),[&](){
            return flag;
        });
        flag = false;
    });

    std::thread sub2([&](){
        std::unique_lock<std::mutex> lck(mutex_);
        re2 = cv_.wait_for(lck,std::chrono::seconds(2),[&](){
            return flag;
        });
        wake_time = std::chrono::steady_clock::now();
    });

    std::thread sub3([&](){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::unique_lock<std::mutex> lck(mutex_);
        lck.unlock();
        cv_.notify_all();
        notify_time = std::chrono::steady_clock::now();
    });

    sub1.detach();
    sub2.detach();
    sub3.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    ASSERT_FALSE(re1);
    ASSERT_FALSE(re2);

    auto time_1 = std::chrono::time_point_cast<std::chrono::milliseconds>(notify_time);
    auto time_2 = std::chrono::time_point_cast<std::chrono::milliseconds>(wake_time);
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(time_2-time_1);
    
    ASSERT_LT(dur.count(),2);
}

TEST(ConditionVariableTest, WaitUntilTest) {
    std::mutex mutex_{};
    ConditionVariable cv_{};
    cv_status re1,re2;
    auto notify_time = std::chrono::steady_clock::now();
    auto wake_time = std::chrono::steady_clock::now();
    std::thread sub1([&](){
        std::unique_lock<std::mutex> lck(mutex_);
        re1 = cv_.wait_until(lck,wake_time+100ms);
    });

    std::thread sub2([&](){
        std::unique_lock<std::mutex> lck(mutex_);
        re2 = cv_.wait_until(lck,wake_time+3s);
        wake_time = std::chrono::steady_clock::now();
    });

    std::thread sub3([&](){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::unique_lock<std::mutex> lck(mutex_);
        lck.unlock();
        cv_.notify_all();
        notify_time = std::chrono::steady_clock::now();
    });

    sub1.detach();
    sub2.detach();
    sub3.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    ASSERT_EQ(re1,cv_status::timeout);
    ASSERT_EQ(re2,cv_status::no_timeout);

    auto time_1 = std::chrono::time_point_cast<std::chrono::milliseconds>(notify_time);
    auto time_2 = std::chrono::time_point_cast<std::chrono::milliseconds>(wake_time);
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(time_2-time_1);
    
    ASSERT_LT(dur.count(),2);
}

TEST(ConditionVariableTest, WaitUntilWithPreTest) {
    std::mutex mutex_{};
    ConditionVariable cv_{};
    bool re1,re2;
    bool flag = true;
    auto notify_time = std::chrono::steady_clock::now();
    auto wake_time = std::chrono::steady_clock::now();
    std::thread sub1([&](){
        std::unique_lock<std::mutex> lck(mutex_);
        re1 = cv_.wait_until(lck,wake_time+100ms,[&](){
            return flag;
        });
        flag = false;
    });

    std::thread sub2([&](){
        std::unique_lock<std::mutex> lck(mutex_);
        re2 = cv_.wait_until(lck,wake_time+3s,[&](){
            return flag;
        });
        wake_time = std::chrono::steady_clock::now();
    });

    std::thread sub3([&](){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::unique_lock<std::mutex> lck(mutex_);
        lck.unlock();
        cv_.notify_all();
        notify_time = std::chrono::steady_clock::now();
    });

    sub1.detach();
    sub2.detach();
    sub3.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    ASSERT_FALSE(re1);
    ASSERT_FALSE(re2);

    auto time_1 = std::chrono::time_point_cast<std::chrono::milliseconds>(notify_time);
    auto time_2 = std::chrono::time_point_cast<std::chrono::milliseconds>(wake_time);
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(time_2-time_1);
    
    ASSERT_LT(dur.count(),2);
}

TEST(ConditionVariableAnyTest, WaitTest) {
    std::mutex mutex_{};
    ConditionVariableAny cv_{};
    std::atomic<bool> flag;
    flag.store(false);
    auto notify_time = std::chrono::steady_clock::now();
    auto wake_time = std::chrono::steady_clock::now();
    std::thread sub1([&](){
        cv_.wait(mutex_);
        flag.store(true);
        wake_time = std::chrono::steady_clock::now();
    });
    std::thread sub2([&](){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        cv_.notify_one();
        notify_time = std::chrono::steady_clock::now();
    });

    sub1.detach();
    sub2.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    ASSERT_TRUE(flag.load());

    auto time_1 = std::chrono::time_point_cast<std::chrono::milliseconds>(notify_time);
    auto time_2 = std::chrono::time_point_cast<std::chrono::milliseconds>(wake_time);
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(time_2-time_1);

    ASSERT_LT(dur.count(),2);
}

TEST(ConditionVariableAnyTest, WaitForTest) {
    std::mutex mutex_{};
    ConditionVariableAny cv_{};
    cv_status re1,re2;
    auto notify_time = std::chrono::steady_clock::now();
    auto wake_time = std::chrono::steady_clock::now();
    std::thread sub1([&](){
        re1 = cv_.wait_for(mutex_,std::chrono::milliseconds(100));
    });

    std::thread sub2([&](){
        re2 = cv_.wait_for(mutex_,std::chrono::seconds(2));
        wake_time = std::chrono::steady_clock::now();
    });

    std::thread sub3([&](){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        cv_.notify_all();
        notify_time = std::chrono::steady_clock::now();
    });

    sub1.detach();
    sub2.detach();
    sub3.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    ASSERT_EQ(re1,cv_status::timeout);
    ASSERT_EQ(re2,cv_status::no_timeout);

    auto time_1 = std::chrono::time_point_cast<std::chrono::milliseconds>(notify_time);
    auto time_2 = std::chrono::time_point_cast<std::chrono::milliseconds>(wake_time);
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(time_2-time_1);
    
    ASSERT_LT(dur.count(),2);
}

TEST(ConditionVariableAnyTest, WaitForWithPreTest) {
    std::mutex mutex_{};
    ConditionVariableAny cv_{};
    bool re1,re2;
    bool flag = true;
    auto notify_time = std::chrono::steady_clock::now();
    auto wake_time = std::chrono::steady_clock::now();
    std::thread sub1([&](){
        re1 = cv_.wait_for(mutex_,std::chrono::milliseconds(100),[&](){
            return flag;
        });
        flag = false;
    });

    std::thread sub2([&](){
        re2 = cv_.wait_for(mutex_,std::chrono::seconds(2),[&](){
            return flag;
        });
        wake_time = std::chrono::steady_clock::now();
    });

    std::thread sub3([&](){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        cv_.notify_all();
        notify_time = std::chrono::steady_clock::now();
    });

    sub1.detach();
    sub2.detach();
    sub3.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    ASSERT_FALSE(re1);
    ASSERT_FALSE(re2);

    auto time_1 = std::chrono::time_point_cast<std::chrono::milliseconds>(notify_time);
    auto time_2 = std::chrono::time_point_cast<std::chrono::milliseconds>(wake_time);
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(time_2-time_1);
    
    ASSERT_LT(dur.count(),2);
}

TEST(ConditionVariableAnyTest, WaitUntilTest) {
    std::mutex mutex_{};
    ConditionVariableAny cv_{};
    cv_status re1,re2;
    auto notify_time = std::chrono::steady_clock::now();
    auto wake_time = std::chrono::steady_clock::now();
    std::thread sub1([&](){
        re1 = cv_.wait_until(mutex_,wake_time+100ms);
    });

    std::thread sub2([&](){
        re2 = cv_.wait_until(mutex_,wake_time+3s);
        wake_time = std::chrono::steady_clock::now();
    });

    std::thread sub3([&](){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        cv_.notify_all();
        notify_time = std::chrono::steady_clock::now();
    });

    sub1.detach();
    sub2.detach();
    sub3.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    ASSERT_EQ(re1,cv_status::timeout);
    ASSERT_EQ(re2,cv_status::no_timeout);

    auto time_1 = std::chrono::time_point_cast<std::chrono::milliseconds>(notify_time);
    auto time_2 = std::chrono::time_point_cast<std::chrono::milliseconds>(wake_time);
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(time_2-time_1);
    
    ASSERT_LT(dur.count(),2);
}

TEST(ConditionVariableAnyTest, WaitUntilWithPreTest) {
    std::mutex mutex_{};
    ConditionVariableAny cv_{};
    bool re1,re2;
    bool flag = true;
    auto notify_time = std::chrono::steady_clock::now();
    auto wake_time = std::chrono::steady_clock::now();
    std::thread sub1([&](){
        re1 = cv_.wait_until(mutex_,wake_time+100ms,[&](){
            return flag;
        });
        flag = false;
    });

    std::thread sub2([&](){
        re2 = cv_.wait_until(mutex_,wake_time+3s,[&](){
            return flag;
        });
        wake_time = std::chrono::steady_clock::now();
    });

    std::thread sub3([&](){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        cv_.notify_all();
        notify_time = std::chrono::steady_clock::now();
    });

    sub1.detach();
    sub2.detach();
    sub3.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    ASSERT_FALSE(re1);
    ASSERT_FALSE(re2);

    auto time_1 = std::chrono::time_point_cast<std::chrono::milliseconds>(notify_time);
    auto time_2 = std::chrono::time_point_cast<std::chrono::milliseconds>(wake_time);
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(time_2-time_1);
    
    ASSERT_LT(dur.count(),2);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
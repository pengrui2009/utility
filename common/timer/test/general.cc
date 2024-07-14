/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-10-25 11:12:34
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-01-13 17:21:42
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include <ara/timer/TimerManager.h>
#include <ara/timer/timer.h>
#include <gtest/gtest.h>

#include <atomic>
#include <iostream>
#include <thread>

using namespace ara::timer;
using namespace std::chrono_literals;

void handler(const std::error_code& e, int32_t* a) {
  if (e.value() > 0) {
    std::cout << e.message() << std::endl;
  } else {
    ++(*a);
  }
}

void handler_persion(const std::error_code& e, time_point* end_time,
                     std::atomic_bool* done) {
  if (e.value() > 0) {
    std::cout << e.message() << std::endl;
  } else {
    *end_time = std::chrono::system_clock::now();
  }
  *done = true;
}

void handler_reuse(const std::error_code& e, TimerPtr p_timer,
                   TimerManager* manager, int* a) {
  if (e.value() > 0) {
    std::cout << e.message() << std::endl;
  } else {
    if (*a < 2) {
      (*a)++;
      manager->SetTimer(
          p_timer, 1ms,
          std::bind(handler_reuse, std::placeholders::_1, p_timer, manager, a));
    }
  }
}

TEST(TIMERMANAGER, sigle_thread) {
  TimerManager manager;
  int32_t a{0};
  manager.AddDelayObject(1ms);
  {
    auto tm1 =
        manager.AddTimer(1s, std::bind(handler, std::placeholders::_1, &a));
  }
  auto tm2 = manager.AddTimer(std::chrono::system_clock::now() + 2ms,
                              std::bind(handler, std::placeholders::_1, &a));
  auto tm3 =
      manager.AddTimer(1ms, std::bind(handler, std::placeholders::_1, &a));
  while (a < 2)
    ;
  EXPECT_EQ(a, 2);
}

TEST(TIMERMANAGER, multi_threads) {
  TimerManager manager(4);
  int32_t a{0};
  manager.AddDelayObject(std::chrono::system_clock::now() + 1ms);

  auto tm1 =
      manager.AddTimer(1ms, std::bind(handler, std::placeholders::_1, &a));
  auto tm2 = manager.AddTimer(std::chrono::system_clock::now() + 1ms,
                              std::bind(handler, std::placeholders::_1, &a));
  auto tm3 =
      manager.AddTimer(1ms, std::bind(handler, std::placeholders::_1, &a));
  auto tm4 = manager.AddTimer(std::chrono::system_clock::now() + 1ms,
                              std::bind(handler, std::placeholders::_1, &a));
  auto tm5 =
      manager.AddTimer(1ms, std::bind(handler, std::placeholders::_1, &a));
  while (a < 5)
    ;
  EXPECT_EQ(a, 5);
}

TEST(TIMERMANAGER, TimerCancle) {
  TimerManager manager;

  auto tm1 = manager.AddTimer(1s, [](const std::error_code& e) {});
  auto tm2 = manager.AddTimer(1s, [](const std::error_code& e) {});
  EXPECT_EQ(1, tm1->Cancle());
  EXPECT_EQ(1, tm2->Cancle());
}

TEST(TIMERMANAGER, TimerExpiry) {
  TimerManager manager;
  auto now = std::chrono::system_clock::now();
  auto tm1 = manager.AddTimer(now + 1s, [](const std::error_code& e) {});
  std::time_t start = std::chrono::system_clock::to_time_t(now);
  std::time_t end = std::chrono::system_clock::to_time_t(tm1->Expiry());
  EXPECT_EQ(1, end - start);
  EXPECT_EQ(1, tm1->Cancle());
}

TEST(TIMERMANAGER, Persion) {
  TimerManager manager;
  std::atomic_bool done{false};
  auto now = std::chrono::system_clock::now();
  std::chrono::system_clock::time_point end;
  manager.AddDelayObject(1ms);
  end = std::chrono::system_clock::now();
  auto diff =
      std::chrono::duration_cast<std::chrono::microseconds>(end - now).count();
  EXPECT_GE(diff, 1000);
  now = std::chrono::system_clock::now();
  auto tm1 = manager.AddTimer(
      now + 1ms,
      std::bind(handler_persion, std::placeholders::_1, &end, &done));
  while (!done)
    ;
  diff =
      std::chrono::duration_cast<std::chrono::microseconds>(end - now).count();
  EXPECT_GE(diff, 1000);
}

TEST(TIMERMANAGER, Remove) {
  TimerManager manager;
  int32_t a{0};
  auto tm1 =
      manager.AddTimer(1s, std::bind(handler, std::placeholders::_1, &a));
  auto tm2 = manager.AddTimer(std::chrono::system_clock::now() + 2ms,
                              std::bind(handler, std::placeholders::_1, &a));
  auto tm3 =
      manager.AddTimer(1ms, std::bind(handler, std::placeholders::_1, &a));
  manager.RemoveTimer(tm1);
  while (a < 2)
    ;
  EXPECT_EQ(a, 2);
}

TEST(TIMERMANAGER, Reuse) {
  TimerManager manager;
  int32_t a{0};
  int32_t b{0};
  auto tm1 = manager.AddTimer(1ms);
  manager.SetCallbackHandle(
      tm1, std::bind(handler_reuse, std::placeholders::_1, tm1, &manager, &a));
  while (a < 2)
    ;
  auto tm2 = manager.AddTimer(std::chrono::system_clock::now() + 1ms);
  manager.SetCallbackHandle(
      tm2, std::bind(handler_reuse, std::placeholders::_1, tm1, &manager, &b));
  while (b < 2)
    ;
  EXPECT_EQ(b, 2);
}

TEST(TIMERMANAGER, Stop) {
  int32_t a{0};
  {
    TimerManager manager(1);
    auto tm1 = manager.AddTimer(1ms);
    manager.SetCallbackHandle(tm1, [&](const std::error_code& e) {
      if (e.value() > 0) {
        std::cout << e.message() << std::endl;
      } else {
        std::this_thread::sleep_for(50ms);
        a++;
      }
    });
    auto tm2 = manager.AddTimer(1ms);
    /* This handle will be queued */
    manager.SetCallbackHandle(tm2, [&](const std::error_code& e) {
      if (e.value() > 0) {
        std::cout << e.message() << std::endl;
      } else {
        std::this_thread::sleep_for(50ms);
        a++;
      }
    });
    /* Sleep to make all timers fire. */
    std::this_thread::sleep_for(3ms);
    /* Only the first completion handler will be
     * invoked when Stop is called. */
    manager.Stop();
  }
  EXPECT_EQ(a, 1);
}

TEST(TIMERMANAGER, AutoCancle) {
  int32_t a{0};
  {
    TimerManager manager(1);
    auto tm1 = manager.AddTimer(1s);
    manager.SetCallbackHandle(tm1, [&](const std::error_code& e) {
      if (e.value() > 0) {
        std::cout << e.message() << std::endl;
      } else {
        a++;
      }
    });
    auto tm2 = manager.AddTimer(1s);
    /* This handle will also be invoded */
    manager.SetCallbackHandle(tm2, [&](const std::error_code& e) {
      if (e.value() > 0) {
        std::cout << e.message() << std::endl;
      } else {
        a++;
      }
    });
  }
  EXPECT_EQ(a, 0);
}

int main(int argc, char** argv) {
  try {
    ::testing::InitGoogleTest(&argc, argv);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  } catch (...) {
  }
  return RUN_ALL_TESTS();
}
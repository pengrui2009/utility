/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-09 16:34:37
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-08-09 17:43:28
 * @FilePath: /aeg-adaptive-autosar/ara-api/common/threadpool/test/threadpool_test.cc
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include <gtest/gtest.h>
#include <iostream>
#include "ara/threadpool/thread_pool.h"

int testF(int in) { return in * in; }
ara::threadpool::ThreadPool tp(4);

class TestTp1 {
 public:
  ara::threadpool::ThreadPool* tp;
  TestTp1() { tp = new ara::threadpool::ThreadPool(2); }
  ~TestTp1() { delete tp; }
};

class TestTp2 {
 public:
  std::unique_ptr<ara::threadpool::ThreadPool> tp;
  TestTp2() : tp(std::move(std::unique_ptr<ara::threadpool::ThreadPool>(new ara::threadpool::ThreadPool(3)))) {}
  ~TestTp2(){};
};

TEST(THREADPOOL, Lambda) {
  auto f1 = tp.enqueue([]() { std::cout << "test" << std::endl; });
  auto f3 = tp.enqueue(
      [](int a) {
        if (a > 5) return true;
      },
      10);
  EXPECT_TRUE(f3.get());
}

TEST(THREADPOOL, Function) {
  std::function<int(int)> f1 = testF;
  auto f2 = tp.enqueue(f1, 10);
  EXPECT_EQ(100, f2.get());
}

TEST(THREADPOOL, Class) {
  TestTp1 tp1;
  auto f1 = tp1.tp->enqueue(testF, 5);
  TestTp2 tp2;
  auto f2 = tp2.tp->enqueue(testF, 6);
  EXPECT_EQ(25, f1.get());
  EXPECT_EQ(36, f2.get());
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
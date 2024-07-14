
  


/**
 * @file
 */

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include "ara/core/promise.h"

using namespace ara::core;

TEST(PromiseTest, SimpleTest) {
  // SWS_CORE_00341
  Promise<int> tmp1;
  // SWS_CORE_00352
  Promise<int> tmp2(std::move(tmp1));
  // SWS_CORE_00343
  Promise<int> tmp3 = std::move(tmp2);
  //
  {
    Promise<int> tmp;
    Result<int> res(1);
    auto futt = tmp.get_future();
    auto fut = std::move(futt);
    tmp.SetResult(res);
    fut.wait();
    auto r = fut.GetResult();
    ASSERT_TRUE(r);
    ASSERT_EQ(r.Value(), 1);
  }
  {
    auto lam = [](){
      Promise<int> tmp;
      tmp.SetResult(Result<int>(2));
      auto fut = tmp.get_future();
      return fut;
    };
    auto fut = lam();
    fut.wait();
    ASSERT_EQ(fut.GetResult(), 2);
  }
  {
    Future<int> tmp;
  }
  {
    Promise<int> tmp;
    auto fut = tmp.get_future();
  }
  {
    Promise<int> tmp;
    auto fut = tmp.get_future();
    tmp.SetResult(Result<int>(2));
    fut.wait();
    ASSERT_EQ(fut.GetResult(), 2);
  }
  {
    Promise<int> tmp;
    auto fut = tmp.get_future();
    int i = 3;
    tmp.set_value(i);
    fut.wait();
    ASSERT_EQ(fut.GetResult(), 3);
  }
  {
    Promise<int> tmp;
    auto fut = tmp.get_future();
    tmp.set_value(4);
    fut.wait();
    ASSERT_EQ(fut.GetResult(), 4);
  }
  {
    Promise<int> t;
    Promise<int> tmp = std::move(t);
    auto fut = tmp.get_future();
    tmp.set_value(4);
    fut.wait();
    ASSERT_EQ(fut.GetResult(), 4);
  }
  {
    Promise<int> tmp;
    auto core_errc = ErrorCode(CoreErrc::no_such_file_or_directory);
    auto fut = tmp.get_future();
    tmp.SetError(core_errc);
    fut.wait();
    ASSERT_EQ(fut.GetResult(), core_errc);
  }
  {
    Promise<int> tmp;
    auto core_errc = ErrorCode(CoreErrc::no_such_file_or_directory);
    auto fut = tmp.get_future();
    tmp.SetError(ErrorCode(CoreErrc::no_such_file_or_directory));
    fut.wait();
    ASSERT_EQ(fut.GetResult(), core_errc);
  }
  {
    Promise<int> tt;
    auto tmp = std::move(tt);
    auto core_errc = ErrorCode(CoreErrc::no_such_file_or_directory);
    {
      auto fut = tmp.get_future();
    }
    tmp.SetError(core_errc);
    ASSERT_EQ(tmp.core_->count_, 1);
  }
  {
    Promise<int> t;
    Promise<int> tmp = std::move(t);
    auto fut = tmp.get_future();
    auto fff = std::move(fut);
    tmp.set_value(4);
    fff.wait();
    ASSERT_EQ(fff.GetResult(), 4);
  }
  {
    auto func = [](){
        Promise<int> p;
        p.set_value(4);
        return p.get_future();
    };
    auto fut = func();
    ASSERT_EQ(fut.GetResult(), 4);

    auto func1 = [](){
        Promise<int> p;
        p.set_value(4);
        return p.get_future();
    };
    auto fut1 = func1();
    ASSERT_EQ(fut1.GetResult(), 4);
  }
  {
    Promise<int> tmp;
    Result<int> res(1);
    tmp.SetResult(res);
    int i = 0;
    {
      tmp.get_future([&i](){i = 1;});
    }

    ASSERT_EQ(i, 1);
  }
  {
    Promise<int> tmp;
    Result<int> res(1);
    tmp.SetResult(res);
    ASSERT_THROW(tmp.SetResult(res), FutureException);
  }
  {
    Promise<int> tmp;
    tmp.get_future();
    ASSERT_THROW(tmp.get_future([](){}), FutureException);
  }
  {
    Promise<int> tmp;
    tmp.get_future([](){});
    ASSERT_THROW(tmp.get_future(), FutureException);
  }
  {
    Promise<int> tmp;
    tmp.get_future([](){});
    ASSERT_THROW(tmp.get_future([](){}), FutureException);
  }
  {
    Promise<int> tmp;
    tmp.get_future();
    ASSERT_THROW(tmp.get_future(), FutureException);
  }
  {
    Future<int> tmp;
    ASSERT_TRUE(tmp.GetResult().Error() == future_errc::no_state);
  }
  {
    auto lam = [](){
      Promise<int> tmp;
      auto fut = tmp.get_future();
      return fut;
    };
    auto fut = lam();
    ASSERT_TRUE(fut.GetResult().Error() == future_errc::broken_promise);
  }
}

TEST(PromiseTest, SimpleTestThen) {
  {
    Promise<int> tmp;
    Result<int> res(1);
    auto fut = tmp.get_future();
    tmp.SetResult(res);
    auto futt =
        fut.then(
            [](const Result<int>& res) { return res; }).then(
            [](const Result<int>& res) { return *res; }).then(
            [](const Result<int>& res) { return std::to_string(*res);}).then(
            [](const Result<std::string>& res) {return res;}).then(
            [](const Result<std::string>& res) {return;}).then(
            [](const Result<void>& res) {return 1;});
  }
}

TEST(PromiseTest, SimpleTestVoid) {
  {
    Promise<void> tmp;
    auto core_errc = ErrorCode(CoreErrc::no_such_file_or_directory);
    tmp.SetError(core_errc);
    ASSERT_EQ(tmp.get_future().GetResult(), core_errc);
  }
  {
    Promise<void> tmp;
    auto core_errc = ErrorCode(CoreErrc::no_such_file_or_directory);
    tmp.SetError(ErrorCode(CoreErrc::no_such_file_or_directory));
    ASSERT_EQ(tmp.get_future().GetResult(), core_errc);
  }
  {
    Promise<void> tmp;
    auto core_errc = ErrorCode(CoreErrc::no_such_file_or_directory);
    Result<void> res(core_errc);
    tmp.SetResult(res);
    ASSERT_EQ(tmp.get_future().GetResult(), core_errc);
  }
  {
    Promise<void> tmp;
    auto core_errc = ErrorCode(CoreErrc::no_such_file_or_directory);
    Result<void> res(core_errc);
    tmp.SetResult(std::move(res));
    ASSERT_EQ(tmp.get_future().GetResult(), core_errc);
  }
  {
    Promise<void> tmp;
    tmp.set_value();

    ASSERT_TRUE(tmp.get_future().GetResult());
  }
  {
    Promise<void> tmp;
    Result<void> res;
    tmp.SetResult(res);

    ASSERT_TRUE(tmp.get_future().GetResult());
  }
  {
    Promise<void> tmp;
    Result<void> res;
    tmp.SetResult(res);
    int i = 0;
    {
      tmp.get_future([&i](){i = 1;});
    }

    ASSERT_EQ(i, 1);
  }
  {
    Promise<void> tmp;
    Result<void> res;
    tmp.SetResult(res);
    ASSERT_THROW(tmp.SetResult(res), FutureException);
  }
  {
    Promise<void> tmp;
    tmp.get_future();
    ASSERT_THROW(tmp.get_future([](){}), FutureException);
  }
  {
    Promise<void> tmp;
    tmp.get_future([](){});
    ASSERT_THROW(tmp.get_future(), FutureException);
  }
  {
    Promise<void> tmp;
    tmp.get_future([](){});
    ASSERT_THROW(tmp.get_future([](){}), FutureException);
  }
  {
    Promise<void> tmp;
    tmp.get_future();
    ASSERT_THROW(tmp.get_future(), FutureException);
  }
  {
    Future<void> tmp;
    ASSERT_TRUE(tmp.GetResult().Error() == future_errc::no_state);
  }
  {
    auto lam = [](){
      Promise<void> tmp;
      auto fut = tmp.get_future();
      return fut;
    };
    auto fut = lam();
    ASSERT_TRUE(fut.GetResult().Error() == future_errc::broken_promise);
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

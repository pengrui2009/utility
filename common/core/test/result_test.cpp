

/**
 * @file
 */

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>

#include "ara/core/core_error_domain.h"
#include "ara/core/error_code.h"
#include "ara/core/future_error_domain.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/vector.h"

using namespace ara::core;

TEST(ResultTest, NoVoidTest) {
  // SWS_CORE_00701 Skip
  // SWS_CORE_00711 Skip
  // SWS_CORE_00712 Skip
  // SWS_CORE_00722
  Result<int> result_value(1);
  ASSERT_EQ(result_value, 1);
  // SWS_CORE_00725
  Result<int> result_value_1(result_value);
  ASSERT_EQ(result_value_1, 1);
  // SWS_CORE_00726 SWS_CORE_00732
  Result<int> result_value_2(Result<int>::FromValue(1));
  ASSERT_EQ(result_value_2, 1);
  // SWS_CORE_00721
  int tmp = 1;
  Result<int> result_value_3(tmp);
  ASSERT_EQ(result_value_3, 1);
  // SWS_CORE_00723
  auto core_errc = ErrorCode(CoreErrc::no_such_file_or_directory);
  Result<int> result_error(core_errc);
  ASSERT_EQ(result_error, core_errc);
  // SWS_CORE_00724
  Result<int> result_error_1(ErrorCode(CoreErrc::no_such_file_or_directory));
  ASSERT_EQ(result_error_1, core_errc);
  // SWS_CORE_00731
  ASSERT_EQ(Result<int>::FromValue(tmp), 1);
  // SWS_CORE_00732
  ASSERT_EQ(Result<int>::FromValue(1), 1);
  // SWS_CORE_00733
  ASSERT_EQ(Result<Vector<int>>::FromValue(1, 1), (Vector<int>{1}));
  ASSERT_EQ(Result<Vector<int>>::FromValue({1, 1}), (Vector<int>{1, 1}));
  // SWS_CORE_00734
  ASSERT_EQ(Result<int>::FromError(core_errc), core_errc);
  // SWS_CORE_00735
  ASSERT_EQ(Result<int>::FromError(ErrorCode(CoreErrc::no_such_file_or_directory)), ErrorCode(CoreErrc::no_such_file_or_directory)); 
  // SWS_CORE_00736
  ASSERT_EQ(Result<int>::FromError(CoreErrc::no_such_file_or_directory), ErrorCode(CoreErrc::no_such_file_or_directory));
  // SWS_CORE_00742
  auto result_from_value_move = Result<int>::FromValue(1);
  ASSERT_EQ(result_from_value_move, 1);
  // SWS_CORE_00741
  auto result_from_value_copy = result_from_value_move;
  ASSERT_EQ(result_from_value_copy, 1);
  // SWS_CORE_00743
  result_value_1.EmplaceValue(2);
  ASSERT_EQ(result_value_1, 2);
  // SWS_CORE_00744
  result_value_1.EmplaceError(CoreErrc::no_such_file_or_directory);
  ASSERT_EQ(result_value_1, ErrorCode(CoreErrc::no_such_file_or_directory));
  // SWS_CORE_00745
  result_value_1.Swap(result_value_2);
  ASSERT_EQ(result_value_1, 1);
  // SWS_CORE_00752
  ASSERT_TRUE(result_value);
  ASSERT_FALSE(result_error);
  // SWS_CORE_00751
  ASSERT_TRUE(result_value.HasValue());
  ASSERT_FALSE(result_error.HasValue());
  // SWS_CORE_00753
  auto result_value_00753 = result_value;
  ASSERT_EQ(*result_value_00753, 1);
  ASSERT_EQ(*(std::move(result_value_00753)), 1);
  // SWS_CORE_00754
  auto str_tmp = Result<String>::FromValue(3, 'A');
  ASSERT_STREQ(str_tmp->c_str(), "AAA");
  auto str = Result<String>::FromValue("123");
  ASSERT_STREQ(str->c_str(), "123");
  // SWS_CORE_00755
  const int& tmp_value = result_value.Value();
  ASSERT_EQ(tmp_value, 1);
  // SWS_CORE_00756
  auto result_value_00756 = result_value;
  int tmp_value_1 = std::move(result_value_00756).Value();
  ASSERT_EQ(tmp_value_1, 1);
  // SWS_CORE_00757
  const ErrorCode& tmp_error = result_error.Error();
  ASSERT_EQ(tmp_error, ErrorCode(CoreErrc::no_such_file_or_directory));
  // SWS_CORE_00758
  auto result_error_00758 = result_error;
  ErrorCode tmp_error_1 = std::move(result_error_00758).Error();
  ASSERT_EQ(tmp_error_1, ErrorCode(CoreErrc::no_such_file_or_directory));
  // SWS_CORE_00770
  const auto& result_value_ok_1 = result_value.Ok();
  const auto& result_error_ok_1 = result_error.Ok();
  ASSERT_TRUE(result_value_ok_1 == 1);
  ASSERT_TRUE(result_error_ok_1 == nullopt);
  // SWS_CORE_00771
  auto result_value_00771 = result_value;
  auto result_error_00771 = result_error;
  ASSERT_TRUE(std::move(result_value_00771).Ok() == 1);
  ASSERT_TRUE(std::move(result_error_00771).Ok() == nullopt);
  // SWS_CORE_00773
  const auto& result_value_err_1 = result_value.Err();
  const auto& result_error_err_1 = result_error.Err();
  ASSERT_TRUE(result_value_err_1 == nullopt);
  ASSERT_TRUE(result_error_err_1 == core_errc);
  // SWS_CORE_00773
  auto result_value_00773 = result_value;
  auto result_error_00773 = result_error;
  ASSERT_TRUE(std::move(result_value_00773).Err() == nullopt);
  ASSERT_TRUE(std::move(result_error_00773).Err() == core_errc);
  // SWS_CORE_00761
  const auto& result_value_value_or = result_value.ValueOr(0);
  ASSERT_EQ(result_value_value_or, 1);
  // SWS_CORE_00762
  auto result_value_00762 = result_value;
  auto result_error_00762 = result_error;
  ASSERT_EQ(std::move(result_value_00762).ValueOr(0), 1);
  ASSERT_EQ(std::move(result_error_00762).ValueOr(1), 1);
  // SWS_CORE_00763
  ASSERT_EQ(result_value.ErrorOr(CoreErrc::connection_aborted), ErrorCode(CoreErrc::connection_aborted));
  ASSERT_EQ(result_error.ErrorOr(CoreErrc::connection_aborted), core_errc);
  // SWS_CORE_00765
  ASSERT_TRUE(result_error.CheckError(ErrorCode(CoreErrc::no_such_file_or_directory)));
  ASSERT_FALSE(result_value.CheckError(ErrorCode(CoreErrc::no_such_file_or_directory)));
  // SWS_CORE_00766
  const auto& result_value_value_or_throw = result_value.ValueOrThrow();
  ASSERT_EQ(result_value_value_or_throw, 1);
  // SWS_CORE_00769
  auto result_value_00769 = result_value;
  ASSERT_EQ(std::move(result_value_00769).ValueOrThrow(), 1);
  // SWS_CORE_00767 Skip
  // SWS_CORE_00768 Skip
  // SWS_CORE_00796
  swap(result_value_1, result_value_2);
  ASSERT_EQ(result_value_2, 1);
  // SWS_CORE_00780
  ASSERT_TRUE(Result<int>::FromValue(1) == 1);
  // SWS_CORE_00781
  ASSERT_TRUE(Result<int>::FromValue(2) != 1);
  // SWS_CORE_00782
  ASSERT_TRUE(1 == Result<int>::FromValue(1));
  // SWS_CORE_00783
  ASSERT_TRUE(2 != Result<int>::FromValue(1));
  // SWS_CORE_00784
  ASSERT_TRUE(Result<int>::FromError(ErrorCode(CoreErrc::no_such_file_or_directory)) == ErrorCode(CoreErrc::no_such_file_or_directory));
  // SWS_CORE_00785
  ASSERT_TRUE(Result<int>::FromError(ErrorCode(CoreErrc::no_such_file_or_directory)) != ErrorCode(CoreErrc::network_reset));
  // SWS_CORE_00786
  ASSERT_TRUE(ErrorCode(CoreErrc::no_such_file_or_directory) == Result<int>::FromError(ErrorCode(CoreErrc::no_such_file_or_directory)));
  // SWS_CORE_00787
  ASSERT_TRUE(ErrorCode(CoreErrc::network_reset) != Result<int>::FromError(ErrorCode(CoreErrc::no_such_file_or_directory)));
  // SWS_CORE_00788
  ASSERT_TRUE(Result<int>::FromValue(1) == Result<int>::FromValue(1));
  // SWS_CORE_00789
  ASSERT_TRUE(Result<int>::FromValue(1) != Result<int>::FromValue(2));

}

TEST(ResultTest, VoidTest) {
  // SWS_CORE_00801 Skip
  // SWS_CORE_00811 Skip
  // SWS_CORE_00812 Skip
  // SWS_CORE_00821
  {
    Result<void> tmp;
    Result<void> tmp1 = tmp;
    Result<void> tmp2 = std::move(tmp);
    tmp2.Value();
  }
  Result<void> result_value_empty;
  // SWS_CORE_00823
  auto core_errc = ErrorCode(CoreErrc::no_such_file_or_directory);
  Result<void> result_error(core_errc);
  ASSERT_EQ(result_error, core_errc);
  // SWS_CORE_00824
  Result<void> result_error_1(ErrorCode(CoreErrc::no_such_file_or_directory));
  ASSERT_EQ(result_error_1, core_errc);
  // SWS_CORE_00831
  //ASSERT_EQ(Result<void>::FromValue(), Result<void>());
  // SWS_CORE_00834
  ASSERT_EQ(Result<void>::FromError(core_errc), core_errc);
  // SWS_CORE_00835
  ASSERT_EQ(Result<void>::FromError(ErrorCode(CoreErrc::no_such_file_or_directory)), ErrorCode(CoreErrc::no_such_file_or_directory)); 
  // SWS_CORE_00836 Skip
  // SWS_CORE_00842
  auto result_from_error_move = Result<void>::FromError(ErrorCode(CoreErrc::no_such_file_or_directory));
  ASSERT_EQ(result_from_error_move, ErrorCode(CoreErrc::no_such_file_or_directory));
  // SWS_CORE_00841
  auto result_from_error_copy = result_from_error_move;
  ASSERT_EQ(result_from_error_copy, ErrorCode(CoreErrc::no_such_file_or_directory));
  // SWS_CORE_00843
  //result_error_1.EmplaceValue(result_value_empty.Value());
  //ASSERT_EQ(result_error_1, Result<void>());
  // SWS_CORE_00844
  result_error_1.EmplaceError(ErrorCode(CoreErrc::no_such_file_or_directory));
  ASSERT_EQ(result_error_1, ErrorCode(CoreErrc::no_such_file_or_directory));
  // SWS_CORE_00845
  result_value_empty.Swap(result_error);
  ASSERT_EQ(result_value_empty, ErrorCode(CoreErrc::no_such_file_or_directory));
  result_value_empty.Swap(result_error);
  // SWS_CORE_00852
  ASSERT_FALSE(result_error);
  ASSERT_TRUE(result_value_empty);
  // SWS_CORE_00851
  ASSERT_FALSE(result_error.HasValue());
  ASSERT_TRUE(result_value_empty.HasValue());
  // SWS_CORE_00856
  //auto tmp_value_1 = result_value_empty.Value();
  //ASSERT_EQ(tmp_value_1, Result<void>());
  // SWS_CORE_00857
  const ErrorCode& tmp_error = result_error.Error();
  ASSERT_EQ(tmp_error, ErrorCode(CoreErrc::no_such_file_or_directory));
  // SWS_CORE_00858
  auto result_error_00858 = result_error;
  ErrorCode tmp_error_1 = std::move(result_error_00858).Error();
  ASSERT_EQ(tmp_error_1, ErrorCode(CoreErrc::no_such_file_or_directory));
  // SWS_CORE_00868
  const auto& result_error_err_1 = result_error.Err();
  ASSERT_TRUE(result_error_err_1 == core_errc);
  // SWS_CORE_00869
   auto result_error_00869 = result_error;
  ASSERT_TRUE(std::move(result_error_00869).Err() == core_errc);
  // SWS_CORE_00863
  ASSERT_EQ(result_value_empty.ErrorOr(CoreErrc::connection_aborted), ErrorCode(CoreErrc::connection_aborted));
  ASSERT_EQ(result_error.ErrorOr(CoreErrc::connection_aborted), core_errc);
  // SWS_CORE_00865
  ASSERT_TRUE(result_error.CheckError(ErrorCode(CoreErrc::no_such_file_or_directory)));
  ASSERT_FALSE(result_value_empty.CheckError(ErrorCode(CoreErrc::no_such_file_or_directory)));
  // SWS_CORE_00866
  //ASSERT_EQ(result_value_empty.ValueOrThrow(), Result<void>());
  // SWS_CORE_00867 Skip
  // SWS_CORE_00868 Skip
  ASSERT_EQ(Result<void>::FromError(ErrorCode(CoreErrc::no_such_file_or_directory)), ErrorCode(CoreErrc::no_such_file_or_directory));

  ASSERT_FALSE(Result<void>::FromError(ErrorCode(CoreErrc::no_such_file_or_directory)));

  ASSERT_TRUE(Result<void>());
  
}
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

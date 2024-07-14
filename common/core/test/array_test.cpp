

/**
 * @file
 */

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include "ara/core/array.h"

TEST(ArrayTest, SimpleTestInt) {
    ara::core::Array<int, 3> arr1{1, 2, 3};
    ara::core::Array<int, 3> arr2{1, 1, 1};
    ara::core::Array<int, 3> arr1_bak{1, 2, 3};
    // SWS_CORE_01201 - SWS_CORE_01272 Skip
    // SWS_CORE_01290
    ASSERT_FALSE(arr1 == arr2);
    // SWS_CORE_01291
    ASSERT_TRUE(arr1 != arr2);
    // SWS_CORE_01292 
    ASSERT_FALSE(arr1 < arr2);
    // SWS_CORE_01293
    ASSERT_TRUE(arr1 > arr2);
    // SWS_CORE_01294
    ASSERT_FALSE(arr1 <= arr2);
    // SWS_CORE_01295
    ASSERT_TRUE(arr1 >= arr2);
    // SWS_CORE_01296
    arr2.swap(arr1);
    ASSERT_TRUE(arr2 == arr1_bak);
    ASSERT_TRUE(arr1 != arr1_bak);
    // SWS_CORE_01280 - SWS_CORE_01284 Skip
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

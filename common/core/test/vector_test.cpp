

/**
 * @file
 */

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include "ara/core/vector.h"

TEST(ArrayTest, SimpleTest) {
    ara::core::Vector<int> arr1{1, 2, 3};
    ara::core::Vector<int> arr2{1, 1, 1};
    ara::core::Vector<int> arr1_bak{1, 2, 3};
    // SWS_CORE_01390
    ASSERT_FALSE(arr1 == arr2);
    // SWS_CORE_01391
    ASSERT_TRUE(arr1 != arr2);
    // SWS_CORE_01392 
    ASSERT_FALSE(arr1 < arr2);
    // SWS_CORE_01393
    ASSERT_FALSE(arr1 <= arr2);
    // SWS_CORE_01394
    ASSERT_TRUE(arr1 > arr2);
    // SWS_CORE_01395
    ASSERT_TRUE(arr1 >= arr2);
    // SWS_CORE_01396
    arr2.swap(arr1);
    ASSERT_TRUE(arr2 == arr1_bak);
    ASSERT_TRUE(arr1 != arr1_bak);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

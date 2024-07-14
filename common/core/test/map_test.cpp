

/**
 * @file
 */

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include "ara/core/map.h"

using namespace ara::core;

TEST(ArrayTest, SimpleTest) {
    Map<int, int> m1;
    Map<int, int> m2;
    m1.emplace(1, 1);
    m2.emplace(2, 2);
    ASSERT_EQ(m1.count(1), 1);
    ASSERT_EQ(m1.count(2), 0);
    ASSERT_EQ(m2.count(2), 1);
    ASSERT_EQ(m2.count(1), 0);
    ASSERT_EQ(m1[1], 1);
    ASSERT_EQ(m2[2], 2);
    // SWS_CORE_01496
    swap(m1, m2);
    ASSERT_EQ(m1.count(2), 1);
    ASSERT_EQ(m1.count(1), 0);
    ASSERT_EQ(m2.count(1), 1);
    ASSERT_EQ(m2.count(2), 0);
    ASSERT_EQ(m1[2], 2);
    ASSERT_EQ(m2[1], 1);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

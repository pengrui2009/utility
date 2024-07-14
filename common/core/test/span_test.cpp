

/**
 * @file
 */

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>

#include "ara/core/span.h"
#include <atomic>

using namespace ara::core;

TEST(SpanTest, SpanTest) {
  std::atomic<int> ato;
  int a[1];
  Span<int> sp(ato);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

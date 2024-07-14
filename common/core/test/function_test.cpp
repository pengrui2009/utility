

/**
 * @file
 */

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include "ara/core/functional.h"

using namespace ara::core;

int func() {
  return 1;
}

TEST(FunctionTest, SimpleTest) {
  //Function<void(int&&)> func = [](int&&){return;};
  //Function<int(int&&)> func_1 = [](int&& val){return val;};
  //std::cout << func_1(2) << std::endl;
  int tmp = 10;
  Function<int(void)> func_2([tmp](){return tmp;});
  std::cout << func_2() << std::endl;
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

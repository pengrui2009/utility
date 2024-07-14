

/**
 * @file
 */

#include "ara/core/abort.h"

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

TEST(AbortTest, SimpleTest) {
  pid_t pid = fork();
  ASSERT_TRUE(pid >= 0);
  if (pid == 0) {
    ara::core::Abort("");
  } else if (pid > 0) {
    int processStatus;
    while (true) {
      pid_t result = ::waitpid(pid, &processStatus, WNOHANG);
      if (result != 0) {
        break;
      }
    }
    ASSERT_FALSE(WIFEXITED(processStatus));
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

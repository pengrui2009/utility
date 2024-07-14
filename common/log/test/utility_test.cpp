/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-06-20 19:29:19
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2022-09-02 13:48:50
 * @FilePath: /aeg-adaptive-autosar/ara-api/log/test/utility_test.cpp
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include "ara/log/utility.h"
#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace ara::log::internal;
using namespace std;

TEST(utility, substring) {
  vector<string> r = substring("", "");
  EXPECT_EQ(r.size(), 1);
  EXPECT_EQ(r[0], "");
  r = substring("abc", "");
  EXPECT_TRUE(1 == r.size());
  EXPECT_EQ(r[0], "abc");
  r = substring("abc", "d");
  EXPECT_TRUE(1 == r.size());
  EXPECT_EQ(r[0], "abc");
  r = substring("abcd", "d");
  EXPECT_TRUE(2 == r.size());
  EXPECT_EQ(r[0], "abc");
  r = substring("", "adf");
  EXPECT_TRUE(1 == r.size());
  EXPECT_EQ(r[0], "");
  r = substring("abcd", "ab");
  EXPECT_TRUE(2 == r.size());
  EXPECT_EQ(r[0], "");
  EXPECT_EQ(r[1], "cd");

  r = substring("./abc/1", "/");
  EXPECT_TRUE(3 == r.size());
  EXPECT_EQ(r[0], ".");
  EXPECT_EQ(r[1], "abc");
  EXPECT_EQ(r[2], "1");

  r = substring("abcdeeeeeeff", "eee");
  EXPECT_EQ(3, r.size());
  EXPECT_EQ(r[0], "abcd");
  EXPECT_EQ(r[1], "");
  EXPECT_EQ(r[2], "ff");

  r = substring("abcdeeeeeff", "eee");
  EXPECT_EQ(2, r.size());
  EXPECT_EQ(r[0], "abcd");
  EXPECT_EQ(r[1], "eeff");
}

TEST(utility, pathExist) {
  EXPECT_FALSE(pathExist(""));
  EXPECT_TRUE(pathExist("."));
  EXPECT_TRUE(pathExist("./"));
  EXPECT_TRUE(pathExist(".///"));
  EXPECT_TRUE(pathExist("../"));
  EXPECT_TRUE(pathExist("/root"));
  EXPECT_FALSE(pathExist("/abcd"));
  EXPECT_FALSE(pathExist("alsdfjal"));
  EXPECT_FALSE(pathExist(".///abcd"));
}

TEST(utility, createDir) {
  int32_t user_id = getuid();
  if (user_id == 0U) {
    EXPECT_TRUE(createDir("/root/abc", S_IWUSR | S_IRUSR | S_IXUSR));
    system("rm -r /root/abc");
  } else {
    EXPECT_FALSE(createDir("/root/abc", S_IWUSR | S_IRUSR | S_IXUSR));
  }
  EXPECT_TRUE(createDir("./", S_IWUSR | S_IRUSR | S_IXUSR));
  EXPECT_TRUE(createDir("./folder/folder1/", S_IWUSR | S_IRUSR | S_IXUSR));
  EXPECT_TRUE(createDir("./noname///folder1//folder2", S_IWUSR | S_IRUSR | S_IXUSR));
  EXPECT_TRUE(createDir("./abc/1//", S_IWUSR | S_IRUSR | S_IXUSR));
  EXPECT_TRUE(createDir("./abd/1", S_IWUSR | S_IRUSR | S_IXUSR));
  system("rm -r ./folder");
  system("rm -r ./noname");
  system("rm -r ./abc");
  system("rm -r ./abd");
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
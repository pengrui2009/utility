

/**
 * @file
 */

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include "ara/core/string.h"
#include "ara/core/vector.h"

using namespace ara::core;

TEST(StringTest, SimpleTest) {
  // BasicString()
  String s1;
  // BasicString(const BasicString& s)
  String s2(s1);
  // BasicString(BasicString&& s)
  String s3(std::move(s2));
  // BasicString(const Base& s)
  std::string str1;
  // BasicString(const Base& s)
  String s4(str1);
  // BasicString(Base&& s)
  String s5(std::move(str1));
  // BasicString& operator=(const BasicString& other) &
  String s6 = s5;
  // BasicString& operator=(BasicString&& other) & noexcept
  String s7 = std::move(s6);
  // SWS_CORE_03301
  String s8("123");
  StringView sv("123");
  ASSERT_EQ(static_cast<StringView>(s8), sv);
  // SWS_CORE_03302
  String s9(sv);
  ASSERT_STREQ(s9.c_str(), sv.data());
  // SWS_CORE_03303
  String s10("123123123", 0, 3);
  ASSERT_STREQ(s10.c_str(), "123");
  // SWS_CORE_03304
  String s11;
  s11 = sv;
  ASSERT_EQ(static_cast<StringView>(s11), sv);
  // SWS_CORE_03307
  String s12 = "";
  s12 += sv;
  ASSERT_EQ(static_cast<StringView>(s12), sv);
  // SWS_CORE_03308
  String s13 = "";
  s13.append(sv);
  ASSERT_EQ(static_cast<StringView>(s13), sv);
  // SWS_CORE_03309
  String s14 = "";
  s14.append("123123123", 0, 3);
  ASSERT_STREQ(s14.c_str(), "123");
  // SWS_CORE_03305
  String s15 = "";
  s15.assign(sv);
  ASSERT_EQ(static_cast<StringView>(s15), sv);
  // SWS_CORE_03306
  String s16 = "";
  s16.assign("123123123", 0, 3);
  ASSERT_STREQ(s16.c_str(), "123");
  // SWS_CORE_03310
  String s17 = "1";
  s17.insert(0, sv);
  ASSERT_STREQ(s17.c_str(), "1231");
  // SWS_CORE_03311
  String s18 = "1";
  s18.insert(0, "123123123", 0, 3);
  ASSERT_STREQ(s18.c_str(), "1231");
  // SWS_CORE_03312
  String s19 = "123456789";
  s19.replace(3, 1, sv);
  ASSERT_STREQ(s19.c_str(), "12312356789");
  // SWS_CORE_03313
  String s20 = "123456789";
  s20.replace(3, 1, "123123123", 0, 3);
  ASSERT_STREQ(s20.c_str(), "12312356789");
  // SWS_CORE_03314
  String s21 = "123456789";
  s21.replace(s21.cbegin(), s21.cend(), sv);
  ASSERT_STREQ(s21.c_str(), "123");
  // SWS_CORE_03315
  String s22 = "0123456123";
  ASSERT_EQ(s22.find(sv), 1);
  // SWS_CORE_03316
  String s23 = "0123456123";
  ASSERT_EQ(s23.rfind(sv), 7);
  // SWS_CORE_03317
  String s24 = "0123456123";
  ASSERT_EQ(s24.find_first_of(sv), 1);
  // SWS_CORE_03318
  String s25 = "0123456123";
  ASSERT_EQ(s25.find_last_of(sv), 9);
  // SWS_CORE_03319
  String s26 = "0123456123";
  ASSERT_EQ(s26.find_first_not_of(sv), 0);
  // SWS_CORE_03320
  String s27 = "0123456123";
  ASSERT_EQ(s27.find_last_not_of(sv), 6);
  // SWS_CORE_03321
  String s28 = "123";
  ASSERT_TRUE(s28.compare(sv) == 0);
  String s29 = "1";
  ASSERT_TRUE(s29.compare(sv) < 0);
  String s30 = "1234";
  ASSERT_TRUE(s30.compare(sv) > 0);
  String s31 = "122";
  ASSERT_TRUE(s31.compare(sv) < 0);
  String s32 = "124";
  ASSERT_TRUE(s32.compare(sv) > 0);
  // SWS_CORE_03322
  String s33 = "01234";
  ASSERT_TRUE(s33.compare(1, 3, sv) == 0);
  ASSERT_TRUE(s33.compare(1, 1, sv) < 0);
  ASSERT_TRUE(s33.compare(0, 3, sv) < 0);
  ASSERT_TRUE(s33.compare(2, 3, sv) > 0);
  ASSERT_TRUE(s33.compare(0, 4, sv) < 0);
  ASSERT_TRUE(s33.compare(1, 4, sv) > 0);
  // SWS_CORE_03323
  ASSERT_TRUE(s33.compare(1, 4, "123123123", 0 ,3) > 0);
  swap(s32, s33);
  ASSERT_STREQ(s32.c_str(), "01234");
  ASSERT_STREQ(s33.c_str(), "124");
  ASSERT_FALSE(std::is_trivially_copyable<String>::value);
  String s34 = s33.substr(0, 1);
  ASSERT_STREQ(s34.c_str(), "1");
  StringView sv123("123");
  String s35(sv123);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

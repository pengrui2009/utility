

/**
 * @file
 */

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include "ara/core/string_view.h"

using namespace std::literals;
using namespace ara::core;

TEST(StringViewTest, SimpleTest) {
  // constexpr basic_string_view() noexcept
  constexpr StringView sv1;
  // constexpr basic_string_view(const_pointer p, size_type count)
  constexpr StringView sv2("123", 3);
  // constexpr basic_string_view(const_pointer p)
  constexpr StringView sv3("123");
  // constexpr basic_string_view(const basic_string_view& other)
  StringView sv4(sv3);
  // basic_string_view& operator=(const basic_string_view& other)
  StringView sv5 = sv3;
  // begin()
  ASSERT_EQ(*(sv5.begin()), '1');
  // cbeign()
  ASSERT_EQ(*(sv5.cbegin()), '1');
  // end()
  ASSERT_EQ(*(sv5.end() - 1), '3');
  // cend()
  ASSERT_EQ(*(sv5.cend() - 1), '3');
  // rbegin()
  ASSERT_EQ(*(sv5.rbegin()), '3');
  // crbegin()
  ASSERT_EQ(*(sv5.crbegin()), '3');
  // rend()
  ASSERT_EQ(*(sv5.rend() - 1), '1');
  // crend()
  ASSERT_EQ(*(sv5.crend() - 1), '1');
  // size()
  ASSERT_EQ(sv5.size(), 3);
  // length()
  ASSERT_EQ(sv5.length(), 3);
  // max_size()
  ASSERT_EQ(sv5.max_size(), std::numeric_limits<std::size_t>::max());
  // empty()
  ASSERT_EQ(sv5.empty(), false);
  ASSERT_EQ(sv1.empty(), true);
  // operator[](size_type pos)
  ASSERT_EQ(sv5[0], '1');
  // at()
  ASSERT_EQ(sv5.at(0), '1');
  // front()
  ASSERT_EQ(sv5.front(), '1');
  // back()
  ASSERT_EQ(sv5.back(), '3');
  // data()
  ASSERT_STREQ(sv5.data(), "123");
  // remove_prefix
  sv5.remove_prefix(1);
  ASSERT_STREQ(sv5.data(), "23");
  // remove_suffix
  sv5.remove_suffix(1);
  ASSERT_EQ(sv5.size(), 1);
  // constexpr void swap(basic_string_view& other) noexcept
  swap(sv4, sv5);
  ASSERT_EQ(sv4.size(), 1);
  ASSERT_STREQ(sv5.data(), "123");
  // size_type copy(pointer dest, size_type count, size_type pos = 0ULL)
  char* tmp = new char[3];
  ASSERT_TRUE(sv5.size() >= 0);
  ASSERT_EQ(sv5.copy(tmp, 3), 3ULL);
  ASSERT_STREQ(sv5.data(), tmp);
  // constexpr basic_string_view substr(size_type pos = 0ULL, size_type count = npos)
  ASSERT_EQ(sv5.substr(1, 1), "2");
  // constexpr int compare(basic_string_view v) const noexcept
  StringView sv("123");
  StringView s28 = "123";
  ASSERT_TRUE(s28.compare(sv) == 0);
  StringView s29 = "1";
  ASSERT_TRUE(s29.compare(sv) < 0);
  StringView s30 = "1234";
  ASSERT_TRUE(s30.compare(sv) > 0);
  StringView s31 = "122";
  ASSERT_TRUE(s31.compare(sv) < 0);
  StringView s32 = "124";
  ASSERT_TRUE(s32.compare(sv) > 0);
  // constexpr int compare(size_type pos1, size_type count1, basic_string_view v)
  StringView s33 = "01234";
  ASSERT_TRUE(s33.compare(1, 3, sv) == 0);
  ASSERT_TRUE(s33.compare(1, 1, sv) < 0);
  ASSERT_TRUE(s33.compare(0, 3, sv) < 0);
  ASSERT_TRUE(s33.compare(2, 3, sv) > 0);
  ASSERT_TRUE(s33.compare(0, 4, sv) < 0);
  ASSERT_TRUE(s33.compare(1, 4, sv) > 0);
  // constexpr int compare(size_type pos1, size_type count1, basic_string_view v, size_type pos2, size_type count2) const 
  StringView sv6 = "123123123";
  ASSERT_TRUE(s33.compare(1, 4, sv6, 0 ,3) > 0);
  // constexpr int compare(const_pointer s)
  ASSERT_TRUE(s28.compare("123") == 0);
  // constexpr int compare(size_type pos1, size_type count1, const_pointer s) const
  ASSERT_TRUE(s33.compare(1, 4, "123") > 0);
  // constexpr int compare(size_type pos1, size_type count1, const_pointer s, size_type count2)
  ASSERT_TRUE(s33.compare(1, 4, "123123123", 0 ,3) > 0);
  // find(basic_string_view v, size_type pos = 0ULL)
  StringView s22 = "0123456123";
  ASSERT_EQ(s22.find(sv), 1);
  // size_type find(value_type ch, size_type pos = 0ULL) const noexcept
  ASSERT_EQ(s22.find('1'), 1);
  // size_type find(const_pointer s, size_type pos, size_type count) const
  ASSERT_EQ(s22.find("123", 0 , 3), 1);
  // size_type find(const_pointer s, size_type pos = 0ULL)
  ASSERT_EQ(s22.find("123"), 1);
  // size_type rfind(basic_string_view v, size_type pos = npos)
  ASSERT_EQ(s22.rfind(sv), 7);
  // size_type rfind(value_type c, size_type pos = npos)
  ASSERT_EQ(s22.rfind('1'), 7);
  // size_type rfind(const_pointer s, size_type pos, size_type count)
  ASSERT_EQ(s22.rfind("123", s22.size(), 3), 7);
  // size_type rfind(const_pointer s, size_type pos = npos)
  ASSERT_EQ(s22.rfind("123"), 7);
  // size_type find_first_of(basic_string_view v, size_type pos = 0ULL)
  ASSERT_EQ(s22.find_first_of(sv), 1);
  // size_type find_first_of(value_type c, size_type pos = 0ULL)
  ASSERT_EQ(s22.find_first_of('1'), 1);
  // size_type find_first_of(const_pointer s, size_type pos, size_type count)
  ASSERT_EQ(s22.find_first_of("123", 0, 3), 1);
  // size_type find_first_of(const_pointer s, size_type pos = 0ULL)
  ASSERT_EQ(s22.find_first_of("123"), 1);
  // size_type find_last_of(basic_string_view v, size_type pos = npos)
  ASSERT_EQ(s22.find_last_of(sv), 9);
  // size_type find_last_of(value_type c, size_type pos = npos)
  ASSERT_EQ(s22.find_last_of('3'), 9);
  // size_type find_last_of(const_pointer s, size_type pos, size_type count)
  ASSERT_EQ(s22.find_last_of("123", s22.size(), 3), 9);
  // size_type find_last_of(const_pointer s, size_type pos = npos)
  ASSERT_EQ(s22.find_last_of("123"), 9);
  // size_type find_first_not_of(basic_string_view v, size_type pos = 0ULL)
  ASSERT_EQ(s22.find_first_not_of(sv), 0);
  // size_type find_first_not_of(value_type c, size_type pos = 0ULL)
  ASSERT_EQ(s22.find_first_not_of('1'), 0);
  // size_type find_first_not_of(const_pointer s, size_type pos, size_type count)
  ASSERT_EQ(s22.find_first_not_of("123", 0, 3), 0);
  // size_type find_first_not_of(const_pointer s, size_type pos = 0ULL)
  ASSERT_EQ(s22.find_first_not_of("123"), 0);
  // size_type find_last_not_of(basic_string_view v, size_type pos = npos)
  ASSERT_EQ(s22.find_last_not_of(sv), 6);
  // size_type find_last_not_of(value_type c, size_type pos = npos)
  ASSERT_EQ(s22.find_last_not_of('3'), 8);
  // size_type find_last_not_of(const_pointer s, size_type pos, size_type count)
  ASSERT_EQ(s22.find_last_not_of("123", s22.size(), 3), 6);
  // size_type find_last_not_of(const_pointer s, size_type pos = npos)
  ASSERT_EQ(s22.find_last_not_of("123"), 6);

  StringView eq("123");
  StringView b1("124");
  StringView b2("1234");
  StringView s1("122");
  StringView s2("12");
  // constexpr bool operator==(basic_string_view<value_type, traits_type> lhs, basic_string_view<value_type, traits_type> rhs) noexcept
  ASSERT_TRUE(sv == eq);
  ASSERT_FALSE(sv == b1);
  // constexpr bool operator==(
  //  basic_string_view<value_type, traits_type> lhs, Identity<basic_string_view<value_type, traits_type>> rhs) noexcept
  ASSERT_TRUE(sv == "123");
  ASSERT_FALSE(sv == "1234");
  //constexpr bool operator==(
  //    Identity<basic_string_view<value_type, traits_type>> lhs, basic_string_view<value_type, traits_type> rhs) noexcept 
  ASSERT_TRUE("123" == sv);
  ASSERT_FALSE("1234" == sv);
  //constexpr bool operator!=(basic_string_view<value_type, traits_type> lhs, basic_string_view<value_type, traits_type> rhs) noexcept 
  ASSERT_TRUE(sv != b1);
  ASSERT_FALSE(sv != eq);
  //constexpr bool operator!=(
  //    basic_string_view<value_type, traits_type> lhs, Identity<basic_string_view<value_type, traits_type>> rhs) noexcept 
  ASSERT_TRUE(sv != "1234");
  ASSERT_FALSE(sv != "123");
  //constexpr bool operator!=(
  //    Identity<basic_string_view<value_type, traits_type>> lhs, basic_string_view<value_type, traits_type> rhs) noexcept 
  ASSERT_TRUE("1234" != sv);
  ASSERT_FALSE("123" != sv);
  //constexpr bool operator<(basic_string_view<value_type, traits_type> lhs, basic_string_view<value_type, traits_type> rhs) noexcept 
  ASSERT_TRUE(sv < b1);
  ASSERT_TRUE(sv < b2);
  ASSERT_FALSE(sv < s1);
  ASSERT_FALSE(sv < s2);
  ASSERT_FALSE(sv < eq);
  //constexpr bool operator<(
  //    basic_string_view<value_type, traits_type> lhs, Identity<basic_string_view<value_type, traits_type>> rhs) noexcept 
  ASSERT_TRUE(sv < "1234");
  ASSERT_TRUE(sv < "124");
  ASSERT_FALSE(sv < "12");
  ASSERT_FALSE(sv < "122");
  ASSERT_FALSE(sv < "123");
  //constexpr bool operator<(
  //    Identity<basic_string_view<value_type, traits_type>> lhs, basic_string_view<value_type, traits_type> rhs) noexcept 
  ASSERT_TRUE("12" < sv);
  ASSERT_TRUE("122" < sv);
  ASSERT_FALSE("124" < sv);
  ASSERT_FALSE("1234" < sv);
  ASSERT_FALSE("123" < sv);
  //constexpr bool operator>(basic_string_view<value_type, traits_type> lhs, basic_string_view<value_type, traits_type> rhs) noexcept 
  ASSERT_TRUE(sv > s1);
  ASSERT_TRUE(sv > s2);
  ASSERT_FALSE(sv > b1);
  ASSERT_FALSE(sv > b2);
  ASSERT_FALSE(sv > eq);
  //constexpr bool operator>(
  //    basic_string_view<value_type, traits_type> lhs, Identity<basic_string_view<value_type, traits_type>> rhs) noexcept 
  ASSERT_TRUE(sv > "12");
  ASSERT_TRUE(sv > "122");
  ASSERT_FALSE(sv > "124");
  ASSERT_FALSE(sv > "1234");
  ASSERT_FALSE(sv > "123");
  //constexpr bool operator>(
  //    Identity<basic_string_view<value_type, traits_type>> lhs, basic_string_view<value_type, traits_type> rhs) noexcept 
  ASSERT_TRUE("124" > sv);
  ASSERT_TRUE("1234" > sv);
  ASSERT_FALSE("122" > sv);
  ASSERT_FALSE("12" > sv);
  ASSERT_FALSE("123" > sv);
  //constexpr bool operator<=(basic_string_view<value_type, traits_type> lhs, basic_string_view<value_type, traits_type> rhs) noexcept 
  ASSERT_TRUE(sv <= b1);
  ASSERT_TRUE(sv <= b2);
  ASSERT_TRUE(sv <= eq);
  ASSERT_FALSE(sv <= s1);
  ASSERT_FALSE(sv <= s2);
  //constexpr bool operator<=(
  //    basic_string_view<value_type, traits_type> lhs, Identity<basic_string_view<value_type, traits_type>> rhs) noexcept 
  ASSERT_TRUE(sv <= "1234");
  ASSERT_TRUE(sv <= "124");
  ASSERT_TRUE(sv <= "123");
  ASSERT_FALSE(sv <= "12");
  ASSERT_FALSE(sv <= "122");
  //constexpr bool operator<=(
  //    Identity<basic_string_view<value_type, traits_type>> lhs, basic_string_view<value_type, traits_type> rhs) noexcept 
  ASSERT_TRUE("12" <= sv);
  ASSERT_TRUE("122" <= sv);
  ASSERT_TRUE("123" <= sv);
  ASSERT_FALSE("124" <= sv);
  ASSERT_FALSE("1234" <= sv);
  //constexpr bool operator>=(basic_string_view<value_type, traits_type> lhs, basic_string_view<value_type, traits_type> rhs) noexcept 
  ASSERT_TRUE(sv >= s1);
  ASSERT_TRUE(sv >= s2);
  ASSERT_TRUE(sv >= eq);
  ASSERT_FALSE(sv >= b1);
  ASSERT_FALSE(sv >= b2);
  //constexpr bool operator>=(
  //    basic_string_view<value_type, traits_type> lhs, Identity<basic_string_view<value_type, traits_type>> rhs) noexcept 
  ASSERT_TRUE(sv >= "12");
  ASSERT_TRUE(sv >= "122");
  ASSERT_TRUE(sv >= "123");
  ASSERT_FALSE(sv >= "124");
  ASSERT_FALSE(sv >= "1234");
  //constexpr bool operator>=(
  //    Identity<basic_string_view<value_type, traits_type>> lhs, basic_string_view<value_type, traits_type> rhs) noexcept 
  ASSERT_TRUE("124" >= sv);
  ASSERT_TRUE("1234" >= sv);
  ASSERT_TRUE("123" >= sv);
  ASSERT_FALSE("122" >= sv);
  ASSERT_FALSE("12" >= sv);
  ASSERT_TRUE(std::is_trivially_copyable<StringView>::value);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

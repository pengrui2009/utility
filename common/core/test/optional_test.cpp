

/**
 * @file
 */

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>

#include "ara/core/optional.h"
#include "ara/core/vector.h"
#include "ara/core/string.h"

using namespace ara::core;

TEST(OptionalTest, SimpleTest) {
  {
    auto tmp = Optional<int>(); // constexpr Optional() noexcept; constexpr Optional<T>& operator=(Optional<T>&& other);
  }
  {
    auto tmp = Optional<int>(); // constexpr Optional() noexcept; constexpr Optional<T>& operator=(Optional<T>&& other);
    Optional<int> tmp1(tmp);
  }
  {
    const Optional<int> tmp;
    Optional<int> tmp1 = tmp;
  }
  {
    auto tmp = Optional<int>(); // constexpr Optional() noexcept; constexpr Optional<T>& operator=(Optional<T>&& other);
    Optional<int> tmp1(std::move(tmp));
  }
  {
     Optional<int> tmp = nullopt;
  }
  {
    auto tmp = Optional<int>(nullopt); // constexpr Optional(nullopt_t) noexcept;
    ASSERT_TRUE(tmp.value_or(1) == 1);
    ASSERT_TRUE(tmp == nullopt);
    ASSERT_FALSE(tmp > nullopt);
    ASSERT_TRUE(tmp >= nullopt);
    ASSERT_FALSE(tmp < nullopt);
    ASSERT_TRUE(nullopt == tmp);
    ASSERT_FALSE(nullopt != tmp);
    ASSERT_FALSE(nullopt > tmp);
    ASSERT_TRUE(nullopt >= tmp);
    ASSERT_FALSE(nullopt < tmp);
    ASSERT_TRUE(nullopt <= tmp);
  }
  {
    auto tmp = Optional<int>(1); // constexpr Optional(U&& u)
    ASSERT_TRUE(tmp != nullopt);
    ASSERT_TRUE(tmp > nullopt);
    ASSERT_TRUE(tmp >= nullopt);
    ASSERT_FALSE(tmp < nullopt);
    ASSERT_FALSE(tmp <= nullopt);
    ASSERT_FALSE(nullopt > tmp);
    ASSERT_FALSE(nullopt >= tmp);
    ASSERT_TRUE(nullopt < tmp);
    ASSERT_TRUE(nullopt <= tmp);
    ASSERT_FALSE(tmp == 0.9);
    ASSERT_TRUE(tmp != 0.9);
    ASSERT_TRUE(tmp > 0.9);
    ASSERT_FALSE(tmp < 0.9);
    ASSERT_TRUE(tmp >= 0.9);
    ASSERT_FALSE(tmp <= 0.9);
    auto tmp4 = tmp; // constexpr Optional<T>& operator=(const Optional<T>& other) & = default;
  }
  {
    Optional<int> tmp = 1; // operator=(U&& u)
  }
  {
    Optional<int> tmp = nullopt; // optional<T>& operator=(nullopt_t)
  }
  {
    Optional<Vector<int>>(in_place, {1, 2, 3}, std::allocator<int>()); // constexpr explicit Optional(in_place_t, std::initializer_list<U> il, Args&&... args)
    Optional<Vector<int>>(in_place, 1, 1); //constexpr explicit Optional(in_place_t, Args&&... args)
  }
  {
    auto tmp = Optional<String>(in_place, 3 , 'A'); // constexpr Optional<T>& operator=(Optional<T>&& other)
    ASSERT_TRUE(tmp); // operator bool()
    ASSERT_TRUE((*tmp) == String("AAA")); // operator*()
    ASSERT_STREQ(tmp->c_str(), "AAA"); // operator->
    ASSERT_TRUE(tmp.has_value()); //has_value()
    ASSERT_TRUE(tmp.value() == String("AAA")); //T&& value()
    ASSERT_TRUE(tmp.value_or("123") == String("AAA")); //T value_or(U&& v) &&
    ASSERT_TRUE(tmp == String("AAA"));
  }
  {
    const Optional<int> tmp(1); 
    const auto tmp1 = *(std::move(tmp)); // const T&& value()
    ASSERT_TRUE(tmp1 == 1);
  }
  {
    Optional<int> tmp = 1; // operator=(U&& u)
    const auto& tmp1 = tmp.value(); //const T& value()
    ASSERT_TRUE(tmp1 == 1);
  }
  {
    const Optional<int> tmp(1); 
    auto& tmp1 = tmp.value(); //T& value()
    ASSERT_TRUE(tmp1 == 1);
  }
  {
    const Optional<int> tmp(1); 
    const auto tmp1 = std::move(tmp).value(); // const T&& value()
    ASSERT_TRUE(tmp1 == 1);
  }
  {
    Optional<int> tmp = 1; // operator=(U&& u)
    auto tmp1 = std::move(tmp).value(); //T&& value()
    ASSERT_TRUE(tmp1 == 1);
  }
  {
    Optional<int> tmp = 1; // operator=(U&& u)
    const auto& tmp1 = tmp.value_or(123); //T value_or(U&& v) const &
    ASSERT_TRUE(tmp1 == 1);
  }
  {
    Optional<int> tmp(nullopt);
    const auto& tmp1 = std::move(tmp).value_or(123); //T value_or(U&& v) &&
    ASSERT_TRUE(tmp1 == 123);
  }
  {
    auto tmp = make_optional<int>(1);
  }
  {
    auto tmp = make_optional<Vector<int>>({1, 2, 3}, std::allocator<int>());
  }
  {
    auto tmp = make_optional<String>(3 , 'A');
  }
  {
    Optional<Vector<int>> tmp; 
    ASSERT_TRUE(tmp.emplace({1, 3}) == (Vector<int>{1, 3})); //emplace(std::initializer_list<U> il, Args&&... args)
    ASSERT_TRUE(tmp == (Vector<int>{1, 3}));
    ASSERT_TRUE(tmp.emplace(1, 3) == (Vector<int>{3})); //emplace(Args&&... args)
  }
  {
    ASSERT_TRUE(Optional<int>(1) == Optional<int>(1));
    ASSERT_FALSE(Optional<int>(2) == Optional<int>(1));
  }
  {
    ASSERT_TRUE(Optional<int>(0) != Optional<int>(1));
    ASSERT_FALSE(Optional<int>(1) != Optional<int>(1));
  }
  {
    ASSERT_TRUE(Optional<int>(0) < Optional<int>(1));
    ASSERT_FALSE(Optional<int>(1) < Optional<int>(1));
    ASSERT_FALSE(Optional<int>(2) < Optional<int>(1));
  }
  {
    ASSERT_TRUE(Optional<int>(2) > Optional<int>(1));
    ASSERT_FALSE(Optional<int>(0) > Optional<int>(1));
    ASSERT_FALSE(Optional<int>(1) > Optional<int>(1));
  }
  {
    ASSERT_TRUE(Optional<int>(0) <= Optional<int>(1));
    ASSERT_TRUE(Optional<int>(1) <= Optional<int>(1));
    ASSERT_FALSE(Optional<int>(2) <= Optional<int>(1));
  }
  {
    ASSERT_TRUE(Optional<int>(2) >= Optional<int>(1));
    ASSERT_TRUE(Optional<int>(1) >= Optional<int>(1));
    ASSERT_FALSE(Optional<int>(0) >= Optional<int>(1));
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

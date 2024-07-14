

/**
 * @file
 */

#include "ara/core/instance_specifier.h"

#include <gtest/gtest.h>

TEST(InstanceSpecifierTest, Constructor_StringView_Success) {
  ara::core::InstanceSpecifier is("executable/swc/port");
}

TEST(InstanceSpecifierTest, Factory_Validation) {
  {
    ara::core::Result<ara::core::InstanceSpecifier> res =
        ara::core::InstanceSpecifier::Create("executable/swc/port");
    ASSERT_TRUE(res.HasValue());
    const ara::core::InstanceSpecifier& is = res.Value();
    ASSERT_EQ(is.ToString(), "executable/swc/port");
  }

  {
    auto res = ara::core::InstanceSpecifier::Create(
        "executable/cluster1/cluster2/swc/port");
    ASSERT_TRUE(res.HasValue());
  }

  {
    auto res = ara::core::InstanceSpecifier::Create("executable/s%%c/port");
    ASSERT_TRUE(res.Error() == ara::core::CoreErrc::invalid_meta_model_shortname);
  }

  {
    auto res = ara::core::InstanceSpecifier::Create("_executable/swc/port");
    ASSERT_TRUE(res.Error() == ara::core::CoreErrc::invalid_meta_model_shortname);
  }

  {
    auto res = ara::core::InstanceSpecifier::Create("/executable/swc/port");
    ASSERT_TRUE(res.Error() == ara::core::CoreErrc::invalid_meta_model_path);
  }

  {
    auto res = ara::core::InstanceSpecifier::Create("executable/swc/port/");
    ASSERT_TRUE(res.Error() == ara::core::CoreErrc::invalid_meta_model_path);
  }

  {
    auto res = ara::core::InstanceSpecifier::Create("executable/swc//port");
    ASSERT_TRUE(res.Error() == ara::core::CoreErrc::invalid_meta_model_path);
  }

  {
    auto res = ara::core::InstanceSpecifier::Create("executable/s c/port");
    ASSERT_TRUE(res.Error() == ara::core::CoreErrc::invalid_meta_model_shortname);
  }

  {
    auto res = ara::core::InstanceSpecifier::Create("executable/  /port");
    ASSERT_TRUE(res.Error() == ara::core::CoreErrc::invalid_meta_model_shortname);
  }

  {
    auto res = ara::core::InstanceSpecifier::Create("  ");
    ASSERT_TRUE(res.Error() == ara::core::CoreErrc::invalid_meta_model_shortname);
  }

  {
    auto res = ara::core::InstanceSpecifier::Create("");
    ASSERT_TRUE(res.Error() == ara::core::CoreErrc::invalid_meta_model_path);
  }
}

TEST(InstanceSpecifierTest, ToString) {
  const ara::core::InstanceSpecifier is("executable/swc/port");

  ASSERT_EQ(is.ToString(), "executable/swc/port");
}

TEST(InstanceSpecifierTest, EqualityComparisonOperator_OtherInstanceSpecifier) {
  const ara::core::InstanceSpecifier is1("executable/swc/port");
  const ara::core::InstanceSpecifier is2("executable/swc/port");
  const ara::core::InstanceSpecifier is3("another_executable/swc/path");

  ASSERT_TRUE(is1 == is2);
  ASSERT_TRUE(is1 != is3);
}

TEST(InstanceSpecifierTest, EqualityComparisonOperator_StringView) {
  const ara::core::InstanceSpecifier is("executable/swc/port");

  ASSERT_TRUE(is == "executable/swc/port");
  ASSERT_TRUE(is != "another_executable/swc/port");
}

TEST(InstanceSpecifierTest, GlobalEqualityComparisonOperator_StringView) {
  ara::core::InstanceSpecifier const is("executable/swc/port");

  ASSERT_TRUE(is == "executable/swc/port");
  ASSERT_TRUE(is != "another_executable/swc/port");
}

TEST(InstanceSpecifierTest, OrderingComparisonOperator_OtherInstanceSpecifier) {
  const ara::core::InstanceSpecifier is1("abc/swc/port");
  const ara::core::InstanceSpecifier is2("aca/swc/port");
  const ara::core::InstanceSpecifier is3("adc/swc/port");

  ASSERT_TRUE(is1 < is2);
  ASSERT_TRUE(is1 < is3);
  ASSERT_TRUE(is2 < is3);
  ASSERT_FALSE(is2 < is1);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


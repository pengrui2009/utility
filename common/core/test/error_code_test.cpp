

/**
 * @file
 */

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>

#include "ara/core/core_error_domain.h"
#include "ara/core/error_code.h"
#include "ara/core/future_error_domain.h"

using namespace ara::core;

TEST(ErrorCodeTest, SimpleTest) {
  // SWS_CORE_00512
  auto core_errc = ErrorCode(CoreErrc::connection_aborted);
  // SWS_CORE_00514
  ASSERT_EQ(core_errc.Value(), static_cast<ErrorDomain::CodeType>(CoreErrc::connection_aborted));
  // SWS_CORE_00516
  core_errc.SupportData();
  // SWS_CORE_00515
  ASSERT_EQ(core_errc.Domain(), internal::g_CoreErrorDomain);
  ASSERT_FALSE(core_errc.Domain() == internal::g_futureErrorDomain);
  auto futu_errc = ErrorCode(future_errc::promise_already_satisfied);
  ASSERT_FALSE(core_errc.Domain() == futu_errc.Domain());
  // SWS_CORE_00518
  core_errc.Message();
  // SWS_CORE_00519
  // EXPECT_THROW(core_errc.ThrowAsException(), CoreException(core_errc));
  // SWS_CORE_00572
  ASSERT_TRUE(core_errc != futu_errc);
  // SWS_CORE_00571
  ASSERT_FALSE(core_errc == futu_errc);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

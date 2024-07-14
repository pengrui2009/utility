

/**
 * @file
 */

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include "ara/core/future_error_domain.h"

using namespace ara::core;

TEST(FutureErrorDomainTest, SimpleTest) {
  // SWS_CORE_00400
  ASSERT_EQ(static_cast<int32_t>(future_errc::broken_promise), 101);
  ASSERT_EQ(static_cast<int32_t>(future_errc::future_already_retrieved), 102);
  ASSERT_EQ(static_cast<int32_t>(future_errc::promise_already_satisfied), 103);
  ASSERT_EQ(static_cast<int32_t>(future_errc::no_state), 104);
  // SWS_CORE_00443
  ASSERT_STREQ(
      internal::g_futureErrorDomain.Message(static_cast<ErrorDomain::CodeType>(future_errc::broken_promise)),
      "broken promise");
  ASSERT_STREQ(
      internal::g_futureErrorDomain.Message(static_cast<ErrorDomain::CodeType>(future_errc::future_already_retrieved)),
      "future already retrieved");
  ASSERT_STREQ(
      internal::g_futureErrorDomain.Message(static_cast<ErrorDomain::CodeType>(future_errc::promise_already_satisfied)),
      "promise already satisfied");
  ASSERT_STREQ(
      internal::g_futureErrorDomain.Message(static_cast<ErrorDomain::CodeType>(future_errc::no_state)),
      "no state associated with this future");
  ASSERT_STREQ(internal::g_futureErrorDomain.Message(static_cast<ErrorDomain::CodeType>(-1)), "unknown future error");
  // SWS_CORE_00280
  ASSERT_EQ(GetFutureErrorDomain(), internal::g_futureErrorDomain);
  // SWS_CORE_00290
  auto error_code = MakeErrorCode(future_errc::broken_promise, ErrorDomain::SupportDataType());
  ASSERT_EQ(error_code.Value(), static_cast<ErrorDomain::CodeType>(future_errc::broken_promise));
  ASSERT_EQ(error_code.Domain(), internal::g_futureErrorDomain);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

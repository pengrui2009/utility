

/**
 * @file
 */

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include "ara/core/core_error_domain.h"

using namespace ara::core;

TEST(CoreErrorDomainTest, SimpleTest) {
  // SWS_CORE_00200
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::address_family_not_supported), 97);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::address_in_use), 98);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::address_not_available), 99);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::already_connected), 106);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::argument_list_too_long), 7);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::argument_out_of_domain), 33);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::bad_address), 14);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::bad_file_descriptor), 9);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::bad_message), 74);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::broken_pipe), 32);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::connection_aborted), 103);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::connection_already_in_progress), 114);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::connection_refused), 111);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::connection_reset), 104);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::cross_device_link), 18);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::destination_address_required), 89);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::device_or_resource_busy), 16);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::directory_not_empty), 39);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::executable_format_error), 8);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::file_exists), 17);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::file_too_large), 27);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::filename_too_long), 36);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::function_not_supported), 38);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::host_unreachable), 113);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::identifier_removed), 43);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::illegal_byte_sequence), 84);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::inappropriate_io_control_operation), 25);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::interrupted), 4);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::invalid_argument), 22);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::invalid_seek), 29);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::io_error), 5);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::is_a_directory), 21);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::message_size), 90);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::network_down), 100);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::network_reset), 102);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::network_unreachable), 101);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::no_buffer_space), 105);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::no_child_process), 10);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::no_link), 67);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::no_lock_available), 37);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::no_message_available), 61);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::no_message), 42);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::no_protocol_option), 92);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::no_space_on_device), 28);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::no_stream_resources), 63);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::no_such_device_or_address), 6);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::no_such_device), 19);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::no_such_file_or_directory), 2);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::no_such_process), 3);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::not_a_directory), 20);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::not_a_socket), 88);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::not_a_stream), 60);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::not_connected), 107);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::not_enough_memory), 12);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::not_supported), 202);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::operation_canceled), 125);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::operation_in_progress), 115);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::operation_not_permitted), 1);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::operation_not_supported), 95);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::operation_would_block), 201);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::owner_dead), 130);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::permission_denied), 13);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::protocol_error), 71);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::protocol_not_supported), 93);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::read_only_file_system), 30);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::resource_deadlock_would_occur), 35);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::resource_unavailable_try_again), 11);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::result_out_of_range), 34);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::state_not_recoverable), 131);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::stream_timeout), 62);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::text_file_busy), 26);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::timed_out), 110);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::too_many_files_open_in_system), 23);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::too_many_files_open), 24);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::too_many_links), 31);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::too_many_symbolic_link_levels), 40);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::value_too_large), 75);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::wrong_protocol_type), 91);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::invalid_meta_model_shortname), 137);
  ASSERT_EQ(static_cast<int32_t>(CoreErrc::invalid_meta_model_path), 138);
  // SWS_CORE_00231 - SWS_CORE_00242 Skip
  // SWS_CORE_00243
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::address_family_not_supported)),
      "Address family not supported");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::address_in_use)),
      "Address in use");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::address_not_available)),
      "Address not available");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::already_connected)),
      "Transport endpoint is already connected");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::argument_list_too_long)),
      "Argument list too long");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::argument_out_of_domain)),
      "Argument out of domain");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::bad_address)), "Bad address");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::bad_file_descriptor)),
      "Bad file descriptor");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::bad_message)), "Bad message");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::broken_pipe)), "Broken pipe");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::connection_aborted)),
      "Connection aborted");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::connection_already_in_progress)),
      "Connection already in progress");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::connection_refused)),
      "Connection refused");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::connection_reset)),
      "Connection reset");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::cross_device_link)),
      "Cross-device link");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::destination_address_required)),
      "Destination address required");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::device_or_resource_busy)),
      "Device or resource busy");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::directory_not_empty)),
      "Directory not empty");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::executable_format_error)),
      "Executable format error");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::file_exists)), "File exists");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::file_too_large)),
      "File too large");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::filename_too_long)),
      "File name too long");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::function_not_supported)),
      "Function not supported");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::host_unreachable)),
      "Host unreachable");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::identifier_removed)),
      "Identifier removed");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::illegal_byte_sequence)),
      "Illegal byte sequence");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(
          static_cast<ErrorDomain::CodeType>(CoreErrc::inappropriate_io_control_operation)),
      "Inappropriate I/O control operation");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::interrupted)),
      "Interrupted system call");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::invalid_argument)),
      "Remote I/O error");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::invalid_seek)), "Invalid seek");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::io_error)), "Remote I/O error");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::is_a_directory)),
      "Is a directory");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::message_size)),
      "Message too long");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::network_down)),
      "Network is down");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::network_reset)),
      "Network dropped connection because of reset");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::network_unreachable)),
      "Network is unreachable");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::no_buffer_space)),
      "No buffer space available");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::no_child_process)),
      "No child process");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::no_link)),
      "Link has been severed");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::no_lock_available)),
      "No record locks available");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::no_message_available)),
      "No data available");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::no_message)),
      "No message of desired type");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::no_protocol_option)),
      "Protocol not available");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::no_space_on_device)),
      "No space left on device");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::no_stream_resources)),
      "Device not a stream");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::no_such_device_or_address)),
      "No such device or address");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::no_such_device)),
      "No such device");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::no_such_file_or_directory)),
      "No such file or directory");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::no_such_process)),
      "No such process");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::not_a_directory)),
      "Not a directory");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::not_a_socket)),
      "Socket operation on non-socket");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::not_a_stream)),
      "Device not a stream");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::not_connected)),
      "Transport endpoint is not connected");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::not_enough_memory)),
      "Out of memory");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::not_supported)),
      "Not supported");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::operation_canceled)),
      "Operation canceled");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::operation_in_progress)),
      "Operation in progress");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::operation_not_permitted)),
      "Operation not permitted");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::operation_not_supported)),
      "Operation not supported");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::operation_would_block)),
      "Operation would block");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::owner_dead)), "Owner dead");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::permission_denied)),
      "Permission denied");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::protocol_error)),
      "Protocol error");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::protocol_not_supported)),
      "Protocol not supported");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::read_only_file_system)),
      "Read-only file system");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::resource_deadlock_would_occur)),
      "Resource deadlock would occur");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::resource_unavailable_try_again)),
      "Resource unavailable, try again");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::result_out_of_range)),
      "Result out of range");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::state_not_recoverable)),
      "State not recoverable");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::stream_timeout)),
      "Stream timeout");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::text_file_busy)),
      "Text file busy");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::timed_out)), "Time out");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::too_many_files_open_in_system)),
      "Too many files open in system");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::too_many_files_open)),
      "Too many open files");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::too_many_links)),
      "Too many links");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::too_many_symbolic_link_levels)),
      "Too many symbolic links encountered");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::value_too_large)),
      " Value too large");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::wrong_protocol_type)),
      "Wrong protocol type");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::invalid_meta_model_shortname)),
      "Invalid Meta Model Shortname");
  ASSERT_STREQ(
      internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(CoreErrc::invalid_meta_model_path)),
      "Invalid Meta Model Path");
  ASSERT_STREQ(internal::g_CoreErrorDomain.Message(static_cast<ErrorDomain::CodeType>(-1)), "Unknown error");
  // SWS_CORE_00280
  ASSERT_EQ(GetCoreErrorDomain(), internal::g_CoreErrorDomain);
  // SWS_CORE_00290
  auto error_code = MakeErrorCode(CoreErrc::invalid_meta_model_path, ErrorDomain::SupportDataType());
  ASSERT_EQ(error_code.Value(), static_cast<ErrorDomain::CodeType>(CoreErrc::invalid_meta_model_path));
  ASSERT_EQ(error_code.Domain(), internal::g_CoreErrorDomain);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

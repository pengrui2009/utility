

#ifndef TUSIMPLEAP_ARA_CORE_CORE_ERROR_DOMAIN_H_
#define TUSIMPLEAP_ARA_CORE_CORE_ERROR_DOMAIN_H_

#include <cerrno>

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"

namespace ara {
namespace core {

/// @brief An enumeration that defines all errors from Core.1.
///
/// @uptrace{SWS_CORE_00200}
enum class CoreErrc : ErrorDomain::CodeType {
  address_family_not_supported = 97,        ///< address family not supported (EAFNOSUPPORT)
  address_in_use = 98,                      ///< address in use (EADDRINUSE)
  address_not_available = 99,               ///< address not available (EADDRNOTAVAIL)
  already_connected = 106,                  ///< already conntected (EISCONN)
  argument_list_too_long = 7,               ///< argument list too long (E2BIG)
  argument_out_of_domain = 33,              ///< argument out of domain (EDOM)
  bad_address = 14,                         ///< bad address (EFAULT)
  bad_file_descriptor = 9,                  ///< bad file descriptor (EBADF)
  bad_message = 74,                         ///< bad message (EBADMSG)
  broken_pipe = 32,                         ///< broken pipe (EPIPE)
  connection_aborted = 103,                 ///< connection aborted (ECONNABORTED)
  connection_already_in_progress = 114,     ///< connection already in progress (EALREADY)
  connection_refused = 111,                 ///< connection refused (ECONNREFUSED)
  connection_reset = 104,                   ///< connection reset (ECONNRESET)
  cross_device_link = 18,                   ///< cross-device link (EXDEV)
  destination_address_required = 89,        ///< destination address required (EDESTADDRREQ)
  device_or_resource_busy = 16,             ///< device or resource busy (EBUSY)
  directory_not_empty = 39,                 ///< directory not empty (ENOTEMPTY)
  executable_format_error = 8,              ///< executable format error (ENOEXEC)
  file_exists = 17,                         ///< file exists (EEXIST)
  file_too_large = 27,                      ///< file too large (EFBIG)
  filename_too_long = 36,                   ///< filename too long (ENAMETOOLONG)
  function_not_supported = 38,              ///< function not supported (ENOSYS)
  host_unreachable = 113,                   ///< host unreachable (EHOSTUNREACH)
  identifier_removed = 43,                  ///< identifier removed (EIDRM)
  illegal_byte_sequence = 84,               ///< illegal byte sequence (EILSEQ)
  inappropriate_io_control_operation = 25,  ///< inappropriate io control operation (ENOTTY)
  interrupted = 4,                          ///< interrupted (EINTR)
  invalid_argument = 22,                    ///< invalid argument (EINVAL)
  invalid_seek = 29,                        ///< invalid seek (ESPIPE)
  io_error = 5,                             ///< I/O error (EIO)
  is_a_directory = 21,                      ///< is a directory (EISDIR)
  message_size = 90,                        ///< message size (EMSGSIZE)
  network_down = 100,                       ///< network down (ENETDOWN)
  network_reset = 102,                      ///< network reset (ENETRESET)
  network_unreachable = 101,                ///< network unreachable (ENETUNREACH)
  no_buffer_space = 105,                    ///< no buffer space (ENOBUFS)
  no_child_process = 10,                    ///< no child process (ECHILD)
  no_link = 67,                             ///< no link (ENOLINK)
  no_lock_available = 37,                   ///< no lock available (ENOLCK)
  no_message_available = 61,                ///< no message available (ENODATA)
  no_message = 42,                          ///< no message (ENOMSG)
  no_protocol_option = 92,                  ///< no protocol option (ENOPROTOOPT)
  no_space_on_device = 28,                  ///< no space on device (ENOSPC)
  no_stream_resources = 63,                 ///< no stream resources (ENOSR)
  no_such_device_or_address = 6,            ///< no such device or address (ENXIO)
  no_such_device = 19,                      ///< no such device (ENODEV)
  no_such_file_or_directory = 2,            ///< no such file or directory (ENOENT)
  no_such_process = 3,                      ///< no such process (ESRCH)
  not_a_directory = 20,                     ///< not a directory (ENOTDIR)
  not_a_socket = 88,                        ///< not a socket (ENOTSOCK)
  not_a_stream = 60,                        ///< not a stream (ENOSTR)
  not_connected = 107,                      ///< not connected (ENOTCONN)
  not_enough_memory = 12,                   ///< not enough memory (ENOMEM)
  not_supported = 202,                      ///< not supported (ENOTSUP)
  operation_canceled = 125,                 ///< operation canceled (ECANCELED)
  operation_in_progress = 115,              ///< operation in progress (EINPROGRESS)
  operation_not_permitted = 1,              ///< operation not permitted (EPERM)
  operation_not_supported = 95,             ///< operation not supported (EOPNOTSUPP)
  operation_would_block = 201,              ///< operation would block (EWOULDBLOCK)
  owner_dead = 130,                         ///< owner dead (EOWNERDEAD)
  permission_denied = 13,                   ///< permission denied (EACCES)
  protocol_error = 71,                      ///< protocol error (EPROTO)
  protocol_not_supported = 93,              ///< protocol not supported (EPROTONOSUPPORT)
  read_only_file_system = 30,               ///< read-only file system (EROFS)
  resource_deadlock_would_occur = 35,       ///< resource deadlock would occur (EDEADLK)
  resource_unavailable_try_again = 11,      ///< resource unavailable, try again (EAGAIN)
  result_out_of_range = 34,                 ///< result out of range (ERANGE)
  state_not_recoverable = 131,              ///< state not recoverable (ENOTRECOVERABLE)
  stream_timeout = 62,                      ///< stream timeout (ETIME)
  text_file_busy = 26,                      ///< text file busy (ETXTBSY)
  timed_out = 110,                          ///< timed out (ETIMEDOUT)
  too_many_files_open_in_system = 23,       ///< too many files open in system (ENFILE)
  too_many_files_open = 24,                 ///< too many files open (EMFILE)
  too_many_links = 31,                      ///< too many links (EMLINK)
  too_many_symbolic_link_levels = 40,       ///< too many symbolic link levels (ELOOP)
  value_too_large = 75,                     ///< value too large (EOVERFLOW)
  wrong_protocol_type = 91,                 ///< wrong protocol type (EPROTOTYPE)
  invalid_meta_model_shortname = 137,
  invalid_meta_model_path = 138
};

/// @brief Exception type thrown for Core errors.
///
/// @uptrace{SWS_CORE_00211}
class CoreException final : public Exception {
public:
  /**
   * @brief Construct a new CoreException from an ErrorCode.
   * @param err  the ErrorCode
   *
   * @uptrace{SWS_CORE_00212}
   */
  explicit CoreException(ErrorCode err) noexcept : Exception(err) {}
};

/// @brief An error domain for Core.1 errors.
///
/// @uptrace{SWS_CORE_00221}
class CoreErrorDomain final : public ErrorDomain {
  constexpr static ErrorDomain::IdType kId = 0x8000000000000000;

public:
  /// @brief Alias for the error code value enumeration
  ///
  /// @uptrace{SWS_CORE_00231}
  using Errc = CoreErrc;

  /// @brief Alias for the exception base class
  ///
  /// @uptrace{SWS_CORE_00232}
  using Exception = CoreException;

  /// @brief Default constructor
  ///
  /// @uptrace{SWS_CORE_00241}
  /// @uptrace{SWS_CORE_00012}
  constexpr CoreErrorDomain() noexcept : ErrorDomain(kId) {}

  /// @brief Return the "shortname" ApApplicationErrorDomain.SN of this error
  /// domain.
  /// @returns "Core"
  ///
  /// @uptrace{SWS_CORE_00242}
  /// @uptrace{SWS_CORE_00012}
  const char* Name() const noexcept final { return "Core"; }

  /// @brief Translate an error code value into a text message.
  /// @param errorCode  the error code value
  /// @returns the text message, never nullptr
  ///
  /// @uptrace{SWS_CORE_00243}
  const char* Message(ErrorDomain::CodeType errorCode) const noexcept final {
    switch (static_cast<Errc>(errorCode)) {
      case Errc::address_family_not_supported: {
        return "Address family not supported";
        break;
      }
      case Errc::address_in_use: {
        return "Address in use";
        break;
      }
      case Errc::address_not_available: {
        return "Address not available";
        break;
      }
      case Errc::already_connected: {
        return "Transport endpoint is already connected";
        break;
      }
      case Errc::argument_list_too_long: {
        return "Argument list too long";
        break;
      }
      case Errc::argument_out_of_domain: {
        return "Argument out of domain";
        break;
      }
      case Errc::bad_address: {
        return "Bad address";
        break;
      }
      case Errc::bad_file_descriptor: {
        return "Bad file descriptor";
        break;
      }
      case Errc::bad_message: {
        return "Bad message";
        break;
      }
      case Errc::broken_pipe: {
        return "Broken pipe";
        break;
      }
      case Errc::connection_aborted: {
        return "Connection aborted";
        break;
      }
      case Errc::connection_already_in_progress: {
        return "Connection already in progress";
        break;
      }
      case Errc::connection_refused: {
        return "Connection refused";
        break;
      }
      case Errc::connection_reset: {
        return "Connection reset";
        break;
      }
      case Errc::cross_device_link: {
        return "Cross-device link";
        break;
      }
      case Errc::destination_address_required: {
        return "Destination address required";
        break;
      }
      case Errc::device_or_resource_busy: {
        return "Device or resource busy";
        break;
      }
      case Errc::directory_not_empty: {
        return "Directory not empty";
        break;
      }
      case Errc::executable_format_error: {
        return "Executable format error";
        break;
      }
      case Errc::file_exists: {
        return "File exists";
        break;
      }
      case Errc::file_too_large: {
        return "File too large";
        break;
      }
      case Errc::filename_too_long: {
        return "File name too long";
        break;
      }
      case Errc::function_not_supported: {
        return "Function not supported";
        break;
      }
      case Errc::host_unreachable: {
        return "Host unreachable";
        break;
      }
      case Errc::identifier_removed: {
        return "Identifier removed";
        break;
      }
      case Errc::illegal_byte_sequence: {
        return "Illegal byte sequence";
        break;
      }
      case Errc::inappropriate_io_control_operation: {
        return "Inappropriate I/O control operation";
        break;
      }
      case Errc::interrupted: {
        return "Interrupted system call";
        break;
      }
      case Errc::invalid_argument: {
        return "Remote I/O error";
        break;
      }
      case Errc::invalid_seek: {
        return "Invalid seek";
        break;
      }
      case Errc::io_error: {
        return "Remote I/O error";
        break;
      }
      case Errc::is_a_directory: {
        return "Is a directory";
        break;
      }
      case Errc::message_size: {
        return "Message too long";
        break;
      }
      case Errc::network_down: {
        return "Network is down";
        break;
      }
      case Errc::network_reset: {
        return "Network dropped connection because of reset";
        break;
      }
      case Errc::network_unreachable: {
        return "Network is unreachable";
        break;
      }
      case Errc::no_buffer_space: {
        return "No buffer space available";
        break;
      }
      case Errc::no_child_process: {
        return "No child process";
        break;
      }
      case Errc::no_link: {
        return "Link has been severed";
        break;
      }
      case Errc::no_lock_available: {
        return "No record locks available";
        break;
      }
      case Errc::no_message_available: {
        return "No data available";
        break;
      }
      case Errc::no_message: {
        return "No message of desired type";
        break;
      }
      case Errc::no_protocol_option: {
        return "Protocol not available";
        break;
      }
      case Errc::no_space_on_device: {
        return "No space left on device";
        break;
      }
      case Errc::no_stream_resources: {
        return "Device not a stream";
        break;
      }
      case Errc::no_such_device_or_address: {
        return "No such device or address";
        break;
      }
      case Errc::no_such_device: {
        return "No such device";
        break;
      }
      case Errc::no_such_file_or_directory: {
        return "No such file or directory";
        break;
      }
      case Errc::no_such_process: {
        return "No such process";
        break;
      }
      case Errc::not_a_directory: {
        return "Not a directory";
        break;
      }
      case Errc::not_a_socket: {
        return "Socket operation on non-socket";
        break;
      }
      case Errc::not_a_stream: {
        return "Device not a stream";
        break;
      }
      case Errc::not_connected: {
        return "Transport endpoint is not connected";
        break;
      }
      case Errc::not_enough_memory: {
        return "Out of memory";
        break;
      }
      case Errc::not_supported: {
        return "Not supported";
        break;
      }
      case Errc::operation_canceled: {
        return "Operation canceled";
        break;
      }
      case Errc::operation_in_progress: {
        return "Operation in progress";
        break;
      }
      case Errc::operation_not_permitted: {
        return "Operation not permitted";
        break;
      }
      case Errc::operation_not_supported: {
        return "Operation not supported";
        break;
      }
      case Errc::operation_would_block: {
        return "Operation would block";
        break;
      }
      case Errc::owner_dead: {
        return "Owner dead";
        break;
      }
      case Errc::permission_denied: {
        return "Permission denied";
        break;
      }
      case Errc::protocol_error: {
        return "Protocol error";
        break;
      }
      case Errc::protocol_not_supported: {
        return "Protocol not supported";
        break;
      }
      case Errc::read_only_file_system: {
        return "Read-only file system";
        break;
      }
      case Errc::resource_deadlock_would_occur: {
        return "Resource deadlock would occur";
        break;
      }
      case Errc::resource_unavailable_try_again: {
        return "Resource unavailable, try again";
        break;
      }
      case Errc::result_out_of_range: {
        return "Result out of range";
        break;
      }
      case Errc::state_not_recoverable: {
        return "State not recoverable";
        break;
      }
      case Errc::stream_timeout: {
        return "Stream timeout";
        break;
      }
      case Errc::text_file_busy: {
        return "Text file busy";
        break;
      }
      case Errc::timed_out: {
        return "Time out";
        break;
      }
      case Errc::too_many_files_open_in_system: {
        return "Too many files open in system";
        break;
      }
      case Errc::too_many_files_open: {
        return "Too many open files";
        break;
      }
      case Errc::too_many_links: {
        return "Too many links";
        break;
      }
      case Errc::too_many_symbolic_link_levels: {
        return "Too many symbolic links encountered";
        break;
      }
      case Errc::value_too_large: {
        return " Value too large";
        break;
      }
      case Errc::wrong_protocol_type: {
        return "Wrong protocol type";
        break;
      }
      case Errc::invalid_meta_model_shortname: {
        return "Invalid Meta Model Shortname";
        break;
      }
      case Errc::invalid_meta_model_path: {
        return "Invalid Meta Model Path";
        break;
      }
      default: {
        return "Unknown error";
        break;
      }
    }
  }

  /// @brief Throw the exception type corresponding to the given ErrorCode.
  /// @param errorCode  the ErrorCode instance
  ///
  /// @uptrace{SWS_CORE_00244}
  void ThrowAsException(const ErrorCode& errorCode) const final { ThrowOrTerminate<Exception>(errorCode); }
};

namespace internal {
constexpr CoreErrorDomain g_CoreErrorDomain;
}

/// @brief Return a reference to the global CoreErrorDomain.
/// @returns the CoreErrorDomain
///
/// @uptrace{SWS_CORE_00280}
constexpr const ErrorDomain& GetCoreErrorDomain() noexcept {
  return internal::g_CoreErrorDomain;
}

/// @brief Create a new ErrorCode within CoreErrorDomain.
///
/// This function is used internally by constructors of ErrorCode. It is usually
/// not used directly by users.
///
/// The lifetime of the text pointed to by @a message must exceed that
/// of all uses of ErrorCode::UserMessage() on the new instance.
///
/// @param code  the CoreErrorDomain-specific error code value
/// @param data  optional vendor-specific error data
/// @param message  optional user-provided message string
/// @returns a new ErrorCode instance
///
/// @uptrace{SWS_CORE_00290}
inline constexpr ErrorCode MakeErrorCode(CoreErrc code, ErrorDomain::SupportDataType data) noexcept {
  return ErrorCode(static_cast<ErrorDomain::CodeType>(code), GetCoreErrorDomain(), data);
}

}  // namespace core
}  // namespace ara

#endif  // TUSIMPLEAP_ARA_CORE_CORE_ERROR_DOMAIN_H_

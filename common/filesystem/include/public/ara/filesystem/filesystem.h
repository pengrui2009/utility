#ifndef ARA_FILESYSTEM_FILESYSTEM_H_
#define ARA_FILESYSTEM_FILESYSTEM_H_

#include "ara/core/string.h"
#include "ara/core/vector.h"
#include "ara/core/result.h"

#ifdef __linux__
struct dirent;
#endif

#ifdef __QNX__
struct dirent64;
#endif

namespace ara {
namespace filesystem {

using ara::core::String;
using ara::core::Vector;
using ara::core::Result;

using ResultVS = Result<Vector<String>>;

class Filesystem {
public:
  static const char PATH_SEPARATOR;

  /**
   * \brief Get the list of sub-directories at the path specified as parameter
   *
   * \returns A vector of strings containing the sub-directories
   */
  static ResultVS GetSubdirectories(const String& dir);

  /**
   * \brief Get recursive the list of sub-directories at the path specified as
   * parameter
   *
   * \returns A vector of strings containing the sub-directories
   */
  static ResultVS GetRecursiveSubdirectories(const String& dir);

  /**
   * \brief Get the list of files at the path specified as parameter
   *
   * \returns A vector of strings containing the file names of the files
   * available at the given path
   */
  static ResultVS GetFiles(const String& dir);

  /**
   * \brief Get the list of files at the path specified as parameter starting
   * with the given prefix
   *
   * \returns A vector of strings containing the file names of the files
   * available at the given path and starting with the given prefix
   */
  static ResultVS GetFilesStartingWith(
      const String& dir, const String& starting, bool recursive = false);

  /**
   * \brief Get recursive the list of files at the path specified as parameter
   *
   * \returns A vector of strings containing the file names of the files
   * available at the given path
   */
  static ResultVS GetRecursiveFiles(const String& dir);

  /**
   * \brief Builds the path string
   *
   * \returns A string containing the path
   */
  static String BuildPath(const String& base, const String& additional);

  /**
   * \brief Extract the file name from the given path
   *
   * \returns A string containing the file name
   */
  static String ExtractFileName(const String& prefix);

  static time_t GetFileModTime(const String& path);

  static bool Access(const String& pathname);

  static String ObtainParentPath(const String& path);

  static String RemoveFristPathSeparator(const String& path);

  static String RemoveLastPathSeparator(const String& path);

  static bool CreateDirectory(const core::String& dir);
  static Result<void> RemoveDirectory(const core::String& dir);

  static int Unzip(const core::String& package_path, const core::String& extract_dir, bool extract_withoutpath = true);
  static Result<void> Rename(core::String entry_path, core::String target_path);
  static Result<void> CopyDirectory(const core::String& source, const core::String& target);

private:
  /**
   * \brief Helper function to find the sub-directories at the given path
   *
   * \returns A vector of strings containing the sub-directory names
   */
  static ResultVS FindSubdirectories(const String& dir, bool recursive);

  /**
   * \brief Helper function to find the files at the given path
   *
   * \returns A vector of strings containing the file names
   */
  static ResultVS FindFiles(const String& dir, bool recursive);
};

}  // namespace filesystem
}  // namespace ara
#endif  // ARA_FILESYSTEM_FILESYSTEM_H_

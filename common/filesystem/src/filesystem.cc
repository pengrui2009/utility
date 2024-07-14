#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <cctype>

#include "boost/filesystem.hpp"
#include "miniunz.h"

#include "ara/core/core_error_domain.h"
#include "ara/filesystem/filesystem.h"
#include "ara/filesystem/filesystem_logger.h"
#include "ara/filesystem/common.h"
#include "ara/core/string.h"

namespace ara {
namespace filesystem {

using ara::core::CoreErrc;
/**
 * \brief Checks if current or parent directory
 *
 * \returns True if any of those directory, false otherwise
 */
bool IsCurrentOrParentDir(const String& dirname);

/**
 * \brief Helper function to find the files/sub-directories at the given path
 *
 * \param dir The path where to search for files/sub-directories.
 * \param recursive The flag if the search should be recursive or not.
 * \tparam DT The type of dir entries to be searched. Only DT_DIR and DT_REG are
 * valid values.
 *
 * \throws FilesystemException in case of error.
 *
 * \returns A vector of strings containing the file/sub-directory names.
 *
 * \note This call wrappes the opendir and readdir system calls.
 */
#ifdef __linux__ 
template <unsigned char DT>
ResultVS Find(const String& dir, bool recursive);
#endif

#ifdef __QNX__
ResultVS Find(const String& dir, bool recursive, const mode_t type);
#endif

/**
 * \brief Check if the passed dir entry has the specified type
 *
 * \param dirEntry The pointer to struct dirent which should be checked for the
 * specified type. \param baseDir The base path for dirEntry. \tparam DT The
 * type to be checked. Only DT_DIR and DT_REG are valid values.
 *
 * \returns True is the passed dir entry has the specified type, false otherwise
 */
#ifdef __linux__
template <unsigned char DT>
Result<bool> IsSameType(const dirent& dir_entry, const String& base_dir);
#endif
#ifdef __QNX__
Result<bool> IsSameType(const dirent64& dir_entry, const String& base_dir, const mode_t type);
Result<bool> IsSameType(const String& full_entry_path, const mode_t type);
#endif

const char Filesystem::PATH_SEPARATOR = '/';

ResultVS Filesystem::GetFiles(const String& dir) {
  return FindFiles(dir, false);
}

ResultVS Filesystem::GetFilesStartingWith(const String& dir, const String& starting, bool recursive) {
  ResultVS files{Vector<String>{}};
  if (recursive) {
    files = GetRecursiveFiles(dir);
  } else {
    files = GetFiles(dir);
  }
  if (!files) {
    return files;
  }
  Vector<String> retFiles;
  for (auto file : *files) {
    if (!ExtractFileName(file).compare(0, starting.size(), starting)) {
      retFiles.push_back(file);
    }
  }
  return ResultVS(retFiles);
}

ResultVS Filesystem::GetRecursiveFiles(const String& dir) {
  return FindFiles(dir, true);
}

ResultVS Filesystem::GetRecursiveSubdirectories(const String& dir) {
  return FindSubdirectories(dir, true);
}

ResultVS Filesystem::GetSubdirectories(const String& dir) {
  return FindSubdirectories(dir, false);
}

ResultVS Filesystem::FindFiles(const String& dir, bool recursive) {
#ifdef __linux__
  return Find<DT_REG>(dir, recursive);
#endif 
#ifdef __QNX__ 
  return Find(dir, recursive, S_IFREG);
#endif
}

ResultVS Filesystem::FindSubdirectories(const String& dir, bool recursive) {
#ifdef __linux__
  return Find<DT_DIR>(dir, recursive);
#endif 
#ifdef __QNX__ 
  return Find(dir, recursive, S_IFDIR);
#endif
}

String Filesystem::BuildPath(const String& base, const String& additional) {
  if (base.back() != PATH_SEPARATOR) {
    return BuildPath(base + PATH_SEPARATOR, additional);
  }
  if (additional.front() == PATH_SEPARATOR) {
    return BuildPath(base, RemoveFristPathSeparator(additional));
  }
  return base + additional;
}

String Filesystem::ExtractFileName(const String& prefix) {
  String application_name(prefix);
  ara::filesystem::Trim(application_name, PATH_SEPARATOR);
  size_t last_path_separator_position(
      application_name.find_last_of(PATH_SEPARATOR));
  if (last_path_separator_position == String::npos) {
    return application_name;
  } else {
    return application_name.substr(
        last_path_separator_position + 1, String::npos);
  }
}

time_t Filesystem::GetFileModTime(const String& path) {
#ifdef __linux__
  struct stat stbuf;
  stat(path.c_str(), &stbuf);
#endif
#ifdef __QNX__
  struct stat64 stbuf;
  stat64(path.c_str(), &stbuf);
#endif
  return stbuf.st_mtime;
}

bool Filesystem::Access(const String& pathname) {
  return access(pathname.c_str(), R_OK | W_OK) == 0;
}

String Filesystem::ObtainParentPath(const String& path) {
  if (path.back() == PATH_SEPARATOR) {
    return ObtainParentPath(RemoveLastPathSeparator(path));
  }
  return path.substr(0, path.find_last_of(PATH_SEPARATOR));
}

String Filesystem::RemoveFristPathSeparator(const String& path) {
  if (path.front() == PATH_SEPARATOR) {
    return RemoveFristPathSeparator(path.substr(1, path.size() - 1));
  }
  return path;
}

String Filesystem::RemoveLastPathSeparator(const String& path) {
  if (path.back() == PATH_SEPARATOR) {
    return RemoveLastPathSeparator(path.substr(0, path.size() - 1));
  }
  return path;
}

#ifdef __linux__
template <unsigned char DT>
ResultVS Find(const String& dir, bool recursive) {
  DIR* directory_stream = opendir(dir.c_str());
  if (NULL == directory_stream) {
    LOG_WARN() << "opendir failed dir:" << dir;
    return ResultVS(CoreErrc::no_such_file_or_directory);
  }

  Vector<String> entries;
  dirent* result = NULL;
  dirent directory_entry = {0};

  int error_number = readdir_r(directory_stream, &directory_entry, &result);
  while (0 == error_number && result != NULL) {
    if (!IsCurrentOrParentDir(directory_entry.d_name)) {
      auto res = IsSameType<DT>(directory_entry, dir);
      if (res && res.Value()) {
        entries.push_back(Filesystem::BuildPath(dir, directory_entry.d_name));
      }
    }

    error_number = readdir_r(directory_stream, &directory_entry, &result);
  }

  if (0 != error_number) {
    LOG_DEBUG() << "ara::filesystem::Find() error:" << error_number;
  }

  closedir(directory_stream);
  return entries;
}
#endif
#ifdef __QNX__
ResultVS Find(const String& dir, bool recursive, const mode_t type) {
  DIR* directory_stream = opendir(dir.c_str());
  if (NULL == directory_stream) {
    LOG_WARN() << "opendir failed dir:" << dir;
    return ResultVS::FromError(ara::core::CoreErrc::no_such_file_or_directory);
  }

  Vector<String> entries;

  struct dirent64* directory_entry = NULL;
  while (true) {
    directory_entry = readdir64(directory_stream);
    if (directory_entry == NULL) {
      break;
    }
    String directory_entry_name(
        directory_entry->d_name, directory_entry->d_namelen);
    if (!IsCurrentOrParentDir(directory_entry_name)) {
      auto res = IsSameType(*directory_entry, dir, type);
      if (res && res.Value()) {
        entries.push_back(Filesystem::BuildPath(dir, directory_entry_name));
      }
    }
  }

  closedir(directory_stream);

  return entries;
}
#endif

bool IsCurrentOrParentDir(const String& dirname) {
  return dirname == "." || dirname == "..";
}

#ifdef __linux__
template <unsigned char DT>
Result<bool> IsSameType(const String& full_entry_path) {
  if (DT != DT_REG && DT != DT_DIR && DT != DT_LNK) {
    LOG_WARN() << "Only DT_REG / DT_LNK / DT_DIR are supported.";
    return Result<bool>(CoreErrc::not_supported);
  }

  struct stat stat_buf {
    0
  };

  if (stat(full_entry_path.c_str(), &stat_buf) != 0) {
    LOG_WARN() << "Stat: " << full_entry_path << "failed, errno: " << errno;
    return Result<bool>(CoreErrc::no_such_file_or_directory);
  }

  if (S_ISLNK(stat_buf.st_mode)) {
    char buf[1024];
    ssize_t link_size = readlink(full_entry_path.c_str(), buf, 1024);
    if (link_size == -1) {
      return false;
    }
    String link_full_path(buf, link_size);
    return IsSameType<DT>(link_full_path);
  }

  const bool isSame =
      ((DT_DIR == DT && S_ISDIR(stat_buf.st_mode)) ||
       (DT_REG == DT && S_ISREG(stat_buf.st_mode)));
  return isSame;
}

template <unsigned char DT>
Result<bool> IsSameType(const dirent& dir_entry, const String& base_dir) {
  const String full_entry_path =
      Filesystem::BuildPath(base_dir, dir_entry.d_name);

  if (DT_UNKNOWN != dir_entry.d_type) {
    if (dir_entry.d_type == DT_LNK) {
      return IsSameType<DT>(full_entry_path);
    }
    return DT == dir_entry.d_type;
  }
  return IsSameType<DT>(full_entry_path);
}
#endif
#ifdef __QNX__   
Result<bool> IsSameType(const String& full_entry_path, const mode_t type) {
  if (type != S_IFREG && type != S_IFDIR && type != S_IFLNK) {
    LOG_WARN() << "Only S_IFREG and S_IFDIR and S_IFLNK are supported.";
    return Result<bool>::FromError(ara::core::CoreErrc::not_supported);
  }

  struct stat64 stat_buf {
    0
  };

  if (stat64(full_entry_path.c_str(), &stat_buf) != 0) {
    LOG_WARN() << "Stat: " << full_entry_path << "failed, errno: " << errno;
    return Result<bool>(ara::core::CoreErrc::no_such_file_or_directory);
  }

  if (S_ISLNK(stat_buf.st_mode)) {
    char buf[1024];
    ssize_t link_size = readlink(full_entry_path.c_str(), buf, 1024);
    if (link_size == -1) {
      return false;
    }
    String link_full_path(buf, link_size);
    return IsSameType(link_full_path, type);
  }

  const bool isSame =
      ((type == S_IFDIR && S_ISDIR(stat_buf.st_mode)) ||
       (type == S_IFREG && S_ISREG(stat_buf.st_mode)));
  return isSame;
}

Result<bool> IsSameType(const dirent64& dir_entry, const String& base_dir, const mode_t type) {
  const String full_entry_path =
      Filesystem::BuildPath(base_dir, dir_entry.d_name);

  return IsSameType(full_entry_path, type);
}
#endif

bool Filesystem::CreateDirectory(const core::String& dir) {
  if (Access(dir)) {
    return true;
  }
  return boost::filesystem::create_directories(dir.c_str());
}

Result<void> Filesystem::RemoveDirectory(const core::String& dir) {
  if (!Filesystem::Access(dir)) {
    return Result<void>(CoreErrc::no_such_file_or_directory);
  }
  boost::system::error_code ec;
  const auto deleted_items_num = boost::filesystem::remove_all(dir.c_str(), ec);
  if (ec || 0 == deleted_items_num) {
    return core::Result<void>(CoreErrc::operation_not_permitted);
  }

  return {};
}

int Filesystem::Unzip(const core::String& package_path, const core::String& extract_dir, bool extract_withoutpath) {
  //TODO(bin.wang) replace system api
  int ret = 0;
  unzFile uf = NULL;

  char* curr_realpath;
  char* pack_realpath;

  curr_realpath = (char*)calloc(PATH_MAX, sizeof(char));
  pack_realpath = (char*)calloc(PATH_MAX, sizeof(char));
  realpath("./", curr_realpath);
  realpath(package_path.c_str(), pack_realpath);

  if (!extract_dir.empty()) {
    if (chdir(extract_dir.c_str()) != 0) {
      free(curr_realpath);
      free(pack_realpath);
      return -1;
    }
  }
  
  do {
    // need to close and then open. Otherwise, continuous operations will cause errors.
    // For example, if you unzip the list first, directly, do extract, an error will occur
    uf = unzOpen64(pack_realpath);

    if (!uf) {
      ret = -3;
      break;
    }
    int opt_do_extract_withoutpath = static_cast<int>(extract_withoutpath);
    int opt_overwrite_withoutpromoting = 1;
    ret = do_extract(uf, opt_do_extract_withoutpath, opt_overwrite_withoutpromoting, NULL);
    unzClose(uf);

    sync();
  } while (0);

  if (!extract_dir.empty()) {
    if (chdir(curr_realpath) != 0) {
      ret = -4;
    }
  }

  free(curr_realpath);
  free(pack_realpath);

  return ret;
}

Result<void> Filesystem::Rename(core::String entry_path, core::String target_path) {
  boost::system::error_code ec;
  boost::filesystem::rename(entry_path, target_path.c_str(), ec);
  if (ec) {
    return Result<void>(CoreErrc::operation_not_permitted);
  }
  return {};
}

Result<void> Filesystem::CopyDirectory(const core::String& source, const core::String& target) {
  if (!Filesystem::Access(source)) {
    return Result<void>(CoreErrc::no_such_file_or_directory);
  }
  if (Filesystem::Access(target)) {
    auto result = Filesystem::RemoveDirectory(target);
    if (!result) {
      return result;
    }
  }
  if (!Filesystem::CreateDirectory(target)) {
    return Result<void>(CoreErrc::operation_not_permitted);
  }

  boost::system::error_code ec;
  const auto dir_iterator = boost::filesystem::recursive_directory_iterator(source.c_str(), ec);
  if (ec) {
    return Result<void>(CoreErrc::operation_not_permitted);
  }
  for (const auto& entry : dir_iterator) {
    ec.clear();
    auto source_path = entry.path().string<std::string>();
    auto relative_path = Filesystem::RemoveFristPathSeparator(source_path.substr(source.size()));
    auto target_path = Filesystem::BuildPath(target, relative_path);
#ifdef __linux__
    boost::filesystem::copy(source_path, target_path, ec);
#endif
#ifdef __QNX__
    boost::filesystem::copy(source_path, target_path, boost::filesystem::copy_options::overwrite_existing, ec);
#endif
    if (ec) {
      return Result<void>(CoreErrc::operation_not_permitted);
    }
  }
  return {};
}

}  //  namespace filesystem
}  //  namespace ara
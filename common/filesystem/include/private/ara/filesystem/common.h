#ifndef ARA_FILESYSTEM_COMMON_H_
#define ARA_FILESYSTEM_COMMON_H_

namespace ara {
namespace filesystem {

inline void Trim(String& text, const char char_to_trim) {
  size_t first_char(text.find_first_not_of(char_to_trim));
  size_t last_char(text.find_last_not_of(char_to_trim));
  if (first_char == 0 && last_char == text.length() - 1) {
    return;
  } else if (
      (first_char == String::npos && last_char == String::npos) ||
      (last_char < first_char)) {
    // This string is either empty or contains nothing but trimmable characters
    text = String();
  } else {
    text = text.substr(first_char, last_char - first_char + 1);
  }
}
}  // namespace filesystem
}  // namespace ara

#endif  // ARA_FILESYSTEM_COMMON_H_
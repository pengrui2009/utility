#pragma once

#include <ara/core/string.h>
#include <sys/time.h>

namespace ara {
namespace ipc {
namespace utilites {

using String = ara::core::String;

inline uint64_t get_timestamp() {
  struct timeval tv;
  struct timezone tz;

  uint64_t system_timestamp;
  if (0 == gettimeofday(&tv, &tz)) {
    system_timestamp =
        static_cast<uint64_t>(tv.tv_sec * 1000 + (tv.tv_usec / 1000));
    return system_timestamp;
  } else {
    return 0UL;
  }
  // return 0;
}

inline uint32_t bytes_to_uint(const char* bytes) {
#if (BYTE_ORDER == BIG_ENDIAN)
  uint32_t r = uint32_t((*bytes << 24) + (*(bytes + 1) << 16) +
                        (*(bytes + 2) << 8) + *(bytes + 3));
#else
  uint32_t r = uint32_t((static_cast<uint8_t>(*(bytes + 3)) << 24) +
                        (static_cast<uint8_t>(*(bytes + 2)) << 16)) +
               (static_cast<uint8_t>(*(bytes + 1)) << 8) +
               static_cast<uint8_t>(*bytes);
#endif
  return r;
}

inline uint64_t bytes_to_ulong(const char* bytes) {
  uint64_t r;
#if (BYTE_ORDER == BIG_ENDIAN)
  r = static_cast<uint8_t>(*bytes);
  r = (r << 8) + static_cast<uint8_t>((*(bytes + 1)));
  r = (r << 8) + static_cast<uint8_t>((*(bytes + 2)));
  r = (r << 8) + static_cast<uint8_t>((*(bytes + 3)));
  r = (r << 8) + static_cast<uint8_t>((*(bytes + 4)));
  r = (r << 8) + static_cast<uint8_t>((*(bytes + 5)));
  r = (r << 8) + static_cast<uint8_t>((*(bytes + 6)));
  r = (r << 8) + static_cast<uint8_t>((*(bytes + 7)));
#else
  r = static_cast<uint8_t>(*(bytes + 7));
  r = (r << 8) + static_cast<uint8_t>((*(bytes + 6)));
  r = (r << 8) + static_cast<uint8_t>((*(bytes + 5)));
  r = (r << 8) + static_cast<uint8_t>((*(bytes + 4)));
  r = (r << 8) + static_cast<uint8_t>((*(bytes + 3)));
  r = (r << 8) + static_cast<uint8_t>((*(bytes + 2)));
  r = (r << 8) + static_cast<uint8_t>((*(bytes + 1)));
  r = (r << 8) + static_cast<uint8_t>(*bytes);

#endif
  return r;
}

inline String get_Datetime(const uint64_t& timestamp_ms) {
  char szTime[25];
  time_t temp = timestamp_ms / 1000;
  struct tm* timeSet = localtime(&temp);
  strftime(szTime, sizeof(szTime) - 1, "%F %T", timeSet);
  return String(szTime);
}

}  // namespace utilites
}  // namespace ipc
}  // namespace ara
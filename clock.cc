#include "clock.h"

#include <time.h>
#include <stdexcept>
#include <string>

uint64_t clock_monotonic() {
  struct timespec t;
  if (clock_gettime(CLOCK_MONOTONIC_COARSE, &t) != 0) {
    throw std::runtime_error("clock_gettime(CLOCK_MONOTONIC_COARSE) failed");
  }


  return t.tv_sec * 1000 + t.tv_nsec / 1'000'000;
}

uint64_t clock_elapsed(uint64_t at, uint64_t since) {
  if (at > since) {
    return at - since;
  } else {
    return 0;
  }
}

std::string clock_isodate() {
  auto t = time(nullptr);
  if (t == ((time_t) -1)) {
    throw std::runtime_error("time() failed");
  }

  struct tm time_info;
  if (localtime_r(&t, &time_info) == NULL) {
    throw std::runtime_error("localtime() failed");
  }

  char time_text[80];
  if (strftime(time_text, 80, "%F %H:%M:%S", &time_info) == 0) {
    throw std::runtime_error("strftime() failed");
  }

  return time_text;
}

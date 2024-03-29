#include "output_tty.h"
#include <fmt/core.h>

std::string tty_print_success(const std::string& s) {
  return fmt::format("\033[1;42m{}\033[0m", s);
}

std::string tty_print_warning(const std::string& s) {
  return fmt::format("\033[1;43m{}\033[0m", s);
}

std::string tty_print_error(const std::string& s) {
  return fmt::format("\033[1;41m{}\033[0m", s);
}

std::string tty_print_value_bytes(uint64_t x) {
  if (x < 1'000) {
    return fmt::format("{}B", x);
  } else if (x < 1'000'000) {
    return fmt::format("{:.3f}KB", x / double(1'000));
  } else if (x < 1'000'000'000) {
    return fmt::format("{:.3f}MB", x / double(1'000'000));
  } else if (x < 1'000'000'000'000) {
    return fmt::format("{:.3f}GB", x / double(1'000'000'000));
  } else {
    return fmt::format("{:.3f}TB", x / double(1'000'000'000'000));
  }
}

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


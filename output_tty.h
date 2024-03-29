#pragma once
#include <string>

std::string tty_print_success(const std::string& s);
std::string tty_print_warning(const std::string& s);
std::string tty_print_error(const std::string& s);
std::string tty_print_value_bytes(uint64_t x);

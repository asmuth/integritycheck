#pragma once
#include <cstdint>
#include <string>

#include "verify.h"

namespace output_tty {

std::string print_success(const std::string& s);
std::string print_warning(const std::string& s);
std::string print_error(const std::string& s);
std::string print_value_bytes(uint64_t x);

void print_result(const VerifyResult& result);

} // namespace output_tty

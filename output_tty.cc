#include "output_tty.h"

#include <fmt/core.h>
#include <iostream>

namespace output_tty {

std::string print_success(const std::string& s) {
  return fmt::format("\033[1;42m{}\033[0m", s);
}

std::string print_warning(const std::string& s) {
  return fmt::format("\033[1;43m{}\033[0m", s);
}

std::string print_error(const std::string& s) {
  return fmt::format("\033[1;41m{}\033[0m", s);
}

std::string print_value_bytes(uint64_t x) {
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

void print_result(const VerifyResult& result) {
  std::string code;
  switch (op_verify_result_status(result)) {
    case VerifyResultStatus::PASS:
      code = print_success("PASS");
      break;
    case VerifyResultStatus::WARN:
      code = print_warning("WARN");
      break;
    case VerifyResultStatus::FAIL:
      code = print_error("FAIL");
      break;
  }

  std::cout << fmt::format(
    "status: {}\n"
    "index:  {} files, {}\n"
    "tree:   {} files\n"
    "diff:   {} files\n",
    code,
    result.index_file_count,
    print_value_bytes(result.index_file_size),
    result.tree_file_count,
    result.diff.size()
  );

  if (result.diff.size() > 0) {
    std::cout << std::endl;
  }

  for (const auto& msg : result.diff) {
    switch (msg.type) {
      case VerifyDiffType::EXTRA:
        std::cout << fmt::format("- {} (extraneous file)", msg.path) << std::endl;
        break;
      case VerifyDiffType::MISSING:
        std::cout << fmt::format("- {} (missing file)", msg.path) << std::endl;
        break;
      case VerifyDiffType::CONFLICT_SIZE:
        std::cout << fmt::format("- {} (size mismatch)", msg.path) << std::endl;
        break;
      case VerifyDiffType::CONFLICT_DATA:
        std::cout << fmt::format("- {} (data mismatch)", msg.path) << std::endl;
        break;
    }
  }
}

} // namespace output_tty

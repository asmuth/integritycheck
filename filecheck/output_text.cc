#include "output_text.h"

#include <fmt/core.h>
#include <iostream>

namespace output_text {

void print_result(const VerifyResult& result) {
  if (result.diff.size() == 0) {
    std::cout << "nodiff" << std::endl;
    return;
  }

  for (const auto& msg : result.diff) {
    switch (msg.type) {
      case VerifyDiffType::EXTRA:
        std::cout << fmt::format("diff extra {}", msg.path) << std::endl;
        break;
      case VerifyDiffType::MISSING:
        std::cout << fmt::format("diff missing {}", msg.path) << std::endl;
        break;
      case VerifyDiffType::CONFLICT_SIZE:
        std::cout << fmt::format("diff size {}", msg.path) << std::endl;
        break;
      case VerifyDiffType::CONFLICT_DATA:
        std::cout << fmt::format("diff data {}", msg.path) << std::endl;
        break;
    }
  }
}

}

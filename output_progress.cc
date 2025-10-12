#include "output_progress.h"

#include "clock.h"
#include "output_tty.h"

#include <iostream>
#include <fmt/core.h>

void output_progress(const VerifyResult& result) {
  auto progress_text =  fmt::format(
    "[{}] index: {} ({}), tree: {}, check: {} ({}), {:.2f}%",
    clock_isodate(),
    result.index_file_count,
    output_tty::print_value_bytes(result.index_file_size),
    result.tree_file_count,
    result.verified_file_count,
    output_tty::print_value_bytes(result.verified_file_size),
    result.verified_file_size / double(result.index_file_size) * 100
  );

  std::cerr
    << progress_text
    << std::endl;
}

void output_progress_bind(VerifyOp* op) {
  auto time_last = 0;

  op->progress = [time_last] (const auto& result) mutable {
    if (auto t = clock_monotonic(); clock_elapsed(t, time_last) > 1000) {
      output_progress(result);
      time_last = t;
    }
  };
}

void output_progress_flush() {
  std::cerr << std::endl;
}

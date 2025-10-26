#include "output_progress.h"

#include "clock.h"
#include "output_tty.h"

#include <iostream>
#include <fmt/core.h>

namespace output_progress {

void print(const Progress& progress) {
  auto progress_text =  fmt::format(
    "[{}] {:.2f}%, {}/{}, {}/{} files",
    clock_isodate(),
    progress.bytes_total > 0
      ? progress.bytes / double(progress.bytes_total) * 100
      : 0,
    output_tty::print_value_bytes(progress.bytes),
    output_tty::print_value_bytes(progress.bytes_total),
    progress.files,
    progress.files_total
  );

  std::cerr
    << progress_text
    << std::endl;
}

void bind(ProgressFn* fn) {
  auto time_last = 0;

  *fn = [time_last] (const auto& result, bool flush) mutable {
    if (auto t = clock_monotonic(); clock_elapsed(t, time_last) > 1000 || flush) {
      print(result);
      time_last = t;
    }
  };
}

} // namespace output_progress

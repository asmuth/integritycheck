#pragma once
#include <cstdint>
#include <functional>

struct Progress {
  uint64_t files;
  uint64_t files_total;
  uint64_t bytes;
  uint64_t bytes_total;
};

using ProgressFn = std::function<void (const Progress& p, bool flush)>;

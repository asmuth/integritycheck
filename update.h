#pragma once
#include <filesystem>
#include <functional>

#include "checksum.h"
#include "progress.h"

struct UpdateOp {
  std::filesystem::path index_path;
  std::filesystem::path root_path;
  ChecksumType checksum_type;
  ProgressFn progress;
};

void update_index(const UpdateOp& op);

#pragma once
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "progress.h"

// Describes the type of difference between index and actual files
enum class VerifyDiffType {

  // A file was listed in the index, but not found in the filesystem
  MISSING,

  // A file was found in the filesystem, but not listed in the index
  EXTRA,

  // A file's data checksum does not match the checksum listed in the index
  CONFLICT_DATA,

  // A file's size does not match the size listed in the index
  CONFLICT_SIZE,

};

struct VerifyDiff {
  VerifyDiffType type;
  std::string path;
};

enum class VerifyResultStatus : int {
  PASS = 1,
  WARN = 2,
  FAIL = 3
};

struct VerifyResult {
  uint64_t index_file_count;
  uint64_t index_file_size;
  uint64_t tree_file_count;
  uint64_t verified_file_count;
  uint64_t verified_file_size;
  std::optional<VerifyResultStatus> status;
  std::vector<VerifyDiff> diff;
};

struct VerifyOp {
  std::filesystem::path root_path;
  std::filesystem::path index_path;
  ProgressFn progress;
};

VerifyResult op_verify(const VerifyOp& op);

VerifyResultStatus op_verify_result_status(const VerifyResult& result);

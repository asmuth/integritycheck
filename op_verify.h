#pragma once
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <vector>

struct VerifyOp {
  std::filesystem::path index_path;
  std::filesystem::path root_path;
};

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
  uint64_t total_file_count;
  uint64_t total_file_size;
  std::optional<VerifyResultStatus> status;
  std::vector<VerifyDiff> diff;
};

enum class VerifyOutputType {
  TTY, TEXT
};

VerifyResult op_verify(const VerifyOp& op);

int op_verify(char** args, size_t arg_count);

#pragma once
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

struct VerifyOp {
  std::filesystem::path index_path;
  std::filesystem::path root_path;
};

enum class VerifyMessageType {
  MISSING,
  CORRUPT_DATA,
  CORRUPT_SIZE
};

struct VerifyMessage {
  VerifyMessageType type;
  std::string path;
};

struct VerifyResult {
  size_t count_ok;
  size_t count_missing;
  size_t count_corrupt;

  std::vector<VerifyMessage> messages;
};

VerifyResult op_verify(const VerifyOp& op);

void op_verify(char** args, size_t arg_count);

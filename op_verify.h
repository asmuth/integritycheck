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

enum class VerifyMessageType {
  MISSING,
  CORRUPT_DATA,
  CORRUPT_SIZE,
  OMITTED
};

struct VerifyMessage {
  VerifyMessageType type;
  std::string path;
};

enum class VerifyResultStatus : int {
  PASS = 1,
  WARN = 2,
  FAIL = 3
};

struct VerifyResult {
  std::optional<VerifyResultStatus> status;
  std::vector<VerifyMessage> messages;
};

enum class VerifyOutputType {
  TTY, TEXT
};

VerifyResult op_verify(const VerifyOp& op);

int op_verify(char** args, size_t arg_count);

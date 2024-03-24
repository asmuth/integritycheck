#include "op_verify.h"
#include "index.h"

#include <array>
#include <filesystem>
#include <getopt.h>
#include <iostream>
#include <stdexcept>

void op_verify_result_add_ok(VerifyResult* result) {
  result->count_ok++;
}

void op_verify_result_add_missing(const std::string& path, VerifyResult* result) {
  result->count_missing++;
  result->messages.push_back(VerifyMessage {
    .type = VerifyMessageType::MISSING,
    .path = path
  });
}

void op_verify_result_add_corrupt_size(const std::string& path, VerifyResult* result) {
  result->count_corrupt++;
  result->messages.push_back(VerifyMessage {
    .type = VerifyMessageType::CORRUPT_SIZE,
    .path = path
  });
}

void op_verify_result_add_corrupt_data(const std::string& path, VerifyResult* result) {
  result->count_corrupt++;
  result->messages.push_back(VerifyMessage {
    .type = VerifyMessageType::CORRUPT_DATA,
    .path = path
  });
}

bool op_verify_record_checksums(const VerifyOp& op, const IndexRecord& record) {
  if (record.checksums.empty()) {
    return false;
  }

  for (const auto& checksum_expected : record.checksums) {
    auto checksum_actual = checksum_compute_sha1(op.root_path / record.path);
    if (!checksum_compare(checksum_actual, checksum_expected)) {
      return false;
    }
  }

  return true;
}

void op_verify_record(
  const VerifyOp& op,
  const IndexRecord& record,
  VerifyResult* result
) {
  if (!std::filesystem::exists(op.root_path / record.path)) {
    op_verify_result_add_missing(record.path, result);
    return;
  }

  if (std::filesystem::file_size(op.root_path / record.path) != record.size) {
    op_verify_result_add_corrupt_size(record.path, result);
    return;
  }

  if (!op_verify_record_checksums(op, record)) {
    op_verify_result_add_corrupt_data(record.path, result);
    return;
  }

  op_verify_result_add_ok(result);
}

VerifyResult op_verify(const VerifyOp& op) {
  VerifyResult result;
  result.count_ok = 0;
  result.count_missing = 0;
  result.count_corrupt = 0;

  Index index;
  index_read(op.index_path, &index);

  for (const auto& record : index.entries) {
    op_verify_record(op, record, &result);
  }

  return result;
}

void op_verify(char** args, size_t arg_count) {
  VerifyOp op;

  auto opts_short = std::string("i:");
  auto opts_long = std::array<struct option, 2>{{
    {"index", required_argument, 0, 'i'},
    {0, 0, 0, 0}
  }};

  for (;;) {
    int opt_long = 0;
    int opt = getopt_long(
        arg_count,
        args,
        opts_short.c_str(),
        opts_long.data(),
        &opt_long);

    if (opt == -1) {
      break;
    }

    switch (opt) {
      case 'i':
        op.index_path = optarg;
        break;
    }
  }

  if (op.index_path.empty()) {
    throw std::runtime_error("need an index (--index)");
  }

  auto op_result = op_verify(op);

  for (const auto& msg : op_result.messages) {
    switch (msg.type) {
      case VerifyMessageType::MISSING:
        std::cerr << "[WARN] missing file: " << msg.path << std::endl;
        break;
      case VerifyMessageType::CORRUPT_SIZE:
        std::cerr << "[WARN] corrupt file (invalid size): " << msg.path << std::endl;
        break;
      case VerifyMessageType::CORRUPT_DATA:
        std::cerr << "[WARN] corrupt file (invalid data): " << msg.path << std::endl;
        break;
    }
  }

  std::cerr << "OK: " << op_result.count_ok << std::endl;
  std::cerr << "Missing: " << op_result.count_missing << std::endl;
  std::cerr << "Corrupt: " << op_result.count_corrupt << std::endl;
}

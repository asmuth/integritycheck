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

void op_verify_result_add_omitted(const std::string& path, VerifyResult* result) {
  result->count_omit++;
  result->messages.push_back(VerifyMessage {
    .type = VerifyMessageType::OMITTED,
    .path = path
  });
}

bool op_verify_record_checksums(const VerifyOp& op, const IndexRecord& record) {
  if (record.checksums.empty()) {
    return false;
  }

  for (const auto& checksum_expected : record.checksums) {
    auto checksum_actual = checksum_compute(
      op.root_path / record.path,
      checksum_expected.type
    );

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

void op_verify_tree(
  const VerifyOp& op,
  const Index& index,
  VerifyResult* result
) {
  auto index_path_set = index_build_path_set(index);

  auto tree_iter = std::filesystem::recursive_directory_iterator(op.root_path);
  for (const auto& tree_path_absolute : tree_iter) {
    auto tree_path = std::filesystem::relative(tree_path_absolute, op.root_path);

    if (std::filesystem::is_directory(tree_path_absolute)) {
      continue;
    }

    if (!index_path_set.paths.contains(tree_path)) {
      op_verify_result_add_omitted(tree_path, result);
    }
  }
}

VerifyResult op_verify(const VerifyOp& op) {
  VerifyResult result;
  result.count_ok = 0;
  result.count_missing = 0;
  result.count_corrupt = 0;
  result.count_omit = 0;

  Index index;
  index_read(op.index_path, &index);

  op_verify_tree(op, index, &result);

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

  if (op.root_path.empty()) {
    op.root_path = std::filesystem::current_path();
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
      case VerifyMessageType::OMITTED:
        std::cerr << "[WARN] file not in index: " << msg.path << std::endl;
        break;
    }
  }

  std::cerr << "OK: " << op_result.count_ok << std::endl;
  std::cerr << "Missing: " << op_result.count_missing << std::endl;
  std::cerr << "Corrupt: " << op_result.count_corrupt << std::endl;
  std::cerr << "Omitted: " << op_result.count_omit << std::endl;
}

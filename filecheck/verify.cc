#include "verify.h"

#include "clock.h"
#include "index.h"
#include "progress.h"

#include <array>
#include <filesystem>
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <fmt/core.h>

void op_verify_progress(const VerifyOp& op, const VerifyResult& result, bool flush) {
  if (op.progress) {
    op.progress(
      Progress {
        .files = result.verified_file_count,
        .files_total = result.index_file_count,
        .bytes = result.verified_file_size,
        .bytes_total = result.index_file_size,
      },
      flush
    );
  }
}

void op_verify_result_update_status(VerifyResult* result, VerifyResultStatus status) {
  if (!result->status || status > *result->status) {
    result->status = status;
  }
}

void op_verify_result_add_match(VerifyResult* result) {
  op_verify_result_update_status(result, VerifyResultStatus::PASS);
}

void op_verify_result_add_missing(const std::string& path, VerifyResult* result) {
  op_verify_result_update_status(result, VerifyResultStatus::FAIL);

  result->diff.push_back(VerifyDiff {
    .type = VerifyDiffType::MISSING,
    .path = path
  });
}

void op_verify_result_add_conflict_size(const std::string& path, VerifyResult* result) {
  op_verify_result_update_status(result, VerifyResultStatus::FAIL);

  result->diff.push_back(VerifyDiff {
    .type = VerifyDiffType::CONFLICT_SIZE,
    .path = path
  });
}

void op_verify_result_add_conflict_data(const std::string& path, VerifyResult* result) {
  op_verify_result_update_status(result, VerifyResultStatus::FAIL);

  result->diff.push_back(VerifyDiff {
    .type = VerifyDiffType::CONFLICT_DATA,
    .path = path
  });
}

void op_verify_result_add_extraneous(const std::string& path, VerifyResult* result) {
  op_verify_result_update_status(result, VerifyResultStatus::WARN);

  result->diff.push_back(VerifyDiff {
    .type = VerifyDiffType::EXTRA,
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
    op_verify_result_add_conflict_size(record.path, result);
    return;
  }

  if (!op_verify_record_checksums(op, record)) {
    op_verify_result_add_conflict_data(record.path, result);
    return;
  }

  op_verify_result_add_match(result);
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
      op_verify_result_add_extraneous(tree_path, result);
    }

    result->tree_file_count++;

    op_verify_progress(op, *result, false);
  }
}

void op_verify_index(
  const VerifyOp& op,
  const Index& index,
  VerifyResult* result
) {
  for (const auto& record : index.entries) {
    op_verify_record(op, record, result);

    result->verified_file_count += 1;
    result->verified_file_size += record.size;

    op_verify_progress(op, *result, false);
  }
}

VerifyResultStatus op_verify_result_status(const VerifyResult& result) {
  return result.status.value_or(VerifyResultStatus::WARN);
}

VerifyResult op_verify(const VerifyOp& op) {
  Index index;
  index_read(op.index_path, &index);

  VerifyResult result;
  result.index_file_count = index_total_file_count(index);
  result.index_file_size = index_total_file_size(index);
  result.tree_file_count = 0;
  result.verified_file_count = 0;
  result.verified_file_size = 0;

  op_verify_progress(op, result, false);

  op_verify_tree(op, index, &result);
  op_verify_index(op, index, &result);

  op_verify_progress(op, result, true);

  return result;
}

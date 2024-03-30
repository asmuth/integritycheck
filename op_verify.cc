#include "op_verify.h"
#include "index.h"
#include "output_tty.h"

#include <array>
#include <filesystem>
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <fmt/core.h>

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
  }
}

VerifyResultStatus op_verify_result_status(const VerifyResult& result) {
  return result.status.value_or(VerifyResultStatus::WARN);
}

VerifyResult op_verify(const VerifyOp& op) {
  Index index;
  index_read(op.index_path, &index);

  VerifyResult result;
  result.total_file_count = index_total_file_count(index);
  result.total_file_size = index_total_file_size(index);

  op_verify_tree(op, index, &result);

  for (const auto& record : index.entries) {
    op_verify_record(op, record, &result);
  }

  return result;
}

void op_verify_output_result_tty(const VerifyResult& result) {
  auto summary = fmt::format(
    "files={} size={} diff={}",
    result.total_file_count,
    tty_print_value_bytes(result.total_file_size),
    result.diff.size()
  );

  switch (op_verify_result_status(result)) {
    case VerifyResultStatus::PASS:
      std::cout << fmt::format("{} {}", tty_print_success("PASS"), summary) << std::endl;
      break;
    case VerifyResultStatus::WARN:
      std::cout << fmt::format("{} {}", tty_print_warning("WARN"), summary) << std::endl;
      break;
    case VerifyResultStatus::FAIL:
      std::cout << fmt::format("{} {}", tty_print_error("FAIL"), summary) << std::endl;
      break;
  }

  if (result.diff.size() > 0) {
    std::cout << std::endl;
  }

  for (const auto& msg : result.diff) {
    switch (msg.type) {
      case VerifyDiffType::EXTRA:
        std::cout << fmt::format("- {} (extraneous file)", msg.path) << std::endl;
        break;
      case VerifyDiffType::MISSING:
        std::cout << fmt::format("- {} (missing file)", msg.path) << std::endl;
        break;
      case VerifyDiffType::CONFLICT_SIZE:
        std::cout << fmt::format("- {} (size mismatch)", msg.path) << std::endl;
        break;
      case VerifyDiffType::CONFLICT_DATA:
        std::cout << fmt::format("- {} (data mismatch)", msg.path) << std::endl;
        break;
    }
  }
}

void op_verify_output_result_text(const VerifyResult& result) {
  if (result.diff.size() == 0) {
    std::cout << "nodiff" << std::endl;
    return;
  }

  for (const auto& msg : result.diff) {
    switch (msg.type) {
      case VerifyDiffType::EXTRA:
        std::cout << fmt::format("diff extra {}", msg.path) << std::endl;
        break;
      case VerifyDiffType::MISSING:
        std::cout << fmt::format("diff missing {}", msg.path) << std::endl;
        break;
      case VerifyDiffType::CONFLICT_SIZE:
        std::cout << fmt::format("diff size {}", msg.path) << std::endl;
        break;
      case VerifyDiffType::CONFLICT_DATA:
        std::cout << fmt::format("diff data {}", msg.path) << std::endl;
        break;
    }
  }
}

VerifyOutputType op_verify_output_type_read(const std::string& x) {
  if (x == "tty") {
    return VerifyOutputType::TTY;
  }

  if (x == "text") {
    return VerifyOutputType::TEXT;
  }

  throw std::runtime_error("invalid output type");
}

int op_verify(char** args, size_t arg_count) {
  VerifyOp op;
  VerifyOutputType output_type;

  auto opts_short = std::string("i:");
  auto opts_long = std::array<struct option, 3>{{
    {"index", required_argument, 0, 'i'},
    {"output", required_argument, 0, 'o'},
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
      case 'o':
        output_type = op_verify_output_type_read(optarg);
        break;
    }
  }

  if (op.index_path.empty()) {
    throw std::runtime_error("need an index (--index)");
  }

  if (op.root_path.empty()) {
    op.root_path = std::filesystem::current_path();
  }

  auto result = op_verify(op);

  switch (output_type) {
    case VerifyOutputType::TTY:
      op_verify_output_result_tty(result);
      break;
    case VerifyOutputType::TEXT:
      op_verify_output_result_text(result);
      break;
  }

  if (op_verify_result_status(result) == VerifyResultStatus::PASS) {
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}

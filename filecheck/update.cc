#include "update.h"

#include "index.h"

#include <fmt/core.h>

void update_index(const UpdateOp& op) {
  Index index;

  Progress progress;
  progress.files = 0;
  progress.files_total = 0;
  progress.bytes = 0;
  progress.bytes_total = 0;

  auto file_iter = std::filesystem::recursive_directory_iterator(op.root_path);
  for (const auto& file_info : file_iter) {
    auto file_path = file_info.path();
    if (std::filesystem::is_directory(file_path)) {
      continue;
    }

    IndexRecord record;
    record.path = std::filesystem::relative(file_path, op.root_path);
    record.size = file_info.file_size();
    index_add(record, &index);

    if (op.progress) {
      progress.files_total += 1;
      progress.bytes_total += record.size;
      op.progress(progress, false);
    }
  }

  if (op.progress) {
    op.progress(progress, true);
  }

  for (auto& record : index.entries) {
    record.checksums.push_back(
      checksum_compute(op.root_path / record.path, op.checksum_type)
    );

    if (op.progress) {
      progress.files += 1;
      progress.bytes += record.size;
      op.progress(progress, true);
    }
  }

  if (op.progress) {
    op.progress(progress, true);
  }

  index_sort(&index);
  index_write(op.index_path, index);
}

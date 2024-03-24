#include "index.h"

void index_add(IndexRecord entry, Index* index) {
  index->entries.emplace_back(entry);
}

IndexPathSet index_build_path_set(const Index& index) {
  IndexPathSet path_set;

  for (const auto& record : index.entries) {
    path_set.paths.insert(record.path);
  }

  return path_set;
}

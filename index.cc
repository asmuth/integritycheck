#include "index.h"
#include <numeric>

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

uint64_t index_total_file_count(const Index& index) {
  return index.entries.size();
}

uint64_t index_total_file_size(const Index& index) {
  return std::accumulate(
    index.entries.begin(),
    index.entries.end(),
    uint64_t(0),
    [] (const auto& sum, const auto& entry) {
      return sum + entry.size;
    }
  );
}


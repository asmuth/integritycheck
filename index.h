#pragma once
#include "checksum.h"
#include <string>
#include <vector>
#include <unordered_set>

struct IndexRecord {

  /**
   * The relative path of the file
   */
  std::string path;

  /**
   * The size of the file in bytes
   */
  size_t size;

  /**
   * A list of checksums of file's content
   */
  std::vector<Checksum> checksums;

};

struct Index {
  std::vector<IndexRecord> entries;
};

struct IndexPathSet {
  std::unordered_set<std::string> paths;
};

void index_add(IndexRecord entry, Index* index);

IndexPathSet index_build_path_set(const Index& index);

void index_read(
  const std::string& file_path,
  Index* index
);

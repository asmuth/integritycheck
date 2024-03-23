#pragma once
#include "checksum.h"
#include <string>
#include <vector>

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

void index_add(IndexRecord entry, Index* index);

#include "index.h"

void index_add(IndexRecord entry, Index* index) {
  index->entries.emplace_back(entry);
}


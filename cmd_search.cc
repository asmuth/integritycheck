#include "checksum.h"

#include <cstring>
#include <fstream>
#include <string>
#include <stdexcept>
#include <iostream>
#include <unordered_set>
#include <string_view>
#include <getopt.h>

struct ChecksumHash {
  std::hash<std::string_view> hash_fn;

  std::size_t operator()(const Checksum& checksum) const {
    return hash_fn(
      std::string_view(
        reinterpret_cast<const char*>(checksum.value.data),
        checksum_size(checksum.type)
      )
    );
  }
};

struct ChecksumCompare {
  bool operator()(const Checksum& a, const Checksum& b) const {
    return checksum_compare(a, b);
  }
};

void index_load(
  const std::string& file_path,
  std::unordered_set<Checksum, ChecksumHash, ChecksumCompare>* index
) {
  std::ifstream file_reader(file_path);
  if (!file_reader) {
    throw std::runtime_error("unable to open file: " + file_path);
  }

  std::string line;
  for (;;) {
    if (!std::getline(file_reader, line)) {
      if (file_reader.bad()) {
        throw std::runtime_error(
          "error while reading from file: " +
          file_path + 
          ": " +
          strerror(errno)
        );
      } else {
        break;
      }
    }

    if (line.size() < 40) {
      throw std::runtime_error("invalid index file: " + file_path);
    }

    Checksum checksum;
    checksum.type = ChecksumType::SHA1;
    checksum.value = checksum_read_value(line.substr(0, 40), checksum.type);

    index->insert(checksum);
  }
}

void cmd_search(char** args, size_t arg_count) {
  std::string index_path;

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
        index_path = optarg;
        break;
    }
  }

  if (index_path.empty()) {
    throw std::runtime_error("need an index (--index)");
  }

  std::unordered_set<Checksum, ChecksumHash, ChecksumCompare> index;
  index_load(index_path, &index);

  std::string file_path;
  for (;;) {
    if (!std::getline(std::cin, file_path)) {
      if (std::cin.bad()) {
        throw std::runtime_error(
          std::string("error while reading from stdin: ") +
          strerror(errno)
        );
      } else {
        break;
      }
    }

    auto file_checksum = checksum_compute_sha1(file_path);

    if (index.count(file_checksum) > 0) {
      std::cout << "hit " << file_path << std::endl;
    } else {
      std::cout << "miss " << file_path << std::endl;
    }
  }
}

// test: non existing index file
// test: invalid index file

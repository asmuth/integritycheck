#include "checksum.h"

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <stdexcept>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <iomanip>
#include <getopt.h>

uint8_t hex_decode(char c) {
  switch (c) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'A': return 10;
    case 'a': return 10;
    case 'B': return 11;
    case 'b': return 11;
    case 'C': return 12;
    case 'c': return 12;
    case 'D': return 13;
    case 'd': return 13;
    case 'E': return 14;
    case 'e': return 14;
    case 'F': return 15;
    case 'f': return 15;
    default:
      throw std::runtime_error("invalid hex string");
  }
}

std::string hex_decode(const char* data, size_t data_len) {
  if (data_len % 2 != 0) {
    throw std::runtime_error("invalid hex string");
  }

  std::string result(data_len / 2, 0);
  for (size_t i = 0; i < data_len / 2; i++) {
    result[i] =
      (hex_decode(data[i * 2 + 0]) << 4) |
      (hex_decode(data[i * 2 + 1]));
  }

  return result;
}

std::string hex_decode(const std::string& data) {
  return hex_decode(data.data(), data.size());
}

char hex_encode(uint8_t x) {
  switch (x) {
    case 0: return '0';
    case 1: return '1';
    case 2: return '2';
    case 3: return '3';
    case 4: return '4';
    case 5: return '5';
    case 6: return '6';
    case 7: return '7';
    case 8: return '8';
    case 9: return '9';
    case 10: return 'a';
    case 11: return 'b';
    case 12: return 'c';
    case 13: return 'd';
    case 14: return 'e';
    case 15: return 'f';
    default:
      throw std::runtime_error("invalid input");
  }
}

std::string hex_encode(const char* data, size_t data_len) {
  std::string result(data_len * 2, 0);

  for (size_t i = 0; i < data_len; ++i) {
    result[i * 2 + 0] = hex_encode(uint8_t(data[i]) >> 4);
    result[i * 2 + 1] = hex_encode(uint8_t(data[i]) & 0xf);
  }

  return result;
}

std::string hex_encode(const std::string& data) {
  return hex_encode(data.data(), data.size());
}

void index_load(
  const std::string& file_path,
  std::unordered_set<std::string>* index
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

    std::string checksum;
    try {
      checksum = hex_decode(line.substr(0, 40));
    } catch (...) {
      throw std::runtime_error("invalid index line: " + line);
    }

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

  std::unordered_set<std::string> index;
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

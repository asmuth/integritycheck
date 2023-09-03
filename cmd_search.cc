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
#include <openssl/sha.h>

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

std::string checksum_compute(const std::string& file_path) {
  SHA_CTX sha1;
  if (!SHA1_Init(&sha1)) {
    throw std::runtime_error("SHA1 error");
  }

  std::ifstream file_reader(file_path);
  if (!file_reader) {
    throw std::runtime_error("unable to open file: " + file_path);
  }

  std::vector<char> file_buffer(1024);
  while (!file_reader.eof()) {
    file_reader.read(file_buffer.data(), file_buffer.size());
    if (file_reader.bad()) {
      throw std::runtime_error(
        "error while reading from file: " +
        file_path + 
        ": " +
        strerror(errno)
      );
    }

    if (file_reader.gcount() > file_buffer.size()) {
      throw std::runtime_error("invalid buffer");
    }

    if (
      !SHA1_Update(
        &sha1,
        file_buffer.data(),
        file_reader.gcount()
      )
    ) {
      throw std::runtime_error("SHA1 error");
    }
  }

  std::string sha1_digest(SHA_DIGEST_LENGTH, 0);
  if (!SHA1_Final((unsigned char*) sha1_digest.data(), &sha1)) {
    throw std::runtime_error("SHA1 error");
  }

  return sha1_digest;
}

int main(int argc, char** argv) {
  std::unordered_set<std::string> index;

  for (int i = 1; i < argc; ++i) {
    index_load(argv[i], &index);
  }

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

    auto file_checksum = checksum_compute(file_path);

    if (index.count(file_checksum) > 0) {
      std::cout << "hit " << file_path << std::endl;
    } else {
      std::cout << "miss " << file_path << std::endl;
    }
  }

  return EXIT_SUCCESS;
}

// test: non existing index file
// test: invalid index file

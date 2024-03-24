#include "checksum.h"

#include <stdexcept>
#include <vector>
#include <fstream>
#include <cstring>
#include <openssl/sha.h>

size_t checksum_size(ChecksumType type) {
  switch (type) {
    case ChecksumType::MD5:
      return 16;
    case ChecksumType::SHA1:
      return 20;
  }

  throw std::runtime_error("invalid checksum type");
}

ChecksumType checksum_read_type(const std::string& input) {
  if (input == "md5") {
    return ChecksumType::MD5;
  }

  if (input == "sha1") {
    return ChecksumType::SHA1;
  }

  throw std::runtime_error("invalid checksum type: " + input);
}

uint8_t checksum_read_value_byte(char c) {
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
      throw std::runtime_error("invalid checksum value");
  }
}

ChecksumValue checksum_read_value(const std::string& input, ChecksumType type) {
  if (input.size() != checksum_size(type) * 2) {
    throw std::runtime_error("invalid checksum value");
  }

  ChecksumValue value;
  for (size_t i = 0; i < input.size() / 2; i++) {
    value.data[i] =
      (checksum_read_value_byte(input[i * 2 + 0]) << 4) |
      (checksum_read_value_byte(input[i * 2 + 1]));
  }

  return value;
}

char checksum_write_value_byte(uint8_t x) {
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

std::string checksum_write_value(const ChecksumValue& value, ChecksumType type) {
  auto value_size = checksum_size(type);

  std::string output(value_size * 2, 0);
  for (size_t i = 0; i < value_size; ++i) {
    output[i * 2 + 0] = checksum_write_value_byte(uint8_t(value.data[i]) >> 4);
    output[i * 2 + 1] = checksum_write_value_byte(uint8_t(value.data[i]) & 0xf);
  }

  return output;
}

std::string checksum_write_value(const Checksum& checksum) {
  return checksum_write_value(checksum.value, checksum.type);
}

bool checksum_compare(const Checksum& a, const Checksum& b) {
  if (a.type != b.type) {
    return false;
  }

  auto result = std::memcmp(
    a.value.data,
    b.value.data,
    checksum_size(a.type)
  );

  return result == 0;
}

Checksum checksum_compute_sha1(const std::string& file_path) {
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
        std::strerror(errno)
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

  Checksum checksum;
  checksum.type = ChecksumType::SHA1;

  if (!SHA1_Final((unsigned char*) checksum.value.data, &sha1)) {
    throw std::runtime_error("SHA1 error");
  }

  return checksum;
}

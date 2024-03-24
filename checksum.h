#pragma once
#include <string>

enum class ChecksumType {
  MD5,
  SHA1,
};

struct ChecksumValue {
  uint8_t data[20];
};

struct Checksum {
  ChecksumType type;
  ChecksumValue value;
};

size_t checksum_get_value_size(ChecksumType type);

ChecksumType checksum_read_type(const std::string& input);

ChecksumValue checksum_read_value(const std::string& input, ChecksumType type);

std::string checksum_write_value(const Checksum& checksum);

bool checksum_compare(const Checksum& a, const Checksum& b);

Checksum checksum_compute_sha1(const std::string& file_path);


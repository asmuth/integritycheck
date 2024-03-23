#pragma once
#include <string>

enum class ChecksumType {
  MD5,
  SHA1,
};

struct ChecksumValue {
  std::string data;
};

struct Checksum {
  ChecksumType type;
  ChecksumValue value;
};

ChecksumType checksum_read_type(const std::string& input);

ChecksumValue checksum_read_value(const std::string& input);

std::string checksum_compute_sha1(const std::string& file_path);

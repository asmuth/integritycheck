#pragma once
#include <filesystem>
#include <functional>

using TestDataGenerator = std::function<uint8_t (size_t)>;

void test_create_file(
  const std::filesystem::path& file_path,
  const std::string& file_data
);

void test_create_file(
  const std::filesystem::path& file_path,
  size_t file_size,
  TestDataGenerator file_data
);

void test_create_directory(
  const std::filesystem::path& file_path
);

TestDataGenerator test_generate_zeroes();


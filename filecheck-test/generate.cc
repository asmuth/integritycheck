#include "filecheck-test/generate.h"
#include <fstream>

void test_create_file(
  const std::filesystem::path& file_path,
  const std::string& file_data
) {
  std::ofstream file_writer(file_path);
  file_writer << file_data;

  if (file_writer.bad()) {
    throw std::runtime_error("error while writing test file: " + file_path.string());
  }
}

void test_create_file(
  const std::filesystem::path& file_path,
  size_t file_size,
  TestDataGenerator file_data
) {
  std::ofstream file_writer(file_path);
  for (size_t i = 0; i < file_size; ++i) {
    file_writer << file_data(i);
  }

  if (file_writer.bad()) {
    throw std::runtime_error("error while writing test file: " + file_path.string());
  }
}

void test_create_directory(
  const std::filesystem::path& file_path
) {
  if (!std::filesystem::create_directory(file_path)) {
    throw std::runtime_error("error while creating test directory: " + file_path.string());
  }
}

TestDataGenerator test_generate_zeroes() {
  return [] (auto i) { return 0; };
}

std::string test_read_file(
  const std::filesystem::path& file_path
) {
  auto data_len = std::filesystem::file_size(file_path);
  auto data = std::string(data_len, '\0');
  auto data_reader = std::ifstream(file_path);
  data_reader.read(data.data(), data_len);
  return data;
}

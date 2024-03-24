#include "generate.h"
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

void test_create_directory(
  const std::filesystem::path& file_path
) {
  if (!std::filesystem::create_directory(file_path)) {
    throw std::runtime_error("error while creating test directory: " + file_path.string());
  }
}

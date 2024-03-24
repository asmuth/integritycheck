#pragma once
#include <filesystem>

void test_create_file(
  const std::filesystem::path& file_path,
  const std::string& file_data
);

void test_create_directory(
  const std::filesystem::path& file_path
);

#include "filecheck/path.h"

#include <filesystem>

std::string path_normalize(const std::string& path) {
  auto path_normal = std::filesystem::path(path).lexically_normal().string();
  if (!path_normal.empty() && path_normal.back() == '/') {
    path_normal.pop_back();
  }

  return path_normal;
}

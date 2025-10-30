#include "filecheck/path_filter.h"
#include "filecheck/path.h"

#include <filesystem>
#include <fmt/core.h>

bool path_filter_match_path(const std::string& prefix_path, const std::string& path) {
  if (!path.starts_with(prefix_path)) {
    return false;
  }

  // A forward slash always implies the end of a path component, regardless of
  // where it appears in the string, since path components themselves can not
  // contain forward slashes. This is guaranteed by POSIX: "The bytes composing
  // the [file]name shall not contain the <NUL> or <slash> characters."
  if (path.size() > prefix_path.size() && path[prefix_path.size()] != '/') {
    return false;
  }

  return true;
}

bool path_filter_match(const PathFilter& filter, const std::string& path) {
  if (filter.select.empty()) {
    return true;
  }

  for (const auto& select_path : filter.select) {
    if (path_filter_match_path(select_path, path)) {
      return true;
    }
  }

  return false;
}

void path_filter_add_select(PathFilter* filter, const std::string& path) {
  auto path_normal = path_normalize(path);
  if (path_normal.empty()) {
    return;
  }

  if (path_normal.starts_with("/")) {
    throw std::runtime_error(
      fmt::format("filter path '{}' must be a relative path", path)
    );
  }

  if (path_normal.starts_with("../")) {
    throw std::runtime_error(
      fmt::format("filter path '{}' is outside of the data directory", path)
    );
  }

  filter->select.push_back(path_normal);
}

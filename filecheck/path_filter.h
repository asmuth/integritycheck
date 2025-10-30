#pragma once
#include <string>
#include <vector>

struct PathFilter {
  std::vector<std::string> select;
};

bool path_filter_match(const PathFilter& filter, const std::string& path);

void path_filter_add_select(PathFilter* filter, const std::string& path);

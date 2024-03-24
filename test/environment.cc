#include "environment.h"
#include <stdexcept>

std::filesystem::path test_get_tmpdir() {
  auto tmpdir = getenv("TEST_TMPDIR");
  if (!tmpdir) {
    throw std::runtime_error("missing TEST_TMPDIR");
  }

  return std::filesystem::path(tmpdir);
}

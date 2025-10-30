#include "verify.h"
#include "filecheck-test/assert.h"
#include "filecheck-test/environment.h"
#include "filecheck-test/generate.h"
#include "filecheck-test/shell.h"

#include <filesystem>
#include <fmt/core.h>

void test_absolute() {
  auto root_path = test_get_tmpdir() / "root";

  auto command = fmt::format(
    "filecheck --index index.lst --directory {} /tmp/c 2>&1",
    root_path.string(),
    root_path.string()
  );

  auto result = shell_exec(command);

  auto output_expected = fmt::format(
    "ERROR: filter path '/tmp/c' must be a relative path\n",
    root_path.string()
  );

  EXPECT(result.exitcode == 1)
  EXPECT(result.output == output_expected)
}

void test_outside() {
  auto root_path = test_get_tmpdir() / "root";

  auto command = fmt::format(
    "filecheck --index index.lst --directory {} ../c 2>&1",
    root_path.string(),
    root_path.string()
  );

  auto result = shell_exec(command);

  auto output_expected = fmt::format(
    "ERROR: filter path '../c' is outside of the data directory\n",
    root_path.string()
  );

  EXPECT(result.exitcode == 1)
  EXPECT(result.output == output_expected)
}

int main(int argc, char** argv) {
  auto root_path = test_get_tmpdir() / "root";
  test_create_directory(root_path);
  test_create_directory(root_path / "a");
  test_create_file(root_path / "a" / "1.txt", "fnord");
  test_create_directory(root_path / "b");
  test_create_file(root_path / "b" / "2.txt", "blah");

  test_absolute();
  test_outside();

  return EXIT_SUCCESS;
}

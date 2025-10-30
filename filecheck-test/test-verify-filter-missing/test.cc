#include "verify.h"
#include "filecheck-test/assert.h"
#include "filecheck-test/environment.h"
#include "filecheck-test/generate.h"
#include "filecheck-test/shell.h"

#include <filesystem>
#include <fmt/core.h>

void test_internal() {
  VerifyOp op;
  op.index_path = "index.lst";
  op.root_path = test_get_tmpdir() / "root";
  op.filter.select.push_back("b/2.txt");

  auto op_result = op_verify(op);

  EXPECT(op_result.status == VerifyResultStatus::PASS);
  EXPECT(op_result.diff.size() == 0);
  EXPECT(op_result.verified_file_count == 1);
  EXPECT(op_result.verified_file_size == 4);
}

void test_shell() {
  auto root_path = test_get_tmpdir() / "root";
  auto command = fmt::format(
    "filecheck --index index.lst --directory {} c",
    root_path.string()
  );

  auto result = shell_exec(command);
  auto output_expected = test_read_file("output_tty.txt");

  fmt::println("out: {}", result.output);
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

  test_internal();
  test_shell();

  return EXIT_SUCCESS;
}

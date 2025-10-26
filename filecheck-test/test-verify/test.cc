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

  auto op_result = op_verify(op);

  EXPECT(op_result.status == VerifyResultStatus::PASS);
  EXPECT(op_result.diff.size() == 0);
}

void test_shell() {
  auto root_path = test_get_tmpdir() / "root";
  auto command = fmt::format("filecheck --index index.lst --directory {}", root_path.string());
  auto result = shell_exec(command);
  auto output_expected = test_read_file("output_tty.txt");

  EXPECT(result.exitcode == 0)
  EXPECT(result.output == output_expected)
}

void test_shell_short() {
  auto root_path = test_get_tmpdir() / "root";
  auto command = fmt::format("filecheck -i index.lst -d {}", root_path.string());
  auto result = shell_exec(command);
  auto output_expected = test_read_file("output_tty.txt");

  EXPECT(result.exitcode == 0)
  EXPECT(result.output == output_expected)
}

int main(int argc, char** argv) {
  auto root_path = test_get_tmpdir() / "root";
  test_create_directory(root_path);
  test_create_file(root_path / "md5.txt", "fnord");
  test_create_file(root_path / "sha1.txt", "fnord");
  test_create_file(root_path / "all.txt", "blah");
  test_create_file(root_path / "zero", 100000, test_generate_zeroes());

  test_internal();
  test_shell();
  test_shell_short();

  return EXIT_SUCCESS;
}

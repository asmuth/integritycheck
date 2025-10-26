#include "verify.h"
#include "filecheck-test/assert.h"
#include "filecheck-test/environment.h"
#include "filecheck-test/generate.h"
#include "filecheck-test/shell.h"

#include <filesystem>
#include <fmt/format.h>

void test_internal() {
  VerifyOp op;
  op.index_path = "index.lst";
  op.root_path = test_get_tmpdir() / "root";

  auto op_result = op_verify(op);

  EXPECT(op_result.status == VerifyResultStatus::FAIL);
  EXPECT(op_result.diff.size() == 1);
  EXPECT(op_result.diff[0].type == VerifyDiffType::MISSING);
  EXPECT(op_result.diff[0].path == "two.txt");
}

void test_shell() {
  auto root_path = test_get_tmpdir() / "root";
  auto command = fmt::format("filecheck --index index.lst --directory {}", root_path.string());
  auto result = shell_exec(command);
  auto output_expected = test_read_file("output_tty.txt");

  EXPECT(result.exitcode == 1)
  EXPECT(result.output == output_expected)
}

int main(int argc, char** argv) {
  auto root_path = test_get_tmpdir() / "root";
  test_create_directory(root_path);
  test_create_file(root_path / "one.txt", "fnord");
  test_create_file(root_path / "three.txt", "blah");

  test_internal();
  test_shell();

  return EXIT_SUCCESS;
}

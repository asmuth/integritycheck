#include "update.h"
#include "filecheck-test/assert.h"
#include "filecheck-test/environment.h"
#include "filecheck-test/generate.h"
#include "filecheck-test/shell.h"

#include <filesystem>
#include <fmt/core.h>

void test_internal() {
  UpdateOp op;
  op.index_path = test_get_tmpdir() / "index_internal.lst";
  op.root_path = test_get_tmpdir() / "root";
  op.checksum_type = ChecksumType::SHA1;

  update_index(op);

  EXPECT(test_read_file(op.index_path) == test_read_file("index.lst"));
}

void test_shell() {
  auto index_path = test_get_tmpdir() / "index_shell_longopts.lst";
  auto root_path = test_get_tmpdir() / "root";
  auto command = fmt::format("filecheck --update --index {} --directory {}", index_path.string(), root_path.string());
  auto result = shell_exec(command);

  EXPECT(result.exitcode == 0)
  EXPECT(result.output.size() == 0)
  EXPECT(test_read_file(index_path) == test_read_file("index.lst"));
}

void test_shell_short() {
  auto index_path = test_get_tmpdir() / "index_shell.lst";
  auto root_path = test_get_tmpdir() / "root";
  auto command = fmt::format("filecheck -u -i {} -d {}", index_path.string(), root_path.string());
  auto result = shell_exec(command);

  EXPECT(result.exitcode == 0)
  EXPECT(result.output.size() == 0)
  EXPECT(test_read_file(index_path) == test_read_file("index.lst"));
}

int main(int argc, char** argv) {
  auto root_path = test_get_tmpdir() / "root";
  test_create_directory(root_path);
  test_create_file(root_path / "file1.bin", "fnord");
  test_create_directory(root_path / "a");
  test_create_file(root_path / "a" / "file with whitespace.txt", "blah");
  test_create_directory(root_path  / "exce\\\\ent\n🔥file\\\n\\ name");
  test_create_file(root_path / "exce\\\\ent\n🔥file\\\n\\ name" / "zero", 100000, test_generate_zeroes());

  test_internal();
  test_shell();
  test_shell_short();

  return EXIT_SUCCESS;
}

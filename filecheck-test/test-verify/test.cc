#include "verify.h"
#include "filecheck-test/assert.h"
#include "filecheck-test/environment.h"
#include "filecheck-test/generate.h"

#include <filesystem>

int main(int argc, char** argv) {
  auto root_path = test_get_tmpdir() / "root";
  test_create_directory(root_path);
  test_create_file(root_path / "md5.txt", "fnord");
  test_create_file(root_path / "sha1.txt", "fnord");
  test_create_file(root_path / "all.txt", "blah");
  test_create_file(root_path / "zero", 100000, test_generate_zeroes());

  VerifyOp op;
  op.index_path = "index.lst";
  op.root_path = root_path;

  auto op_result = op_verify(op);

  EXPECT(op_result.status == VerifyResultStatus::PASS);
  EXPECT(op_result.diff.size() == 0);

  return EXIT_SUCCESS;
}

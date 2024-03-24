#include "op_verify.h"
#include "test/assert.h"
#include "test/environment.h"
#include "test/generate.h"

#include <filesystem>

int main(int argc, char** argv) {
  auto root_path = test_get_tmpdir() / "root";
  test_create_directory(root_path);
  test_create_file(root_path / "one.txt", "fnord");
  test_create_file(root_path / "three.txt", "blah");

  VerifyOp op;
  op.index_path = "index.lst";
  op.root_path = root_path;

  auto op_result = op_verify(op);

  EXPECT(op_result.count_ok == 2);
  EXPECT(op_result.count_missing == 1);
  EXPECT(op_result.count_corrupt == 0);
  EXPECT(op_result.messages.size() == 1);
  EXPECT(op_result.messages[0].type == VerifyMessageType::MISSING);
  EXPECT(op_result.messages[0].path == "two.txt");

  return EXIT_SUCCESS;
}

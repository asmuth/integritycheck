#include "op_verify.h"
#include "test/assert.h"
#include "test/environment.h"
#include "test/generate.h"

#include <filesystem>

int main(int argc, char** argv) {
  auto root_path = test_get_tmpdir() / "root";
  test_create_directory(root_path);
  test_create_file(root_path / "sha1_ok.txt", "fnord1");
  test_create_file(root_path / "sha1_size.txt", "fnord");
  test_create_file(root_path / "sha1_data.txt", "fnordX");

  VerifyOp op;
  op.index_path = "index.lst";
  op.root_path = root_path;

  auto op_result = op_verify(op);

  EXPECT(op_result.count_ok == 1);
  EXPECT(op_result.count_missing == 0);
  EXPECT(op_result.count_corrupt == 2);
  EXPECT(op_result.messages.size() == 2);
  EXPECT(op_result.messages[0].type == VerifyMessageType::CORRUPT_SIZE);
  EXPECT(op_result.messages[0].path == "sha1_size.txt");
  EXPECT(op_result.messages[1].type == VerifyMessageType::CORRUPT_DATA);
  EXPECT(op_result.messages[1].path == "sha1_data.txt");

  return EXIT_SUCCESS;
}

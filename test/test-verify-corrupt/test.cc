#include "op_verify.h"
#include "test/assert.h"
#include "test/environment.h"
#include "test/generate.h"

#include <filesystem>

int main(int argc, char** argv) {
  auto root_path = test_get_tmpdir() / "root";
  test_create_directory(root_path);
  test_create_file(root_path / "md5_ok.txt", "fnord1");
  test_create_file(root_path / "md5_size.txt", "fnord");
  test_create_file(root_path / "md5_data.txt", "fnordX");
  test_create_file(root_path / "sha1_ok.txt", "fnord1");
  test_create_file(root_path / "sha1_size.txt", "fnord");
  test_create_file(root_path / "sha1_data.txt", "fnordX");

  VerifyOp op;
  op.index_path = "index.lst";
  op.root_path = root_path;

  auto op_result = op_verify(op);

  EXPECT(op_result.count_ok == 2);
  EXPECT(op_result.count_missing == 0);
  EXPECT(op_result.count_corrupt == 4);
  EXPECT(op_result.count_omit == 0);
  EXPECT(op_result.messages.size() == 4);
  EXPECT(op_result.messages[0].type == VerifyMessageType::CORRUPT_SIZE);
  EXPECT(op_result.messages[0].path == "md5_size.txt");
  EXPECT(op_result.messages[1].type == VerifyMessageType::CORRUPT_DATA);
  EXPECT(op_result.messages[1].path == "md5_data.txt");
  EXPECT(op_result.messages[2].type == VerifyMessageType::CORRUPT_SIZE);
  EXPECT(op_result.messages[2].path == "sha1_size.txt");
  EXPECT(op_result.messages[3].type == VerifyMessageType::CORRUPT_DATA);
  EXPECT(op_result.messages[3].path == "sha1_data.txt");

  return EXIT_SUCCESS;
}

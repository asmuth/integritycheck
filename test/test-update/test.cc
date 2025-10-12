#include "update.h"
#include "test/assert.h"
#include "test/environment.h"
#include "test/generate.h"

#include <filesystem>

int main(int argc, char** argv) {
  auto root_path = test_get_tmpdir() / "root";
  test_create_directory(root_path);
  test_create_file(root_path / "file1.bin", "fnord");
  test_create_directory(root_path / "a");
  test_create_file(root_path / "a" / "file with whitespace.txt", "blah");
  test_create_directory(root_path  / "exce\\\\ent\n🔥file\\\n\\ name");
  test_create_file(root_path / "exce\\\\ent\n🔥file\\\n\\ name" / "zero", 100000, test_generate_zeroes());

  UpdateOp op;
  op.index_path = test_get_tmpdir() / "index.lst";
  op.root_path = root_path;
  op.checksum_type = ChecksumType::SHA1;

  update_index(op);

  EXPECT(test_read_file(op.index_path) == test_read_file("index.lst"));

  return EXIT_SUCCESS;
}

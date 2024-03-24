#include "op_verify.h"
#include "test/assert.h"
#include "test/environment.h"
#include "test/generate.h"

#include <filesystem>
#include <unordered_set>

int main(int argc, char** argv) {
  auto root_path = test_get_tmpdir() / "root";
  test_create_directory(root_path);
  test_create_directory(root_path / "A");
  test_create_directory(root_path / "A" / "X");
  test_create_directory(root_path / "A" / "Y");
  test_create_directory(root_path / "B");
  test_create_directory(root_path / "B" / "X");
  test_create_file(root_path / "1.txt", "1");
  test_create_file(root_path / "2.txt", "2");
  test_create_file(root_path / "3.txt", "3");
  test_create_file(root_path / "A/4.txt", "4");
  test_create_file(root_path / "A/5.txt", "5");
  test_create_file(root_path / "A/X/6.txt", "6");
  test_create_file(root_path / "A/X/7.txt", "7");
  test_create_file(root_path / "A/Y/8.txt", "8");
  test_create_file(root_path / "A/Y/9.txt", "9");
  test_create_file(root_path / "B/X/10.txt", "10");
  test_create_file(root_path / "B/X/11.txt", "11");

  VerifyOp op;
  op.index_path = "index.lst";
  op.root_path = root_path;

  auto op_result = op_verify(op);

  EXPECT(op_result.count_ok == 6);
  EXPECT(op_result.count_missing == 0);
  EXPECT(op_result.count_corrupt == 0);
  EXPECT(op_result.count_omit == 5);
  EXPECT(op_result.messages.size() == 5);

  std::unordered_set<std::string> omitted;
  for (const auto& msg : op_result.messages) {
    EXPECT(msg.type == VerifyMessageType::OMITTED);
    omitted.insert(msg.path);
  }

  EXPECT(omitted.contains("2.txt"));
  EXPECT(omitted.contains("A/4.txt"));
  EXPECT(omitted.contains("A/X/7.txt"));
  EXPECT(omitted.contains("A/Y/9.txt"));
  EXPECT(omitted.contains("B/X/11.txt"));

  return EXIT_SUCCESS;
}

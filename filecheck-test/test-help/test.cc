#include "filecheck-test/assert.h"
#include "filecheck-test/generate.h"
#include "filecheck-test/shell.h"

int main(int argc, char** argv) {
  auto command = "filecheck --help 2>&1";
  auto result = shell_exec(command);
  auto output_expected = test_read_file("output.txt");

  EXPECT(result.exitcode == 0)
  EXPECT(result.output == output_expected)

  return EXIT_SUCCESS;
}

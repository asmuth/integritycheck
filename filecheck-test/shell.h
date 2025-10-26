#include <string>

struct ShellResult {
  int exitcode;
  std::string output;
};

ShellResult shell_exec(const std::string& command);

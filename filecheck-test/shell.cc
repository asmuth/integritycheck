#include "filecheck-test/shell.h"

#include <array>
#include <cstdio>
#include <fmt/core.h>
#include <string>
#include <stdexcept>
#include <sys/wait.h>

std::string shell_expand_command(const std::string& command) {
  auto test_rundir = getenv("TEST_RUNDIR");
  if (!test_rundir) {
    throw std::runtime_error("TEST_RUNDIR is not set");
  }

  return fmt::format("PATH={} {}; exit $?", test_rundir, command);
}

ShellResult shell_exec(const std::string& command) {
  ShellResult result;
  result.exitcode = -1;

  auto procfd = popen(shell_expand_command(command).c_str(), "r");
  if (!procfd) {
    throw std::runtime_error("unable to run command");
  }

  std::array<char, 512> buffer;
  while (fgets(buffer.data(), buffer.size(), procfd) != nullptr) {
    result.output += buffer.data();
  }

  if (ferror(procfd) != 0) {
    pclose(procfd);
    throw std::runtime_error("error while reading command output");
  }

  int status = pclose(procfd);
  if (WIFEXITED(status)) {
    result.exitcode = WEXITSTATUS(status);
  } else {
    throw std::runtime_error("error while reading command exit code");
  }

  return result;
}

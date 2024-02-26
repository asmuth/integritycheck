#include <iostream>
#include <string>

#include "cmd_search.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "ERROR: need a command" << std::endl;
    return EXIT_FAILURE;
  }

  auto cmd = std::string(argv[1]);

  if (cmd == "search") {
    cmd_search(argv + 1, argc - 1);
    return EXIT_SUCCESS;
  }

  std::cerr << "ERROR: invalid command" << std::endl;
  return EXIT_FAILURE;
}


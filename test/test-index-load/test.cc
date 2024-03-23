#include <stdexcept>

int main(int argc, char** argv) {
  throw std::runtime_error("fnord");
  return EXIT_SUCCESS;
}

#include "checksum.h"

#include <stdexcept>
#include <vector>
#include <fstream>
#include <cstring>
#include <openssl/sha.h>

std::string checksum_compute_sha1(const std::string& file_path) {
  SHA_CTX sha1;
  if (!SHA1_Init(&sha1)) {
    throw std::runtime_error("SHA1 error");
  }

  std::ifstream file_reader(file_path);
  if (!file_reader) {
    throw std::runtime_error("unable to open file: " + file_path);
  }

  std::vector<char> file_buffer(1024);
  while (!file_reader.eof()) {
    file_reader.read(file_buffer.data(), file_buffer.size());
    if (file_reader.bad()) {
      throw std::runtime_error(
        "error while reading from file: " +
        file_path +
        ": " +
        std::strerror(errno)
      );
    }

    if (file_reader.gcount() > file_buffer.size()) {
      throw std::runtime_error("invalid buffer");
    }

    if (
      !SHA1_Update(
        &sha1,
        file_buffer.data(),
        file_reader.gcount()
      )
    ) {
      throw std::runtime_error("SHA1 error");
    }
  }

  std::string sha1_digest(SHA_DIGEST_LENGTH, 0);
  if (!SHA1_Final((unsigned char*) sha1_digest.data(), &sha1)) {
    throw std::runtime_error("SHA1 error");
  }

  return sha1_digest;
}

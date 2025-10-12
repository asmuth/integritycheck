#include "index.h"
#include "test/assert.h"
#include "test/environment.h"
#include "test/generate.h"

#include <fstream>

int main(int argc, char** argv) {
  Index index;

  index.entries.push_back(IndexRecord {
    .path = "my/test/file.bin",
    .size = 555,
    .checksums = {
      {
        .type = ChecksumType::SHA1,
        .value = checksum_read_value("19b162f03f28273c383e5b834ec037518d751a05", ChecksumType::SHA1)
      }
    }
  });

  index.entries.push_back(IndexRecord {
    .path = "my/test/file2",
    .size = 123,
    .checksums = {
      {
        .type = ChecksumType::SHA1,
        .value = checksum_read_value("6c597dc1a66434091d7aef13c0294998c506501d", ChecksumType::SHA1)
      },
      {
        .type = ChecksumType::MD5,
        .value = checksum_read_value("d58a0606ed616820de291d594602665d", ChecksumType::MD5)
      }
    }
  });

  index.entries.push_back(IndexRecord {
    .path = "a/file with whitespace.txt",
    .size = 100,
    .checksums = {
      {
        .type = ChecksumType::MD5,
        .value = checksum_read_value("3ba9914955fd6025641300592b56a74b", ChecksumType::MD5)
      }
    }
  });

  index.entries.push_back(IndexRecord {
    .path = "b/exce\\\\ent\n🔥file\\\n\\ name",
    .size = 200,
    .checksums = {
      {
        .type = ChecksumType::MD5,
        .value = checksum_read_value("321060ae067e2a25091be3372719e053", ChecksumType::MD5)
      }
    }
  });

  auto index_path = test_get_tmpdir() / "index.lst";
  index_write(index_path.string(), index);

  EXPECT(test_read_file(index_path) == test_read_file("index.lst"));

  return EXIT_SUCCESS;
}

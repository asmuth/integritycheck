#include "index.h"
#include "test/assert.h"

#include <iostream>

int main(int argc, char** argv) {
  Index index;
  index_read("index.lst", &index);

  EXPECT(index.entries.size() == 2);
  EXPECT(index.entries[0].path == "my/test/file.bin");
  EXPECT(index.entries[0].size == 555);
  EXPECT(index.entries[0].checksums.size() == 1);
  EXPECT(index.entries[0].checksums[0].type == ChecksumType::SHA1);
  EXPECT(index.entries[0].checksums[0].value.data == "19b162f03f28273c383e5b834ec037518d751a05");

  EXPECT(index.entries[1].path == "my/test/file2");
  EXPECT(index.entries[1].size == 123);
  EXPECT(index.entries[1].checksums.size() == 2);
  EXPECT(index.entries[1].checksums[0].type == ChecksumType::SHA1);
  EXPECT(index.entries[1].checksums[0].value.data == "6c597dc1a66434091d7aef13c0294998c506501d");
  EXPECT(index.entries[1].checksums[1].type == ChecksumType::MD5);
  EXPECT(index.entries[1].checksums[1].value.data == "d58a0606ed616820de291d594602665d");

  return EXIT_SUCCESS;
}

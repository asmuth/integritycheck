#include "index.h"
#include "test/assert.h"

int main(int argc, char** argv) {
  Index index;
  index_read("index.lst", &index);

  EXPECT(index.entries.size() == 4);
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

  EXPECT(index.entries[2].path == "a/file with whitespace.txt");
  EXPECT(index.entries[2].size == 100);
  EXPECT(index.entries[2].checksums.size() == 1);
  EXPECT(index.entries[2].checksums[0].type == ChecksumType::MD5);
  EXPECT(index.entries[2].checksums[0].value.data == "3ba9914955fd6025641300592b56a74b");

  EXPECT(index.entries[3].path == "b/exce\\\\ent\n🔥file\\\n\\ name");
  EXPECT(index.entries[3].size == 200);
  EXPECT(index.entries[3].checksums.size() == 1);
  EXPECT(index.entries[3].checksums[0].type == ChecksumType::MD5);
  EXPECT(index.entries[3].checksums[0].value.data == "321060ae067e2a25091be3372719e053");

  return EXIT_SUCCESS;
}

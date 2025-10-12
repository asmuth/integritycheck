#include "index.h"

#include <fstream>
#include <sstream>

std::string index_write_record(const IndexRecord& record) {
  std::stringstream record_writer;

  for (const auto& path_char : record.path) {
    switch (path_char) {
      case ' ':
        record_writer << '\\';
        record_writer << ' ';
        break;
      case '\\':
        record_writer << '\\';
        record_writer << '\\';
        break;
      case '\n':
        record_writer << '\\';
        record_writer << 'n';
        break;
      default:
        record_writer << path_char;
        break;
    }
  }

  record_writer << ' ';
  record_writer << record.size;

  for (const auto& checksum : record.checksums) {
    record_writer << ' ';
    record_writer << checksum_write_type(checksum.type);
    record_writer << ':';
    record_writer << checksum_write_value(checksum);
  }

  return record_writer.str();
}

void index_write(
  const std::string& file_path,
  const Index& index
) {
  std::ofstream file_writer(file_path);
  if (!file_writer) {
    throw std::runtime_error("unable to open file: " + file_path);
  }

  for (const auto& record : index.entries) {
    file_writer << index_write_record(record) << std::endl;
    if (file_writer.bad()) {
      throw std::runtime_error("unable to write file: " + file_path);
    }
  }

  if (file_writer.close(); !file_writer) {
    throw std::runtime_error("unable to write file: " + file_path);
  }
}

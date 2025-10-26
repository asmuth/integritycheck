#include "index.h"

#include <fstream>
#include <sstream>
#include <cstring>

std::string index_read_record_field(
  const std::string& input,
  size_t* input_cursor
) {
  std::string output;

  if (*input_cursor >= input.size()) {
    throw std::runtime_error("invalid index record");
  }

  bool input_escape = false;
  while (*input_cursor < input.size()) {
    auto input_char = input.at((*input_cursor)++);

    if (input_escape) {
      switch (input_char) {
        case ' ':
          output += ' ';
          break;
        case '\\':
          output += '\\';
          break;
        case 'n':
          output += '\n';
          break;
        default:
          throw std::runtime_error("invalid escape sequence");
      }

      input_escape = false;
    } else if (input_char == ' ') {
      break;
    } else if (input_char == '\\') {
      input_escape = true;
    } else {
      output += input_char;
    }
  }

  return output;
}

void index_read_record_path(
  const std::string& input,
  size_t* input_cursor,
  IndexRecord* record
) {
  record->path = index_read_record_field(input, input_cursor);
}

void index_read_record_size(
  const std::string& input,
  size_t* input_cursor,
  IndexRecord* record
) {
  record->size = std::stoull(index_read_record_field(input, input_cursor));
}

void index_read_record_checksum(
  const std::string& input,
  size_t* input_cursor,
  IndexRecord* record
) {
  auto checksum_spec = index_read_record_field(input, input_cursor);
  auto checksum_type = checksum_spec.substr(0, checksum_spec.find(':'));
  auto checksum_value = checksum_spec.substr(checksum_type.size() + 1);

  Checksum checksum;
  checksum.type = checksum_read_type(checksum_type);
  checksum.value = checksum_read_value(checksum_value, checksum.type);

  record->checksums.push_back(checksum);
}

void index_read_record(
  const std::string& input,
  Index* index
) {
  size_t input_cursor = 0;

  IndexRecord record;
  index_read_record_path(input, &input_cursor, &record);
  index_read_record_size(input, &input_cursor, &record);

  while (input_cursor < input.size()) {
    index_read_record_checksum(input, &input_cursor, &record);
  }

  index_add(record, index);
}

void index_read_records(
  const std::string& file_path,
  Index* index
) {
  std::ifstream file_reader(file_path);
  if (!file_reader) {
    throw std::runtime_error("unable to open file: " + file_path);
  }

  std::string line;
  for (;;) {
    if (!std::getline(file_reader, line)) {
      if (file_reader.bad()) {
        throw std::runtime_error(
          "error while reading from file: " +
          file_path +
          ": " +
          std::strerror(errno)
        );
      } else {
        break;
      }
    }

    index_read_record(line, index);
  }
}

void index_read(
  const std::string& file_path,
  Index* index
) {
  index_read_records(file_path, index);
}

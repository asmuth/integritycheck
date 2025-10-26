#include <fmt/core.h>
#include <fmt/ranges.h>
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>

#include "cmd_search.h"
#include "output_progress.h"
#include "output_text.h"
#include "output_tty.h"
#include "update.h"
#include "verify.h"

enum class OpMode {
  CHECK,
  UPDATE,
  SEARCH,
  HELP
};

enum class OutputType {
  TTY,
  TEXT
};

struct Options {
  OpMode mode;
  std::string index_path;
  std::string data_path;
  OutputType output;
  bool progress;
};

void print_help() {
  std::cerr <<
    "Usage: $ filecheck [OPTION...]\n" \
    "   -i, --index=<path>            Index file path\n" \
    "   -d, --directory=<path>        Data directory path (default: '.')\n" \
    "   -c, --check                   Check the integrity of files referenced by the index file (default)\n" \
    "   -u, --update                  Update the index file\n" \
    "   -s, --search                  Search in the index file\n" \
    "   -?, --help                    Display this help text and exit\n" \
    "   -V, --version                 Display the version of this program and exit\n" \
    "\n" \
    "Output format:\n" \
    "   -o, --output=<format>         Output format (tty or text)\n" \
    "   -p, --progress                Enable progress output to STDERR\n" \
    "   -P, --noprogress              Disable progress output to STDERR\n" \
    "\n" \
    "Options for the 'check' mode:\n" \
    "   -q, --quick                   Disable checksum verification, only verify file presence and size\n" \
    "\n" \
    "Examples:\n" \
    "   $ filecheck -i index.lst\n" \
    "   $ filecheck -i index.lst -D path/to/files\n" \
    "   $ filecheck -i index.lst -u\n" \
    "   $ filecheck -i index.lst -s file1 file2\n" \
    "   $ filecheck -i index.lst -s - < file_list.txt\n" \
    ;
}

bool parse_output_type(Options* opts, const std::string& value) {
  if (value == "tty") {
    opts->output = OutputType::TTY;
    return true;
  } else if (value == "text") {
    opts->output = OutputType::TEXT;
    return true;
  } else {
    fmt::println(stderr, "ERROR: invalid output type");
    return false;
  }
}

bool parse_options(Options* opts, int argc, char** argv) {
  auto opts_short = std::string("i:d:cuso:pPh");
  auto opts_long = std::array<struct option, 10>{{
    {"index", required_argument, 0, 'i'},
    {"directory", required_argument, 0, 'd'},
    {"check", no_argument, 0, 'c'},
    {"update", no_argument, 0, 'u'},
    {"search", no_argument, 0, 's'},
    {"output", required_argument, 0, 'o'},
    {"progress", no_argument, 0, 'p'},
    {"noprogress", no_argument, 0, 'P'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
  }};

  for (optind = 1;;) {
    char opt = getopt_long(argc, argv, opts_short.c_str(), opts_long.data(), NULL);
    if (opt == -1) {
      break;
    }

    switch (opt) {
      case 'i':
        opts->index_path = optarg;
        break;
      case 'd':
        opts->data_path = optarg;
        break;
      case 'c':
        opts->mode = OpMode::CHECK;
        break;
      case 'u':
        opts->mode = OpMode::UPDATE;
        break;
      case 's':
        opts->mode = OpMode::SEARCH;
        break;
      case 'o':
        if (!parse_output_type(opts, optarg)) {
          return false;
        }
        break;
      case 'p':
        opts->progress = true;
        break;
      case 'P':
        opts->progress = false;
        break;
      case 'h':
        opts->mode = OpMode::HELP;
        break;
      case '?':
        return false;
    }
  }

  for (auto argn = optind; argn < argc; ++argn) {
    std::cerr << "ERROR: invalid argument: " << argv[argn] << std::endl;
    return false;
  }

  return true;
}

bool run_check(const Options& opts) {
  VerifyOp op;
  op.index_path = std::filesystem::path(opts.index_path);
  op.root_path = std::filesystem::path(opts.data_path);

  if (opts.progress) {
    output_progress::bind(&op.progress);
  }

  auto result = op_verify(op);

  switch (opts.output) {
    case OutputType::TTY:
      output_tty::print_result(result);
      break;
    case OutputType::TEXT:
      output_text::print_result(result);
      break;
  }

  if (op_verify_result_status(result) == VerifyResultStatus::PASS) {
    return true;
  } else {
    return false;
  }
}

bool run_update(const Options& opts) {
  UpdateOp op;
  op.index_path = std::filesystem::path(opts.index_path);
  op.root_path = std::filesystem::path(opts.data_path);
  op.checksum_type = ChecksumType::SHA1;

  if (opts.progress) {
    output_progress::bind(&op.progress);
  }

  update_index(op);
  return true;
}

int main(int argc, char** argv) {
  Options opts;
  opts.data_path = ".";
  opts.mode = OpMode::CHECK;
  opts.output = OutputType::TTY;
  opts.progress = false;
  if (!parse_options(&opts, argc, argv)) {
    return EXIT_FAILURE;
  }

  if (opts.mode == OpMode::HELP) {
    print_help();
    return EXIT_SUCCESS;
  }

  if (opts.index_path.empty()) {
    std::cerr << "ERROR: need an index file path (--index)" << std::endl;
    return EXIT_FAILURE;
  }

  bool result;
  switch (opts.mode) {
    case OpMode::CHECK:
      try {
        result = run_check(opts);
      } catch (const std::runtime_error& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        result = false;
      }
      break;
    case OpMode::UPDATE:
      try {
        result = run_update(opts);
      } catch (const std::runtime_error& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        result = false;
      }
      break;
    default:
      std::cerr << "ERROR: invalid command" << std::endl;
      result = false;
      break;
  }

  if (result) {
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}


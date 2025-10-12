#include <fmt/core.h>
#include <fmt/ranges.h>
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>

#include "cmd_search.h"
#include "op_verify.h"
#include "output_progress.h"

enum class OpMode {
  CHECK,
  UPDATE,
  SEARCH,
  HELP
};

struct Options {
  OpMode mode;
  std::vector<std::string> path_list;
  std::string output;
  bool progress;
};

void print_help() {
  std::cerr <<
    "Usage: $ flix [OPTION...] [INDEX] [PATH...]\n" \
    "   -c, --check                   Check the integrity of files referenced by the index file (default)\n" \
    "   -u, --update                  Update the index file\n" \
    "   -s, --search                  Search in the index file\n" \
    "   -?, --help                    Display this help text and exit\n" \
    "   -V, --version                 Display the version of this program and exit\n" \
    "\n" \
    "Output format:\n" \
    "   -o, --output                  Output format (tty or text)\n" \
    "   -p, --progress                Enable progress output to STDERR\n" \
    "   -P, --noprogress              Disable progress output to STDERR\n" \
    "\n" \
    "Options for the 'check' mode:\n" \
    "   -q, --quick                   Disable checksum verification, only verify file presence and size\n" \
    "\n" \
    "Examples:\n" \
    "   $ flix index.lst .\n" \
    "   $ flix -u index.lst path/to/files\n" \
    "   $ flix -s index.lst file1 file2\n" \
    "   $ flix -s index.lst - < file_list.txt\n" \
    ;
}

bool parse_options(Options* opts, int argc, char** argv) {
  auto opts_short = std::string("hcuso:pP");
  auto opts_long = std::array<struct option, 8>{{
    {"help", no_argument, 0, 'h'},
    {"check", no_argument, 0, 'c'},
    {"update", no_argument, 0, 'u'},
    {"search", no_argument, 0, 's'},
    {"output", required_argument, 0, 'o'},
    {"progress", no_argument, 0, 'p'},
    {"noprogress", no_argument, 0, 'P'},
    {0, 0, 0, 0}
  }};

  for (optind = 1;;) {
    char opt = getopt_long(argc, argv, opts_short.c_str(), opts_long.data(), NULL);
    if (opt == -1) {
      break;
    }

    switch (opt) {
      case 'h':
        opts->mode = OpMode::HELP;
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
        opts->output = optarg;
        break;
      case 'p':
        opts->progress = true;
        break;
      case 'P':
        opts->progress = false;
        break;
      case '?':
        return false;
    }
  }

  for (auto argn = optind; argn < argc; ++argn) {
    opts->path_list.push_back(argv[argn]);
  }

  return true;
}

bool run_check(const Options& opts) {
  VerifyOp op;
  VerifyOutputType output_type = VerifyOutputType::TTY;

  if (opts.path_list.size() > 0) {
    op.index_path = std::filesystem::path(opts.path_list[0]);
  } else {
    std::cerr << "ERROR: need a an index file" << std::endl;
    return false;
  }

  if (opts.path_list.size() > 1) {
    op.root_path = std::filesystem::path(opts.path_list[1]);
  } else {
    op.root_path = std::filesystem::current_path();
  }

  if (opts.path_list.size() > 2) {
    std::cerr << "ERROR: extraneous arguments" << std::endl;
    return false;
  }

  if (opts.progress) {
    output_progress_bind(&op);
  }

  auto result = op_verify(op);

  if (opts.progress) {
    output_progress(result);
    output_progress_flush();
  }

  switch (output_type) {
    case VerifyOutputType::TTY:
      op_verify_output_result_tty(result);
      break;
    case VerifyOutputType::TEXT:
      op_verify_output_result_text(result);
      break;
  }

  if (op_verify_result_status(result) == VerifyResultStatus::PASS) {
    return true;
  } else {
    return false;
  }
}

int main(int argc, char** argv) {
  Options opts;
  opts.mode = OpMode::CHECK;
  opts.progress = false;
  if (!parse_options(&opts, argc, argv)) {
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
    case OpMode::HELP:
      print_help();
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


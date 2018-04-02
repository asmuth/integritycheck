usage: fhistory <command> [options]

global options:
  -d,--data_dir=PATH     Set the path of the repository/data directory
  -x,--index_dir=PATH    Set the path of the history/index directory
  --help=PATH            Print the help message for one of the commands and exit

commands:
  status  Display status of the repository (quick)
  ack     Acknowledge changes to files in the repository
  log     Display the history of the reposiroy
  fsck    Perform a full check of the repository's integrity
  help    Print the help message for one of the commands and exit

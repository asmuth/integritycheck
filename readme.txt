usage: fhistory <command> [options]
Another file integrity monitoring tool.

global options:
  -d,--data_dir=PATH     Set the path of the repository/data directory
  -x,--index_dir=PATH    Set the path of the history/index directory
  --help=PATH            Print the help message for one of the commands and exit

commands:
  diff      Compare the current state of the repository to a snapshot (quick)
  ack       Acknowledge changes to files in the repository and create a new snapshot
  log       Display a historical log of snapshots and changes to the repository
  fsck      Perform a full check of the repository's integrity
  version   Print the version of this program and exit
  help      Print the help message for one of the commands and exit

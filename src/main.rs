extern crate getopts;

use std::env;
use std::io;
use std::io::Write;
use std::process;
use getopts::Options;

mod op;
mod op_status;
mod index;

use op::*;

type Error = String;

const DEFAULT_DATA_DIR : &'static str = ".";
const DEFAULT_INDEX_DIR : &'static str = ".fhistory";

const USAGE : &'static str = "\
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
";

#[derive(Debug)]
enum Command {
  Help{ topic: Option<Operation> },
  Operation{ op: Operation, args: Vec<String> }
}

fn perform(op: Operation, args: &Vec<String>) -> Result<(), Error> {
  return Ok(());
}

fn usage(op: Option<Operation>) -> Result<(), Error> {
  let usage_msg = match op {
    Some(Operation::Status) => op_status::USAGE,
    _ => USAGE,
  };

  match std::io::stdout().write(usage_msg.as_bytes()) {
    Err(e) => Err(e.to_string()),
    Ok(_) => Ok(())
  }
}

fn main() {
  let args : Vec<String> = env::args().collect();
  let argsr : Vec<&str> = args.iter().map(|s| s.as_ref()).collect();

  let command = match argsr.get(1) {
    Some(&"help") =>
      match argsr.get(2) {
        Some(topic) =>
          Command::Help{ topic: Operation::from_str(topic) },
        None =>
          Command::Help{ topic: None },
      }
    Some(cmd) =>
      match Operation::from_str(cmd) {
        Some(op) =>
          if argsr[2..].iter().any(|x| *x == "--help") {
            Command::Help{ topic: Some(op) }
          } else {
            Command::Operation{ op: op, args: args[2..].to_vec() }
          },
        _ =>
          Command::Help{ topic: None },
      },
    _ =>
      Command::Help{ topic: None },
  };

  let result = match command {
    Command::Help{topic} => usage(topic),
    Command::Operation{op, args} => perform(op, &args),
    _ => usage(None),
  };

  if let Err(e) = result {
    writeln!(&mut std::io::stderr(), "ERROR: {}", e).expect("ERROR");
    std::process::exit(1);
  }
}

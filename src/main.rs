extern crate getopts;

use std::env;
use std::io::Write;
use getopts::Options;
mod op;
use op::*;

#[derive(Debug)]
enum Command {
  Help{ topic: Option<Operation> },
  Operation{ op: Operation, args: Vec<String> }
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

  println!("cmd: {:?}", command);
}

#[derive(Debug)]
pub enum Operation {
  Diff,
  History,
  Acknowledge
}

pub trait OperationHelp {
  fn usage(self: &Self) -> String;
}

impl Operation {

  pub fn from_str(str: &str) -> Option<Operation> {
    return match str {
      "ack" => Some(Operation::Acknowledge),
      "diff" => Some(Operation::Diff),
      "log" => Some(Operation::History),
      _ => None,
    };
  }

}

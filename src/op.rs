#[derive(Debug)]
pub enum Operation {
  Status,
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
      "log" => Some(Operation::History),
      "status" => Some(Operation::Status),
      _ => None,
    };
  }

}

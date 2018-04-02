#[derive(Debug)]
pub enum Operation {
  Status,
  Acknowledge
}

pub trait OperationHelp {
  fn usage(self: &Self) -> String;
}

impl Operation {

  pub fn from_str(str: &str) -> Option<Operation> {
    return match str {
      "status" => Some(Operation::Status),
      "ack" => Some(Operation::Acknowledge),
      _ => None,
    };
  }

}

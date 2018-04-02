#[derive(Debug)]
pub enum Operation {
  Status
}

impl Operation {

  pub fn from_str(str: &str) -> Option<Operation> {
    return match str {
      "status" => Some(Operation::Status),
      _ => None,
    };
  }

}

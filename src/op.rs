/**
 * integritycheck - https://github.com/asmuth/integritycheck
 * Copyright (c) 2018, Paul Asmuth <paul@asmuth.com>
 *
 * This file is part of the "integritycheck" project. integritycheck is free software
 * licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.
 */
#[derive(Debug)]
pub enum Operation {
  Acknowledge,
  Status,
  Verify,
  Index,
  History,
  Initialize
}

pub trait OperationHelp {
  fn usage(self: &Self) -> String;
}

impl Operation {

  pub fn from_str(str: &str) -> Option<Operation> {
    return match str {
      "ack" => Some(Operation::Acknowledge),
      "status" => Some(Operation::Status),
      "verify" => Some(Operation::Verify),
      "index" => Some(Operation::Index),
      "init" => Some(Operation::Initialize),
      "log" => Some(Operation::History),
      _ => None,
    };
  }

}

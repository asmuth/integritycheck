use std::io;
use std::path::PathBuf;

pub struct Index {
}

pub struct IndexList {
  path: PathBuf
}

impl IndexList {

  pub fn latest() -> Option<Index> {
    return None;
  }

}


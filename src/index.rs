use std::fs;
use std::path::{Path,PathBuf};
use std::collections::HashMap;
use regex::Regex;
use chrono::DateTime;

const INDEX_FILENAME_PATTERN : &'static str =
    r"^fhistory-(?P<date>\d{4}-\d\d-\d\dT\d\d:\d\d:\d\d(\.\d+)?(([+-]\d\d:\d\d)|Z)?)-(?P<hash>[a-z0-9]+)$";

#[derive(Clone, Debug)]
pub struct IndexReference {
  pub timestamp: i64,
  pub hash: String,
}

#[derive(Clone, Debug)]
pub struct IndexDirectory {
  index_dir: PathBuf,
  index_files: Vec<IndexReference>,
}

#[derive(Clone, Debug)]
pub struct IndexFileInfo {
  pub size_bytes: u64,
  pub modified_timestamp: Option<u64>,
  pub checksum_sha256: Option<String>
}

#[derive(Clone, Debug)]
pub struct IndexSnapshot {
  files: HashMap<String, IndexFileInfo>
}

impl IndexDirectory {

  pub fn open(data_dir: &Path, index_dir: &Path) -> Result<IndexDirectory, ::Error> {
    let index_dir : PathBuf = if index_dir.has_root() {
      index_dir.to_path_buf()
    } else {
      data_dir.join(index_dir)
    };

    let directory_listing = match fs::read_dir(&index_dir) {
      Ok(d) => d,
      Err(e) => return Err(e.to_string()),
    };

    let mut index_files = Vec::<IndexReference>::new();
    for entry in directory_listing {
      let entry = match entry {
        Ok(e) => e,
        Err(e) => return Err(e.to_string()),
      };

      let entry_fname = entry.file_name();
      let pattern = Regex::new(INDEX_FILENAME_PATTERN).unwrap();
      let pattern_match = match entry_fname.to_str().and_then(|x| pattern.captures(x)) {
        Some(m) => m,
        None => return Err(format!("invalid file in index directory: {:?}", entry_fname)),
      };

      let timestamp = match DateTime::parse_from_rfc3339(&pattern_match["date"]) {
        Ok(v) => v.timestamp(),
        Err(e) => return Err(format!("internal error: {}", e)),
      };

      index_files.push(IndexReference {
        timestamp: timestamp,
        hash: pattern_match["hash"].to_string()
      });
    }

    index_files.sort_by(|a, b| b.timestamp.cmp(&a.timestamp));

    return Ok(IndexDirectory {
      index_dir: index_dir,
      index_files: index_files,
    });
  }

  pub fn latest(self: &Self) -> Option<IndexReference> {
    return self.index_files.get(0).cloned();
  }

  pub fn list(self: &Self) -> &Vec<IndexReference> {
    return &self.index_files;
  }

  pub fn load(self: &Self, idxref: &IndexReference) -> Result<IndexSnapshot, ::Error> {
    return Err(format!("not yet implemented"));
  }

  pub fn append(self: &mut Self, idxsnap: &IndexSnapshot) -> Result<IndexReference, ::Error> {
    return Err(format!("not yet implemented"));
  }

}

impl IndexSnapshot {

  pub fn new() -> IndexSnapshot {
    return IndexSnapshot {
      files: HashMap::<String, IndexFileInfo>::new()
    }
  }

  pub fn list(self: &Self) -> Vec<String> {
    return self.files.iter().map(|(path, _)| path.clone()).collect();
  }

  pub fn get(self: &mut Self, path: &str) -> Option<&IndexFileInfo> {
    return self.files.get(path);
  }

  pub fn update(self: &mut Self, path: &str, info: &IndexFileInfo) {
    self.files.insert(path.to_owned(), info.to_owned());
  }

  pub fn merge(self: &mut Self, other: &IndexSnapshot) {
    for (k, v) in other.files.iter() {
      self.files.insert(k.to_owned(), v.to_owned());
    }
  }

  pub fn clear(self: &mut Self, path_prefix: &str) {
    let delete_paths : Vec<String> = self.files
        .iter()
        .filter(|&(path, _)| !path.starts_with(path_prefix))
        .map(|(path, _)| path.clone())
        .collect();

    for path in delete_paths {
      self.files.remove(&path);
    }
  }

}



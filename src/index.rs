use std::fs;
use std::fs::File;
use std::io::{Read,Write};
use std::path::{Path,PathBuf};
use std::collections::HashMap;
use std::time::{SystemTime, UNIX_EPOCH};
use regex::Regex;
use serde_json as json;

const INDEX_FILENAME_PATTERN : &'static str =
    r"^(?P<timestamp>\d+)-(?P<checksum>[a-z0-9]+)\.idx$";

#[derive(Clone, Debug)]
pub struct IndexReference {
  pub timestamp: i64,
  pub checksum: String,
}

#[derive(Clone, Debug)]
pub struct IndexDirectory {
  index_path: PathBuf,
  index_files: Vec<IndexReference>,
}

#[derive(Serialize, Deserialize, Clone, Debug)]
pub struct IndexFileInfo {
  pub size_bytes: u64,
  pub modified_timestamp: Option<i64>,
  pub checksum_sha256: Option<String>
}

#[derive(Serialize, Deserialize, Clone, Debug)]
pub struct IndexSnapshot {
  files: HashMap<String, IndexFileInfo>
}

impl IndexDirectory {

  pub fn open(data_dir: &Path, index_path: &Path) -> Result<IndexDirectory, ::Error> {
    let index_path : PathBuf = if index_path.has_root() {
      index_path.to_path_buf()
    } else {
      data_dir.join(index_path)
    };

    if !index_path.exists() {
      return Err(
          format!(
              "index not found at '{}'; maybe you need to run 'fhistory init' first?",
              index_path.to_str().unwrap_or("")));
    }

    let directory_listing = match fs::read_dir(&index_path) {
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

      let timestamp = match pattern_match["timestamp"].parse::<i64>() {
        Ok(v) => v,
        Err(e) => return Err(format!("internal error: {}", e)),
      };

      index_files.push(IndexReference {
        timestamp: timestamp,
        checksum: pattern_match["checksum"].to_string()
      });
    }

    index_files.sort_by(|a, b| b.timestamp.cmp(&a.timestamp));

    return Ok(IndexDirectory {
      index_path: index_path,
      index_files: index_files,
    });
  }

  pub fn create(index_path: &Path) -> Result<(), ::Error> {
    if let Err(e) = fs::create_dir(index_path) {
      return Err(format!("error while creating index directory: {}", e));
    }

    return Ok(());
  }

  pub fn latest(self: &Self) -> Option<IndexReference> {
    return self.index_files.get(0).cloned();
  }

  pub fn list(self: &Self) -> &Vec<IndexReference> {
    return &self.index_files;
  }

  pub fn load(self: &Self, reference: &IndexReference) -> Result<IndexSnapshot, ::Error> {
    let snapshot_path = self.index_path.join(&reference.filename());
    let mut snapshot_data = Vec::<u8>::new();
    let read_result =
        File::open(&snapshot_path)
        .and_then(|mut f| f.read_to_end(&mut snapshot_data));

    if let Err(e) = read_result {
      return Err(e.to_string());
    }

    return match json::from_str(&String::from_utf8_lossy(&snapshot_data)) {
      Ok(snap) => Ok(snap),
      Err(e) => Err(e.to_string()),
    };
  }

  pub fn append(self: &mut Self, snapshot: &IndexSnapshot) -> Result<IndexReference, ::Error> {
    let now = SystemTime::now();
    let snapshot_timestamp = match now.duration_since(UNIX_EPOCH) {
      Ok(v) => v.as_secs() as i64,
      Err(e) => return Err(format!("internal error: {}", e)),
    };

    if let Some(latest) = self.latest() {
      if snapshot_timestamp <= latest.timestamp {
        return Err(format!("a newer snapshot exists. did we go back into the future?"));
      }
    }

    let snapshot_encoded = json::to_string(&snapshot).unwrap();
    let snapshot_checksum = ::checksum::sha256(snapshot_encoded.as_bytes());

    let snapshot_ref = IndexReference {
      timestamp: snapshot_timestamp,
      checksum: snapshot_checksum.to_owned()
    };

    //println!("Write new index file {:?}", snapshot_ref.filename());

    let result =
        fs::File::create(self.index_path.join(snapshot_ref.filename()))
        .and_then(|mut f| f.write_all(snapshot_encoded.as_bytes()));

    return match result {
      Ok(_) => Ok(snapshot_ref),
      Err(e) => Err(format!("error while writing index: {}", e)),
    }
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

impl IndexReference {

  fn filename(self: &Self) -> String {
    return format!("{}-{}.idx", self.timestamp, &self.checksum);
  }

}

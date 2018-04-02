use std::fs;
use std::path::Path;
use walkdir::WalkDir;

pub fn scan(data_path: &Path, prefix: &str) -> Result<::IndexSnapshot, ::Error> {
  let index = scan_metadata(data_path, prefix)?;
  return Ok(index);
}

fn scan_metadata(data_path: &Path, prefix: &str) -> Result<::IndexSnapshot, ::Error> {
  let data_path = match fs::canonicalize(data_path) {
    Ok(e) => e,
    Err(e) => return Err(e.to_string()),
  };

  let mut index = ::IndexSnapshot::new();
  for entry in WalkDir::new(Path::new(&data_path).join(&prefix)) {
    let entry = match entry {
      Ok(v) => v,
      Err(e) => return Err(e.to_string()),
    };

    let entry_meta = match entry.metadata() {
      Ok(v) => v,
      Err(e) => return Err(e.to_string()),
    };

    let entry_path = match fs::canonicalize(entry.path()) {
      Ok(e) => e,
      Err(e) => return Err(e.to_string()),
    };

    let entry_path = match entry_path.strip_prefix(&data_path) {
      Ok(v) => v,
      Err(e) => return Err(e.to_string()),
    };

    let entry_path = match entry_path.to_str() {
      Some(v) => v,
      None => return Err(format!("invalid path")),
    };

    index.update(entry_path, &::IndexFileInfo {
      size_bytes: entry_meta.len(),
      modified_timestamp: None,
      checksum_sha256: None
    });
  }

  return Ok(index);
}

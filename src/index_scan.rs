use std::fs;
use std::path::Path;
use walkdir::WalkDir;

pub fn scan(data_path: &Path, prefix: &str) -> Result<::IndexSnapshot, ::Error> {
  let data_path = match fs::canonicalize(data_path) {
    Ok(e) => e,
    Err(e) => return Err(e.to_string()),
  };

  for entry in WalkDir::new(Path::new(&data_path).join(&prefix)) {
    let entry = match entry {
      Ok(e) => e,
      Err(e) => return Err(e.to_string()),
    };

    let entry_path = match fs::canonicalize(entry.path()) {
      Ok(e) => e,
      Err(e) => return Err(e.to_string()),
    };

    let entry_path = match entry_path.strip_prefix(&data_path) {
      Ok(e) => e,
      Err(e) => return Err(e.to_string()),
    };

    println!("{:?}", entry_path);
  }

  return Err(format!("not yet implemented"));
}

use std::fs;
use std::fs::File;
use std::io::Read;
use std::path::Path;
use walkdir::WalkDir;

pub struct ScanOptions {
  pub calculate_checksums: bool
}

pub fn scan(
    data_path: &Path,
    prefix: &str,
    opts: &ScanOptions) -> Result<::IndexSnapshot, ::Error> {
  let mut index = scan_metadata(data_path, prefix)?;

  if (opts.calculate_checksums) {
    index = scan_checksums(data_path, index)?;
  }

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

    if !entry_meta.is_file() {
      continue;
    }

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

    println!("Read metadata for {:?}", entry_path);
    index.update(entry_path, &::IndexFileInfo {
      size_bytes: entry_meta.len(),
      modified_timestamp: None,
      checksum_sha256: None
    });
  }

  return Ok(index);
}

fn scan_checksums(data_path: &Path, index: ::IndexSnapshot) -> Result<::IndexSnapshot, ::Error> {
  let mut index = index;

  for file_path in index.list() {
    let mut file_info = match &index.get(&file_path) {
      &Some(v) => v.to_owned(),
      &None => return Err(format!("invalid path")),
    };

    println!("Calculating SHA256 checksum for {:?}", file_path);

    let mut file_data = Vec::<u8>::new();
    if let Err(e) = File::open(&file_path).and_then(|mut f| f.read_to_end(&mut file_data)) {
      return Err(e.to_string());
    }

    file_info.checksum_sha256 = Some(::checksum::sha256(&file_data));
    index.update(&file_path, &file_info);
  }

  return Ok(index)
}


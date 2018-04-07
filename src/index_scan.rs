use std::fs;
use std::fs::File;
use std::io::Read;
use std::path::{Path,PathBuf};
use std::time::UNIX_EPOCH;
use walkdir::WalkDir;

pub struct ScanOptions {
  pub exclude_paths: Vec<PathBuf>,
  pub exclusive_paths: Option<Vec<PathBuf>>
}

pub fn scan_metadata(
    data_path: &Path,
    prefix: &str,
    opts: &ScanOptions) -> Result<::IndexSnapshot, ::Error> {
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

    let entry_mtime = entry_meta
        .modified()
        .and_then(|x| Ok(x.duration_since(UNIX_EPOCH).unwrap().as_secs() as i64))
        .ok();

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

    if !check_excludes(&Path::new(&entry_path), opts) {
      ::prompt::print_debug(&format!("Skipping file: {:?}", entry_path));
      continue;
    }

    ::prompt::print_debug(&format!("Reading file metadata: {:?}", entry_path));
    index.update(entry_path, &::IndexFileInfo {
      size_bytes: entry_meta.len(),
      modified_timestamp: entry_mtime,
      checksum: None
    });
  }

  return Ok(index);
}

pub fn scan_checksums(
    data_path: &Path,
    index: ::IndexSnapshot,
    opts: &ScanOptions) -> Result<::IndexSnapshot, ::Error> {
  let mut index = index;

  for file_path in index.list() {
    if !check_excludes(&Path::new(&file_path), opts) {
      ::prompt::print_debug(&format!("Skipping checksum calculation for {:?}", file_path));
      continue;
    }

    ::prompt::print_debug(&format!("Computing checksum for {:?}", file_path));
    let mut file_info = match &index.get(&file_path) {
      &Some(v) => v.to_owned(),
      &None => return Err(format!("invalid path")),
    };

    let file_path_abs = data_path.join(&file_path).to_str().unwrap_or("").to_owned();
    let mut file_data = Vec::<u8>::new();
    if let Err(e) = File::open(&file_path_abs).and_then(|mut f| f.read_to_end(&mut file_data)) {
      return Err(e.to_string());
    }

    file_info.checksum = Some(::checksum::sha256(&file_data));
    index.update(&file_path, &file_info);

    ::prompt::print_debug(&format!(
        "Checksum for {:?} => {:?}",
        file_path,
        file_info.checksum));
  }

  return Ok(index)
}

fn check_excludes(path: &Path, opts: &ScanOptions) -> bool {
  {
    let skip = opts
        .exclude_paths
        .iter()
        .any(|p| path.starts_with(p));

    if skip {
      return false;
    }
  }

  if let Some(ref exclusive_paths) = opts.exclusive_paths {
    let skip = !exclusive_paths
        .iter()
        .any(|p| path.starts_with(p));

    if skip {
      return false;
    }
  }

  return true;
}


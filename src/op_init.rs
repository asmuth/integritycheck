use std::path::{Path,PathBuf};
use getopts::Options;

pub const USAGE : &'static str = "\
usage: fhistory init [options]
Create a new index file.

options:
  -d,--data_dir=PATH     Set the path of the repository/data directory
                         default: '.'
  -x,--index_dir=PATH    Set the path of the index directory. Note that this
                         path is relative to the data directory. Absolute
                         paths are allowed. default: '.fh'
  --help                 Print this help message and exit
";

pub fn perform(args: &Vec<String>) -> Result<bool, ::Error> {
  let mut flag_cfg = Options::new();
  flag_cfg.optopt("d", "data_dir", "data_dir", "PATH");
  flag_cfg.optopt("x", "index_dir", "index_dir", "PATH");

  let flags = match flag_cfg.parse(args) {
    Ok(f) => f,
    Err(e) => return Err(e.to_string()),
  };

  let data_path = flags.opt_str("data_dir").unwrap_or(::DEFAULT_DATA_DIR.into());
  let index_path = flags.opt_str("index_dir").unwrap_or(::DEFAULT_INDEX_DIR.into());

  println!("[1/4] Creating index...");
  let mut index = ::IndexDirectory::create(
      &Path::new(&data_path),
      &Path::new(&index_path))?;

  println!("[2/4] Scanning file metadata...");
  let scan_opts = ::index_scan::ScanOptions {
    exclude_paths: vec!(PathBuf::from(&index_path)),
    exclusive_paths: None,
  };

  let mut snapshot = ::index_scan::scan_metadata(
      &Path::new(&data_path),
      ".",
      &scan_opts)?;

  println!("[3/4] Computing file checksums...");
  snapshot = ::index_scan::scan_checksums(
      &Path::new(&data_path),
      snapshot,
      &scan_opts)?;

  println!("[4/4] Committing new snapshot...");
  index.append(&snapshot)?;

  return Ok(true);
}

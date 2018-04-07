use std::path::{Path,PathBuf};
use getopts::Options;

pub const USAGE : &'static str = "\
usage: fhistory status [options]
Compare the current state of the repository to the latest snapshot

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
  flag_cfg.optflag("", "verify", "verify");

  let flags = match flag_cfg.parse(args) {
    Ok(f) => f,
    Err(e) => return Err(e.to_string()),
  };

  let verify = flags.opt_present("verify");
  let data_path = flags.opt_str("data_dir").unwrap_or(::DEFAULT_DATA_DIR.into());
  let index_path = flags.opt_str("index_dir").unwrap_or(::DEFAULT_INDEX_DIR.into());
  let index = ::IndexDirectory::open(&Path::new(&data_path), &Path::new(&index_path))?;

  let snapshot_target = match index.latest() {
    Some(idx) => index.load(&idx)?,
    None => return Err(format!("no snapshots"))
  };

  let mut snapshot_actual = ::index_scan::scan_metadata(
      &Path::new(&data_path),
      ".",
      &::index_scan::ScanOptions {
        exclude_paths: vec!(PathBuf::from(&index_path)),
        exclusive_paths: None,
      })?;

  if verify {
    snapshot_actual = ::index_scan::scan_checksums(
        &Path::new(&data_path),
        snapshot_actual,
        &::index_scan::ScanOptions {
          exclude_paths: vec!(PathBuf::from(&index_path)),
          exclusive_paths: None,
        })?;
  }

  let diff = ::index_diff::diff(&snapshot_target, &snapshot_actual);

  println!("diff: {:?}", diff);
  return Ok(diff.len() == 0);
}

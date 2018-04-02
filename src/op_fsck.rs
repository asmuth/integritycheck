use std::path::Path;
use getopts::Options;

pub const USAGE : &'static str = "\
usage: fhistory fsck [options]
Perform a full check of the repository's integrity

options:
  -d,--data_dir=PATH     Set the path of the repository/data directory
  -x,--index_dir=PATH    Set the path of the history/index directory
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
  let index = ::IndexDirectory::open(&Path::new(&data_path), &Path::new(&index_path))?;

  println!("[1/4] Loading index...");
  let snapshot_target = match index.latest() {
    Some(idx) => index.load(&idx)?,
    None => return Err(format!("no snapshots"))
  };

  println!("[2/4] Scanning file metadata...");
  let mut snapshot_actual = ::index_scan::scan_metadata(
      &Path::new(&data_path),
      ".")?;

  println!("[3/4] Computing file checksums...");
  snapshot_actual = ::index_scan::scan_checksums(
      &Path::new(&data_path),
      snapshot_actual)?;

  println!("[4/4] Computing diff...");
  let diff = ::index_diff::diff(&snapshot_target, &snapshot_actual);

  println!("diff: {:?}", diff);

  return Ok(diff.len() == 0);
}

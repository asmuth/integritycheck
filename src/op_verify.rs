use std::path::{Path,PathBuf};
use getopts::Options;
use colored::*;

pub const USAGE : &'static str = "\
usage: fhistory verify [options]
Perform a full check of the repository's integrity

options:
  -d,--data_dir=PATH     Set the path of the repository/data directory
                         default: '.'
  -x,--index_dir=PATH    Set the path of the index directory. Note that this
                         path is relative to the data directory. Absolute
                         paths are allowed. default: '.fh'
  --progress=[on/off]    Turn progress reporting on stderr on or off
                         default: on
  --colours=[on/off]     Turn coloured terminal output on or off
                         default: on
  -v,--verbose           Enable verbose output,
  -h,--help              Print this help message and exit
";

pub fn perform(args: &Vec<String>) -> Result<bool, ::Error> {
  let mut flag_cfg = Options::new();
  flag_cfg.optopt("d", "data_dir", "data_dir", "PATH");
  flag_cfg.optopt("x", "index_dir", "index_dir", "PATH");
  flag_cfg.optopt("", "progress", "progress", "ONOFF");
  flag_cfg.optopt("", "colours", "progress", "ONOFF");
  flag_cfg.optflag("v", "verbose", "verbose");

  let flags = match flag_cfg.parse(args) {
    Ok(f) => f,
    Err(e) => return Err(e.to_string()),
  };

  ::prompt::set_debug(flags.opt_present("verbose"));
  ::prompt::set_progress(flags.opt_str("progress") != Some("off".to_owned()));
  ::prompt::set_colours(flags.opt_str("colours") != Some("off".to_owned()));

  let data_path = flags.opt_str("data_dir").unwrap_or(::DEFAULT_DATA_DIR.into());
  let index_path = flags.opt_str("index_dir").unwrap_or(::DEFAULT_INDEX_DIR.into());

  ::prompt::print_progress_step(1, 4, "Loading index");
  let index = ::IndexDirectory::open(&Path::new(&data_path), &Path::new(&index_path))?;
  let snapshot_target_ref = index.latest();
  let snapshot_target = match &snapshot_target_ref {
    &Some(ref idx) => index.load(&idx)?,
    &None => return Err(format!("no snapshots"))
  };

  ::prompt::print_progress_step(2, 4, "Scanning file metadata");
  let scan_opts = ::index_scan::ScanOptions {
    exclude_paths: vec!(PathBuf::from(&index_path)),
    exclusive_paths: None,
  };

  let mut snapshot_actual = ::index_scan::scan_metadata(
      &Path::new(&data_path),
      ".",
      &scan_opts)?;

  ::prompt::print_progress_step(3, 4, "Computing file checksums");
  snapshot_actual = ::index_scan::scan_checksums(
      &Path::new(&data_path),
      snapshot_actual,
      &scan_opts)?;

  ::prompt::print_progress_step(4, 4, "Computing diff");
  let diff = ::index_diff::diff(&snapshot_target, &snapshot_actual);

  ::prompt::print_progress_complete();
  ::prompt::print_repository_path(&data_path);
  ::prompt::print_snapshot_time(snapshot_target_ref.unwrap().timestamp);
  ::prompt::print_repository_status(diff.len() == 0);
  ::prompt::print_diff(&diff);

  return Ok(diff.len() == 0);
}

use std::path::{Path,PathBuf};
use getopts::Options;

pub const USAGE : &'static str = "\
usage: fhistory ack [options] <path>
Acknowledge changes to files in the repository and create a new snapshot

options:
  -d,--data_dir=PATH     Set the path of the repository/data directory
                         default: '.'
  -x,--index_dir=PATH    Set the path of the index directory. Note that this
                         path is relative to the data directory. Absolute
                         paths are allowed. default: '.fh'
  --progress=[on/off]    Turn progress reporting on stderr on or off
                         default: off
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

  let pathspec = match flags.free.get(0) {
    Some(p) => p,
    None => return Err("need a path (e.g. 'fhistory ack .')".into()),
  };

  ::prompt::set_debug(flags.opt_present("verbose"));
  ::prompt::set_progress(flags.opt_str("progress") == Some("on".to_owned()));
  ::prompt::set_colours(flags.opt_str("colours") != Some("off".to_owned()));

  let data_path = flags.opt_str("data_dir").unwrap_or(::DEFAULT_DATA_DIR.into());
  let index_path = flags.opt_str("index_dir").unwrap_or(::DEFAULT_INDEX_DIR.into());

  ::prompt::print_progress_step(1, 4, "Loading index");
  let mut index = ::IndexDirectory::open(&Path::new(&data_path), &Path::new(&index_path))?;
  let mut snapshot = match index.latest() {
    Some(idx) => index.load(&idx)?,
    None => return Err(format!("no index snapshot found")),
  };

  ::prompt::print_progress_step(2, 4, "Scanning file metadata");
  let scan_opts = ::index_scan::ScanOptions {
    exclude_paths: vec!(PathBuf::from(&index_path)),
    exclusive_paths: None,
  };

  let mut updates = ::index_scan::scan_metadata(
      &Path::new(&data_path),
      ::IndexSnapshot::new(snapshot.checksum_function.to_owned()),
      &pathspec,
      &scan_opts)?;

  ::prompt::print_progress_step(3, 4, "Computing file checksums for changed files");
  updates = ::index_scan::scan_checksums(
      &Path::new(&data_path),
      updates,
      &scan_opts)?;

  ::prompt::print_progress_step(4, 4, "Committing new snapshot");
  snapshot.clear(&pathspec);
  snapshot.merge(&updates);
  let updated_ref = index.append(&snapshot)?;

  ::prompt::print_progress_complete();
  ::prompt::print_success(&format!("Created snapshot {:?}", updated_ref.checksum));

  return Ok(true);
}

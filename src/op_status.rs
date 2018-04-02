use std::path::Path;
use getopts::Options;

pub const USAGE : &'static str = "\
usage: fhistory status [options]
Display status of the repository (quick)

options:
  -d,--data_dir=PATH     Set the path of the repository/data directory
  -x,--index_dir=PATH    Set the path of the history/index directory
  --help                 Print this help message and exit
";

pub fn perform(args: &Vec<String>) -> Result<(), ::Error> {
  let mut flag_cfg = Options::new();
  flag_cfg.optopt("d", "data_dir", "data_dir", "PATH");
  flag_cfg.optopt("x", "index_dir", "index_dir", "PATH");

  let flags = match flag_cfg.parse(args) {
    Ok(f) => f,
    Err(e) => return Err(e.to_string()),
  };

  let data_dir = flags.opt_str("data_dir").unwrap_or(::DEFAULT_DATA_DIR.into());
  let index_dir = flags.opt_str("index_dir").unwrap_or(::DEFAULT_INDEX_DIR.into());
  let index_list = ::IndexDirectory::open(&Path::new(&data_dir), &Path::new(&index_dir))?;

  let index = match index_list.latest() {
    Some(idx) => idx,
    None => return Err(format!("no snapshots"))
  };

  return Ok(());
}

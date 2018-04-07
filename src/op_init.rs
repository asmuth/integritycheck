use std::path::Path;
use getopts::Options;

pub const USAGE : &'static str = "\
usage: fhistory init [options]
Create a new index file.

options:
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
  let index = ::IndexDirectory::create(&Path::new(&data_path), &Path::new(&index_path))?;

  return Ok(true);
}

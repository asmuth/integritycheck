/**
 * fhistory - https://github.com/asmuth/fhistory
 * Copyright (c) 2018, Paul Asmuth <paul@asmuth.com>
 *
 * This file is part of the "fhistory" project. fhistory is free software
 * licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.
 */
use std::path::{Path,PathBuf};
use getopts::Options;

pub const USAGE : &'static str = "\
usage: fhistory log [options]
Display a historical log of snapshots and changes to the repository

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

  ::prompt::set_debug(flags.opt_present("verbose"));
  ::prompt::set_progress(flags.opt_str("progress") == Some("on".to_owned()));
  ::prompt::set_colours(flags.opt_str("colours") != Some("off".to_owned()));

  let data_path = flags.opt_str("data_dir").unwrap_or(::DEFAULT_DATA_DIR.into());
  let index_path = flags.opt_str("index_dir").unwrap_or(::DEFAULT_INDEX_DIR.into());

  ::prompt::print_progress_step(1, 1, "Loading index");
  let index = ::IndexDirectory::open(&Path::new(&data_path), &Path::new(&index_path))?;

  ::prompt::print_progress_complete();
  ::prompt::print_repository_path(&data_path);
  ::prompt::print_snapshot_table(&index)?;

  return Ok(true);
}

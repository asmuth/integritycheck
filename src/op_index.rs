/**
 * integritycheck - https://github.com/asmuth/integritycheck
 * Copyright (c) 2018, Paul Asmuth <paul@asmuth.com>
 *
 * This file is part of the "integritycheck" project. integritycheck is free software
 * licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.
 */
use std::path::{Path,PathBuf};
use std::fs;
use std::io;
use std::io::Write;
use getopts::Options;

pub const USAGE : &'static str = "\
usage: integritycheck index [options] <path>...
Create an index file containing checksums of all files in a directory
Compare the current state of the repository to the latest snapshot

options:
  -d,--data_dir=PATH     Set the path of the repository/data directory
                         default: '.'
  --progress=[on/off]    Turn progress reporting on stderr on or off
                         default: off
  --colours=[on/off]     Turn coloured terminal output on or off
                         default: on
  -v,--verbose           Enable verbose output,
  -h,--help              Print this help message and exit
";

pub fn perform(args: &Vec<String>) -> Result<bool, ::Error> {
  let mut flag_cfg = Options::new();
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

  let mut pathspecs = Vec::<PathBuf>::new();
  for pathspec in &flags.free {
    pathspecs.push(PathBuf::from(pathspec));
  }

  if pathspecs.len() == 0 {
    return Err("need a path (e.g. 'integritycheck index .')".into());
  }

  ::prompt::print_progress_step(1, 1, "Scanning file metadata");
  let mut snapshot = ::index_scan::scan_metadata(
      &Path::new(&pathspecs.get(0).unwrap()), // FIXME
      ::IndexSnapshot::new(::checksum::ChecksumFunction::SHA256),
      &::index_scan::ScanOptions {
        exclude_paths: vec!(),
        exclusive_paths: None,
      })?;

  snapshot = ::index_scan::scan_checksums(
      &Path::new(&pathspecs.get(0).unwrap()), // FIXME
      snapshot.to_owned(),
      &::index_scan::ScanOptions {
        exclude_paths: vec!(),
        exclusive_paths: None
      })?;

  ::prompt::print_progress_complete();
  ::prompt::print_repository_size(&snapshot);

  io::stdout().write(snapshot.unparse().as_bytes());

  return Ok(true);
}

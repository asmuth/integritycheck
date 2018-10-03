/**
 * fhistory - https://github.com/asmuth/fhistory
 * Copyright (c) 2018, Paul Asmuth <paul@asmuth.com>
 *
 * This file is part of the "fhistory" project. fhistory is free software
 * licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.
 */
use std::fs;
use std::path::{Path,PathBuf};
use std::time::{SystemTime, UNIX_EPOCH};
use getopts::Options;

pub const USAGE : &'static str = "\
usage: fhistory ack [options] <path>
Acknowledge changes to files in the repository and create a new snapshot

options:
  -m,--message=MSG       Set a message to be stored along with the snapshot
  -y,--noconfirm         Don't prompt to confirm changes
  -d,--data_dir=PATH     Set the path of the repository/data directory
                         default: '.'
  -x,--index_dir=PATH    Set the path of the index directory. Note that this
                         path is relative to the data directory. Absolute
                         paths are allowed. default: '.fh'
  --progress=[on/off]    Turn progress reporting on stderr on or off
                         default: off
  --colours=[on/off]     Turn coloured terminal output on or off
                         default: on
  --set_time=TIMESTAMP   Use the specified current unix microsecond timestamp
                         instead of the real system time
  -v,--verbose           Enable verbose output,
  -h,--help              Print this help message and exit
";

pub fn perform(args: &Vec<String>) -> Result<bool, ::Error> {
  let mut flag_cfg = Options::new();
  flag_cfg.optopt("m", "message", "message", "MSG");
  flag_cfg.optflag("y", "noconfirm", "noconfirm");
  flag_cfg.optopt("d", "data_dir", "data_dir", "PATH");
  flag_cfg.optopt("x", "index_dir", "index_dir", "PATH");
  flag_cfg.optopt("", "progress", "progress", "ONOFF");
  flag_cfg.optopt("", "colours", "progress", "ONOFF");
  flag_cfg.optopt("", "set_time", "set_time", "TIMESTAMP");
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

  let data_path_abs = match fs::canonicalize(&data_path) {
    Ok(p) => p,
    Err(e) => return Err(e.to_string()),
  };

  let time = match flags.opt_str("set_time").and_then(|x| x.parse::<i64>().ok()) {
    Some(time) => time,
    None => {
      let now = SystemTime::now();
      match now.duration_since(UNIX_EPOCH) {
        Ok(v) => v.as_secs() as i64 * 1_000_000 + v.subsec_nanos() as i64 / 1_000,
        Err(e) => return Err(format!("internal error: {}", e)),
      }
    }
  };

  //let mut pathspecs = Vec::<PathBuf>::new();
  //for pathspec in &flags.free {
  //  let pathspec = match fs::canonicalize(pathspec) {
  //    Ok(e) => e,
  //    Err(e) => return Err(e.to_string()),
  //  };

  //  if !pathspec.starts_with(&data_path_abs) {
  //    return Err(format!("path is outside of repository: {:?}", pathspec));
  //  }

  //  match pathspec.strip_prefix(&data_path_abs) {
  //    Ok(v) => pathspecs.push(PathBuf::from(v)),
  //    Err(e) => return Err(e.to_string()),
  //  };
  //}

  //if pathspecs.len() == 0 {
  //  return Err("need a path (e.g. 'fhistory ack .')".into());
  //}

  ::prompt::print_progress_step(1, 4, "Loading index");
  let mut index = ::IndexDirectory::open(
      &Path::new(&data_path),
      &Path::new(&index_path))?;

  let snapshot_old = match index.latest() {
    Some(idx) => index.load(&idx)?,
    None => return Err(format!("no index snapshot found")),
  };

  ::prompt::print_progress_step(2, 4, "Scanning file metadata");

  let mut snapshot_new = ::index_scan::scan_metadata(
      &Path::new(&data_path),
      ::IndexSnapshot::new(snapshot_old.checksum_function.to_owned()),
      &::index_scan::ScanOptions {
        exclude_paths: vec!(PathBuf::from(&index_path)),
        exclusive_paths: None,
      })?;

  ::prompt::print_progress_step(3, 4, "Computing file checksums for changed files");

  snapshot_new = ::index_scan::copy_checksums(
      snapshot_new.to_owned(),
      &snapshot_old)?;

  snapshot_new = ::index_scan::scan_checksums(
      &Path::new(&data_path),
      snapshot_new.to_owned(),
      &::index_scan::ScanOptions {
        exclude_paths: vec!(PathBuf::from(&index_path)),
        exclusive_paths: None
      })?;

  ::prompt::print_progress_step(4, 4, "Computing diff");
  let diffs = ::index_diff::diff(&snapshot_old, &snapshot_new);

  ::prompt::print_progress_complete();

  if diffs.len() == 0 {
    ::prompt::print_success(&format!("Nothing to commit"));
    return Ok(true);
  }

  if !flags.opt_present("noconfirm") {
    if !::prompt::confirm_diffs(&diffs) {
      return Ok(false);
    }
  } else {
    ::prompt::print_confirmed_diffs(&diffs);
  }

  snapshot_new.message = flags.opt_str("message");

  let updated_ref = index.append(&snapshot_new, time)?;
  ::prompt::print_success(&format!("Created snapshot {:?}", updated_ref.checksum));

  return Ok(true);
}

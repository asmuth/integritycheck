/**
 * fhistory - https://github.com/asmuth/fhistory
 * Copyright (c) 2018, Paul Asmuth <paul@asmuth.com>
 *
 * This file is part of the "fhistory" project. fhistory is free software
 * licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.
 */
use std;
use std::io::Write;
use colored;
use colored::*;
use libc;
use time;

#[allow(non_upper_case_globals)]
static mut enable_progress : bool = false;

#[allow(non_upper_case_globals)]
static mut enable_debug : bool = false;

pub fn set_progress(opt: bool) {
  unsafe {
    enable_progress = opt;
  }
}

pub fn set_debug(opt: bool) {
  unsafe {
    enable_debug = opt;
  }
}

pub fn set_colours(opt: bool) {
  colored::control::set_override(opt);
}

pub fn print_progress_step(step: u32, steps_total: u32, msg: &str) {
  unsafe {
    if !enable_progress {
      return;
    }
  }

  let res = writeln!(
      &mut std::io::stderr(),
      "{} {}",
      format!("[{}/{}]", step, steps_total).white().dimmed(),
      msg);

  res.expect("cannot write to stderr");
}

pub fn print_progress_complete() {
  unsafe {
    if !enable_progress {
      return;
    }
  }

  writeln!(&mut std::io::stderr(), "").expect("cannot write to stderr");
}

pub fn print_debug(msg: &str) {
  unsafe {
    if !enable_debug {
      return;
    }
  }

  let res = writeln!(
      &mut std::io::stderr(),
      "{} {}",
      "DEBUG".white().dimmed(),
      msg);

  res.expect("cannot write to stderr");
}

pub fn print_success(msg: &str) {
  println!("{}", msg.green());
}

pub fn print_repository_path(path: &str) {
  if let Ok(path) = std::fs::canonicalize(std::path::Path::new(&path)) {
    println!("Repository: {}", path.to_str().unwrap_or("ERROR"));
  } else {
    println!("Repository: {}", path);
  }
}

pub fn print_repository_size(snap: &::IndexSnapshot) {
  println!("Total Size: {}B ({} files)", snap.total_size_bytes(), snap.total_file_count());
}

pub fn print_repository_status(status: bool) {
  println!("Status: {}", if status { "CLEAN".green() } else { "DIRTY".red() });
}

pub fn print_snapshot_time(timestamp_us: i64) {
  let time = time::at(time::Timespec::new(timestamp_us / 1_000_000, 0));
  println!("Last Snapshot: {}", time.rfc822z());
}

pub fn print_diff(diff: &::index_diff::IndexDiffList) {
  let mut diff = diff.to_owned();
  if diff.len() == 0 {
    return;
  }

  let sort_name = |d: &::index_diff::IndexDiff| match d {
    &::index_diff::IndexDiff::Deleted{ref file} => file.to_owned(),
    &::index_diff::IndexDiff::Modified{ref file} => file.to_owned(),
    &::index_diff::IndexDiff::Renamed{ref from, ..} => from.to_owned(),
    &::index_diff::IndexDiff::Created{ref file} => file.to_owned(),
  };

  diff.sort_by(|a, b| sort_name(&a).cmp(&sort_name(&b)));

  let sort_rank = |d: &::index_diff::IndexDiff| match d {
    &::index_diff::IndexDiff::Deleted{..} => 1,
    &::index_diff::IndexDiff::Modified{..} => 2,
    &::index_diff::IndexDiff::Renamed{..} => 3,
    &::index_diff::IndexDiff::Created{..} => 4,
  };

  diff.sort_by(|a, b| sort_rank(&a).cmp(&sort_rank(&b)));

  print!("\n");

  for d in diff {
    let msg = match d {
      ::index_diff::IndexDiff::Created{ref file} =>
       format!("    created  {:?}", file).green(),
      ::index_diff::IndexDiff::Deleted{ref file} =>
       format!("    deleted  {:?}", file).red(),
      ::index_diff::IndexDiff::Modified{ref file} =>
       format!("    modified {:?}", file).yellow(),
      ::index_diff::IndexDiff::Renamed{ref from, ref to} =>
        format!("    renamed  {:?} -> {:?}", from, to).yellow()
    };

    println!("{}", msg);
  }

  print!("\n");
}

pub fn confirm_diffs(diff: &::index_diff::IndexDiffList) -> bool {
  println!("Acknowledging {} changes:", diff.len());
  print_diff(diff);
  print!("Apply changes? (y/n): ");
  std::io::stdout().flush().ok().expect("Could not flush stdout");

  let resp : i32;
  unsafe {
    resp = libc::getchar();
  };

  return match resp {
    121 => true,
    _ => false,
  };
}

pub fn print_confirmed_diffs(diff: &::index_diff::IndexDiffList) {
  println!("Changes ({})", diff.len());
  print_diff(diff);
}

pub fn print_snapshot_table(index: &::IndexDirectory) -> Result<(), ::Error> {
  for snap_ref in index.list() {
    println!("{}", format!("snapshot {}", snap_ref.checksum).yellow());
    let snap = index.load(snap_ref)?;
    let snap_time = time::at(time::Timespec::new(snap_ref.timestamp_us / 1_000_000, 0));
    println!("Timestamp: {}", snap_time.rfc822z());
    println!("Size: {}B ({} files)", snap.total_size_bytes(), snap.total_file_count());
    println!("\n    {}\n", snap.message.unwrap_or("<no message>".into()));
  }

  return Ok(());
}


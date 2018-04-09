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
use std::time::UNIX_EPOCH;
use walkdir::WalkDir;

pub struct ScanOptions {
  pub exclude_paths: Vec<PathBuf>,
  pub exclusive_paths: Option<Vec<PathBuf>>
}

pub fn scan_metadata(
    data_path: &Path,
    index: ::IndexSnapshot,
    opts: &ScanOptions) -> Result<::IndexSnapshot, ::Error> {
  let mut index = index;
  let mut stats_files_scanned = 0;
  let mut stats_bytes_scanned = 0;

  let data_path = match fs::canonicalize(data_path) {
    Ok(e) => e,
    Err(e) => return Err(e.to_string()),
  };

  for entry in WalkDir::new(Path::new(&data_path)) {
    let entry = match entry {
      Ok(v) => v,
      Err(e) => return Err(e.to_string()),
    };

    let entry_meta = match entry.metadata() {
      Ok(v) => v,
      Err(e) => return Err(e.to_string()),
    };

    if !entry_meta.is_file() {
      continue;
    }

    let entry_mtime_ms = entry_meta
        .modified()
        .and_then(|x| Ok(x.duration_since(UNIX_EPOCH).unwrap()))
        .ok()
        .map(|v| v.as_secs() as i64 * 1_000_000 + v.subsec_nanos() as i64 / 1_000);

    let entry_path = match fs::canonicalize(entry.path()) {
      Ok(e) => e,
      Err(e) => return Err(e.to_string()),
    };

    let entry_path = match entry_path.strip_prefix(&data_path) {
      Ok(v) => v,
      Err(e) => return Err(e.to_string()),
    };

    let entry_path = match entry_path.to_str() {
      Some(v) => v,
      None => return Err(format!("invalid path")),
    };

    if !check_excludes(&Path::new(&entry_path), opts) {
      ::prompt::print_debug(&format!("Skipping file: {:?}", entry_path));
      continue;
    }

    ::prompt::print_debug(&format!("Reading file metadata: {:?}", entry_path));
    index.update(entry_path, &::IndexFileInfo {
      size_bytes: entry_meta.len(),
      modified_timestamp_us: entry_mtime_ms,
      checksum: None
    });

    stats_files_scanned += 1;
    stats_bytes_scanned += entry_meta.len();

    ::prompt::print_scanprogress(
        stats_files_scanned,
        stats_bytes_scanned,
        0,
        0);
  }

  ::prompt::print_scanprogress_complete();

  return Ok(index);
}

pub fn scan_checksums(
    data_path: &Path,
    index: ::IndexSnapshot,
    opts: &ScanOptions) -> Result<::IndexSnapshot, ::Error> {
  let mut index = index;
  let mut stats_files_scanned = 0;
  let mut stats_bytes_scanned = 0;
  let stats_files_total = index.total_file_count();
  let stats_bytes_total = index.total_size_bytes();

  for file_path in index.list() {
    if !check_excludes(&Path::new(&file_path), opts) {
      ::prompt::print_debug(&format!("Skipping checksum calculation for {:?}", file_path));
      continue;
    }

    ::prompt::print_debug(&format!("Computing checksum for {:?}", file_path));
    let mut file_info = match &index.get(&file_path) {
      &Some(v) => v.to_owned(),
      &None => return Err(format!("invalid path")),
    };

    file_info.checksum = Some(::checksum::compute_file(
        index.checksum_function.clone(),
        &data_path.join(&file_path))?);

    index.update(&file_path, &file_info);

    ::prompt::print_debug(&format!(
        "Checksum for {:?} => {:?}",
        file_path,
        file_info.checksum));

    stats_files_scanned += 1;
    stats_bytes_scanned += file_info.size_bytes;

    ::prompt::print_scanprogress(
        stats_files_scanned,
        stats_bytes_scanned,
        stats_files_total,
        stats_bytes_total);
  }

  ::prompt::print_scanprogress_complete();

  return Ok(index)
}

fn check_excludes(path: &Path, opts: &ScanOptions) -> bool {
  {
    let skip = opts
        .exclude_paths
        .iter()
        .any(|p| path.starts_with(p));

    if skip {
      return false;
    }
  }

  if let Some(ref exclusive_paths) = opts.exclusive_paths {
    let skip = !exclusive_paths
        .iter()
        .any(|p| path.starts_with(p));

    if skip {
      return false;
    }
  }

  return true;
}


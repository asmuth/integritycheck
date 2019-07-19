/**
 * integritycheck - https://github.com/asmuth/integritycheck
 * Copyright (c) 2018, Paul Asmuth <paul@asmuth.com>
 *
 * This file is part of the "integritycheck" project. integritycheck is free software
 * licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.
 */
use std::path::PathBuf;
use std::collections::{HashMap,HashSet};

pub type IndexDiffList = Vec<IndexDiff>;

#[derive(Clone, Debug)]
pub enum IndexDiff {
  Created {
    file: PathBuf,
  },
  MetadataModified {
    file: PathBuf
  },
  Modified {
    file: PathBuf
  },
  Deleted {
    file: PathBuf,
  },
  Renamed {
    from: PathBuf,
    to: PathBuf
  },
}

#[derive(Clone, Debug, Hash, PartialEq, Eq)]
struct DeletedFile {
  file: PathBuf,
  modified_timestamp: Option<i64>,
}

pub fn diff(
    target: &::IndexSnapshot,
    actual: &::IndexSnapshot) -> IndexDiffList {
  let mut diffs = IndexDiffList::new();
  let mut deleted = HashMap::<String, Vec<DeletedFile>>::new();

  /* check that all files in the target index exist */
  for (fpath, finfo_target) in &target.files {
    match actual.get(fpath) {
      None => {
        diffs.push(IndexDiff::Deleted{
          file: fpath.into(),
        });

        if let Some(ref checksum) = finfo_target.checksum {
          if !deleted.contains_key(&checksum.to_owned()) {
            deleted.insert(checksum.to_owned(), Vec::<DeletedFile>::new());
          }

          deleted.get_mut(&checksum.to_owned()).unwrap().push(DeletedFile {
            file: fpath.into(),
            modified_timestamp: finfo_target.modified_timestamp_us.to_owned(),
          });
        }
      }
      Some(finfo_actual) =>
        if let Some(d) = compare_finfo(&fpath, finfo_target, finfo_actual) {
          diffs.push(d);
        }
    }
  }

  /* check for untracked files in the actual index */
  let mut renamed = HashSet::<PathBuf>::new();
  for (fpath, finfo) in &actual.files {
    if target.get(fpath).is_none() {
      if let Some(ref checksum) = finfo.checksum {
        let deleted_file_candidates = deleted.get(checksum);
        let deleted_file = deleted_file_candidates
            .and_then(|l| l.iter().find(|f| f.modified_timestamp == finfo.modified_timestamp_us))
            .or(deleted_file_candidates.and_then(|v| v.get(0)));

        if let Some(fpath_prev) = deleted_file {
          diffs.push(IndexDiff::Renamed {
            from: fpath_prev.file.to_owned(),
            to: fpath.into(),
          });

          renamed.insert(fpath_prev.file.to_owned());
          continue;
        }
      }

      diffs.push(IndexDiff::Created{
        file: fpath.into(),
      });
    }
  }

  /* collapse renames */
  diffs = diffs
      .iter()
      .cloned()
      .filter(|d| match d {
        &IndexDiff::Deleted{ref file} => !renamed.contains(file),
        _ => true,
      })
      .collect();

  return diffs;
}

pub fn list_files(diffs: &IndexDiffList) -> Vec<PathBuf> {
  let mut files = Vec::<PathBuf>::new();
  for d in diffs {
    let file = match d {
      &::index_diff::IndexDiff::Deleted{ref file} => file.to_owned(),
      &::index_diff::IndexDiff::Modified{ref file} => file.to_owned(),
      &::index_diff::IndexDiff::MetadataModified{ref file} => file.to_owned(),
      &::index_diff::IndexDiff::Renamed{ref from, ..} => from.to_owned(),
      &::index_diff::IndexDiff::Created{ref file} => file.to_owned(),
    };

    files.push(file);
  }

  return files;
}

// returns true if the files match and false if they dont match
fn compare_finfo(fpath: &String, target: &::IndexFileInfo, actual: &::IndexFileInfo) -> Option<IndexDiff> {
  if target.modified_timestamp_us != actual.modified_timestamp_us {
    return Some(IndexDiff::MetadataModified{file: fpath.into()});
  }

  if target.size_bytes != actual.size_bytes {
    return Some(IndexDiff::Modified{file: fpath.into()});
  }

  if actual.checksum.is_some() &&
     target.checksum != actual.checksum {
    return Some(IndexDiff::Modified{file: fpath.into()});
  }

  return None;
}



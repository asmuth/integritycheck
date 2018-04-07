use std::path::PathBuf;
use std::collections::{HashMap,HashSet};

type IndexDiffList = Vec<IndexDiff>;

#[derive(Clone, Debug)]
pub enum IndexDiff {
  Created {
    file: PathBuf,
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

pub fn diff(
    target: &::IndexSnapshot,
    actual: &::IndexSnapshot) -> IndexDiffList {
  let mut diffs = IndexDiffList::new();
  let mut deleted = HashMap::<String, PathBuf>::new();

  /* check that all files in the target index exist */
  for (fpath, finfo_target) in &target.files {
    match actual.get(fpath) {
      None => {
        diffs.push(IndexDiff::Deleted{
          file: fpath.into(),
        });

        if let Some(ref checksum) = finfo_target.checksum_sha256 {
          // use random file if multiple files with same checksum were deleted
          deleted.insert(checksum.to_owned(), fpath.into());
        }
      }
      Some(finfo_actual) =>
        if !compare_finfo(finfo_target, finfo_actual) {
          diffs.push(IndexDiff::Modified{file: fpath.into()});
        }
    }
  }

  /* check for untracked files in the actual index */
  let mut renamed = HashSet::<PathBuf>::new();
  for (fpath, finfo) in &actual.files {
    if target.get(fpath).is_none() {
      if let Some(ref checksum) = finfo.checksum_sha256 {
        if let Some(fpath_prev) = deleted.get(checksum).cloned() {
          diffs.push(IndexDiff::Renamed {
            from: fpath_prev.to_owned(),
            to: fpath.into(),
          });

          renamed.insert(fpath_prev.to_owned());
          deleted.remove(checksum);
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

// returns true if the files match and false if they dont match
fn compare_finfo(target: &::IndexFileInfo, actual: &::IndexFileInfo) -> bool {
  if target.size_bytes != actual.size_bytes ||
     target.modified_timestamp != actual.modified_timestamp {
    return false; // metadata mismatch
  }

  if actual.checksum_sha256.is_some() &&
     target.checksum_sha256 != actual.checksum_sha256 {
    return false; // checksum mismatch
  }

  return true;
}



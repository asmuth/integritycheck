use std::path::PathBuf;
use std::collections::{HashMap,HashSet};

pub type IndexDiffList = Vec<IndexDiff>;

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

#[derive(Clone, Debug, Hash, PartialEq, Eq)]
struct IndexDiffKey {
  checksum: String,
  size_bytes: u64,
  modified_timestamp: Option<i64>,
}

pub fn diff(
    target: &::IndexSnapshot,
    actual: &::IndexSnapshot) -> IndexDiffList {
  let mut diffs = IndexDiffList::new();
  let mut deleted = HashMap::<IndexDiffKey, Vec<PathBuf>>::new();

  /* check that all files in the target index exist */
  for (fpath, finfo_target) in &target.files {
    match actual.get(fpath) {
      None => {
        diffs.push(IndexDiff::Deleted{
          file: fpath.into(),
        });

        if let Some(ref checksum) = finfo_target.checksum_sha256 {
          let diff_key = IndexDiffKey {
            checksum: checksum.to_owned(),
            size_bytes: finfo_target.size_bytes,
            modified_timestamp: finfo_target.modified_timestamp.to_owned()
          };

          if !deleted.contains_key(&diff_key) {
            deleted.insert(diff_key.to_owned(), Vec::<PathBuf>::new());
          }

          deleted.get_mut(&diff_key).unwrap().push(fpath.into());
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
        let diff_key = IndexDiffKey {
          checksum: checksum.to_owned(),
          size_bytes: finfo.size_bytes,
          modified_timestamp: finfo.modified_timestamp.to_owned(),
        };

        if let Some(fpath_prev) = deleted.get(&diff_key).and_then(|v| v.get(0)).cloned() {
          diffs.push(IndexDiff::Renamed {
            from: fpath_prev.to_owned(),
            to: fpath.into(),
          });

          renamed.insert(fpath_prev.to_owned());
          deleted.get_mut(&diff_key).unwrap().remove(0);
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



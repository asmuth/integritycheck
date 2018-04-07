use std::path::PathBuf;

type IndexDiffList = Vec<IndexDiff>;

#[derive(Clone, Debug)]
pub enum IndexDiff {
  Created {
    file: PathBuf
  },
  Modified {
    file: PathBuf
  },
  Deleted {
    file: PathBuf
  },
  Moved {
    src: PathBuf,
    dst: PathBuf
  },
}

pub fn diff(
    target: &::IndexSnapshot,
    actual: &::IndexSnapshot) -> IndexDiffList {
  let mut diffs = IndexDiffList::new();

  /* check that all files in the target index exist */
  for (fpath, finfo_target) in &target.files {
    match actual.get(fpath) {
      None =>
        diffs.push(IndexDiff::Deleted{file: fpath.into()}),
      Some(finfo_actual) =>
        if !compare_finfo(finfo_target, finfo_actual) {
          diffs.push(IndexDiff::Modified{file: fpath.into()});
        }
    }
  }

  /* check for untracked files in the actual index */
  for (fpath, _) in &actual.files {
    match target.get(fpath) {
      None => diffs.push(IndexDiff::Created{file: fpath.into()}),
      Some(_) => (),
    }
  }

  /* detect renames */
  diffs = detect_renames(&diffs);

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

fn detect_renames(diffs: &IndexDiffList) -> IndexDiffList {
  return diffs.to_owned();
}


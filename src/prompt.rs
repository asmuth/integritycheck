use std::io::Write;
use colored::*;

pub fn print_diff(diff: &::index_diff::IndexDiffList) {
  let mut diff = diff.to_owned();

  let sort_rank = |d: &::index_diff::IndexDiff| match d {
    &::index_diff::IndexDiff::Deleted{ref file} => 1,
    &::index_diff::IndexDiff::Modified{ref file} => 2,
    &::index_diff::IndexDiff::Renamed{ref from, ref to} => 3,
    &::index_diff::IndexDiff::Created{ref file} => 4,
  };

  diff.sort_by(|a, b| sort_rank(&a).cmp(&sort_rank(&b)));

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
}


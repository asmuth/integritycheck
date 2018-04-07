use std::io::Write;
use colored::*;

pub fn print_diff(diff: &::index_diff::IndexDiffList) {

  for d in diff {
    let msg = match d {
      &::index_diff::IndexDiff::Created{ref file} =>
        format!("    created  {:?}", file).green(),
      &::index_diff::IndexDiff::Deleted{ref file} =>
        format!("    deleted  {:?}", file).red(),
      &::index_diff::IndexDiff::Modified{ref file} =>
        format!("    modified {:?}", file).yellow(),
      &::index_diff::IndexDiff::Renamed{ref from, ref to} =>
        format!("    renamed  {:?} -> {:?}", from, to).yellow()
    };

    println!("{}", msg);
  }
}


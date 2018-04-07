use std::io::Write;

pub fn print_diff(diff: &::index_diff::IndexDiffList) {

  for d in diff {
    match d {
      &::index_diff::IndexDiff::Created{ref file} =>
        println!("    created  {:?}", file),
      &::index_diff::IndexDiff::Deleted{ref file} =>
        println!("    deleted  {:?}", file),
      &::index_diff::IndexDiff::Modified{ref file} =>
        println!("    modified {:?}", file),
      &::index_diff::IndexDiff::Renamed{ref from, ref to} =>
        println!("    renamed  {:?} -> {:?}", from, to)
    }
  }
}


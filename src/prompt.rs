use std;
use std::io::Write;
use colored;
use colored::*;

static mut enable_progress : bool = false;
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

pub fn print_snapshot_time(time: i64) {
  println!("Last Snapshot: {}", time);
}

pub fn print_diff(diff: &::index_diff::IndexDiffList) {
  let mut diff = diff.to_owned();
  if diff.len() == 0 {
    return;
  }

  let sort_name = |d: &::index_diff::IndexDiff| match d {
    &::index_diff::IndexDiff::Deleted{ref file} => file.to_owned(),
    &::index_diff::IndexDiff::Modified{ref file} => file.to_owned(),
    &::index_diff::IndexDiff::Renamed{ref from, ref to} => from.to_owned(),
    &::index_diff::IndexDiff::Created{ref file} => file.to_owned(),
  };

  diff.sort_by(|a, b| sort_name(&a).cmp(&sort_name(&b)));

  let sort_rank = |d: &::index_diff::IndexDiff| match d {
    &::index_diff::IndexDiff::Deleted{ref file} => 1,
    &::index_diff::IndexDiff::Modified{ref file} => 2,
    &::index_diff::IndexDiff::Renamed{ref from, ref to} => 3,
    &::index_diff::IndexDiff::Created{ref file} => 4,
  };

  diff.sort_by(|a, b| sort_rank(&a).cmp(&sort_rank(&b)));

  println!("");
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

  println!("");
}


#[derive(Clone, Debug)]
pub struct IndexDiff {}

pub fn diff(
    target: &::IndexSnapshot,
    actual: &::IndexSnapshot) -> Vec<IndexDiff> {
  //println!("target index: {:?}", target);
  //println!("actual index: {:?}", actual);
  return Vec::<IndexDiff>::new();
}


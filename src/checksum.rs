use crypto::digest::Digest;
use crypto::sha2::Sha256;

#[derive(Clone, Debug)]
pub enum ChecksumFunction {
  SHA256
}

pub fn checksum_function_from_str(s: &str) -> Result<ChecksumFunction, ::Error> {
  return match s {
    "sha256" => Ok(ChecksumFunction::SHA256),
    _ => return Err(format!("invalid checksum function: {}", s)),
  };
}

pub fn checksum_function_to_str(f: &ChecksumFunction) -> String {
  return match f {
    &ChecksumFunction::SHA256 => "sha256".into(),
  };
}

pub fn compute(checksum_fn: ChecksumFunction, data: &[u8]) -> String {
  match checksum_fn {
    ChecksumFunction::SHA256 => return compute_sha256(data),
  };
}

fn compute_sha256(data: &[u8]) -> String {
  let mut sha256 = Sha256::new();
  sha256.input(&data);
  return sha256.result_str();
}

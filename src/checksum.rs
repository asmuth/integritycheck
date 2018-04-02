use crypto::digest::Digest;
use crypto::sha2::Sha256;

pub fn sha256(data: &[u8]) -> String {
  let mut sha256 = Sha256::new();
  sha256.input(&data);
  return sha256.result_str();
}

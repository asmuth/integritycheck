/**
 * integritycheck - https://github.com/asmuth/integritycheck
 * Copyright (c) 2018, Paul Asmuth <paul@asmuth.com>
 *
 * This file is part of the "integritycheck" project. integritycheck is free software
 * licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.
 */
use std::fs::File;
use std::path::Path;
use std::io::Read;
use crypto::digest::Digest;
use crypto::sha2::Sha256;
use crypto::md5::Md5;

#[derive(Clone, Debug)]
pub enum ChecksumFunction {
  SHA256, MD5
}

pub fn checksum_function_from_str(s: &str) -> Result<ChecksumFunction, ::Error> {
  return match s {
    "sha256" => Ok(ChecksumFunction::SHA256),
    "md5" => Ok(ChecksumFunction::MD5),
    _ => return Err(format!("invalid checksum function: {}", s)),
  };
}

pub fn checksum_function_to_str(f: &ChecksumFunction) -> String {
  return match f {
    &ChecksumFunction::SHA256 => "sha256".into(),
    &ChecksumFunction::MD5 => "md5".into(),
  };
}

pub fn compute(checksum_fn: ChecksumFunction, data: &[u8]) -> String {
  match checksum_fn {
    ChecksumFunction::SHA256 => return compute_sha256(data),
    ChecksumFunction::MD5 => return compute_md5(data),
  };
}

pub fn compute_file(checksum_fn: ChecksumFunction, path: &Path) -> Result<String, ::Error> {
  match checksum_fn {
    ChecksumFunction::SHA256 => return compute_file_sha256(path),
    ChecksumFunction::MD5 => return compute_file_md5(path),
  };
}

fn compute_sha256(data: &[u8]) -> String {
  let mut digest = Sha256::new();
  digest.input(&data);
  return digest.result_str();
}

fn compute_file_sha256(path: &Path) -> Result<String, ::Error> {
  // FIXME
  let mut data = Vec::<u8>::new();
  if let Err(e) = File::open(&path).and_then(|mut f| f.read_to_end(&mut data)) {
    return Err(e.to_string());
  }

  let mut digest = Sha256::new();
  digest.input(&data);
  return Ok(digest.result_str());
}

fn compute_md5(data: &[u8]) -> String {
  let mut digest = Md5::new();
  digest.input(&data);
  return digest.result_str();
}

fn compute_file_md5(path: &Path) -> Result<String, ::Error> {
  // FIXME
  let mut data = Vec::<u8>::new();
  if let Err(e) = File::open(&path).and_then(|mut f| f.read_to_end(&mut data)) {
    return Err(e.to_string());
  }

  let mut digest = Md5::new();
  digest.input(&data);
  return Ok(digest.result_str());
}


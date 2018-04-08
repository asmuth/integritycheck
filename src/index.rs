/**
 * fhistory - https://github.com/asmuth/fhistory
 * Copyright (c) 2018, Paul Asmuth <paul@asmuth.com>
 *
 * This file is part of the "fhistory" project. fhistory is free software
 * licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.
 */
use std::fs;
use std::fs::File;
use std::io::{Read,Write};
use std::io::prelude::*;
use std::path::{Path,PathBuf};
use std::collections::{HashMap,BTreeMap};
use regex::Regex;
use inflate;
use deflate;

const INDEX_FILENAME_PATTERN : &'static str =
    r"^(?P<timestamp>\d+)-(?P<checksum>[a-z0-9]+)\.idx$";

#[derive(Clone, Debug)]
pub struct IndexReference {
  pub timestamp_us: i64,
  pub checksum: String,
}

#[derive(Clone, Debug)]
pub struct IndexDirectory {
  index_path: PathBuf,
  index_files: Vec<IndexReference>,
}

#[derive(Clone, Debug)]
pub struct IndexFileInfo {
  pub size_bytes: u64,
  pub modified_timestamp_us: Option<i64>,
  pub checksum: Option<String>
}

#[derive(Clone, Debug)]
pub struct IndexSnapshot {
  pub checksum_function: ::checksum::ChecksumFunction,
  pub files: BTreeMap<String, IndexFileInfo>,
  pub message: Option<String>,
}

impl IndexDirectory {

  pub fn open(data_dir: &Path, index_path: &Path) -> Result<IndexDirectory, ::Error> {
    let index_path : PathBuf = if index_path.has_root() {
      index_path.to_path_buf()
    } else {
      data_dir.join(index_path)
    };

    ::prompt::print_debug(&format!("Opening index directory at {:?}", index_path));
    if !index_path.exists() {
      return Err(
          format!(
              "index not found at '{}'; maybe you need to run 'fhistory init' first?",
              index_path.to_str().unwrap_or("")));
    }

    let directory_listing = match fs::read_dir(&index_path) {
      Ok(d) => d,
      Err(e) => return Err(e.to_string()),
    };

    let mut index_files = Vec::<IndexReference>::new();
    for entry in directory_listing {
      let entry = match entry {
        Ok(e) => e,
        Err(e) => return Err(e.to_string()),
      };

      let entry_fname = entry.file_name();
      let pattern = Regex::new(INDEX_FILENAME_PATTERN).unwrap();
      let pattern_match = match entry_fname.to_str().and_then(|x| pattern.captures(x)) {
        Some(m) => m,
        None => return Err(format!("invalid file in index directory: {:?}", entry_fname)),
      };

      let timestamp_us = match pattern_match["timestamp"].parse::<i64>() {
        Ok(v) => v,
        Err(e) => return Err(format!("internal error: {}", e)),
      };

      index_files.push(IndexReference {
        timestamp_us: timestamp_us,
        checksum: pattern_match["checksum"].to_string()
      });
    }

    index_files.sort_by(|a, b| b.timestamp_us.cmp(&a.timestamp_us));

    return Ok(IndexDirectory {
      index_path: index_path,
      index_files: index_files,
    });
  }

  pub fn create(data_dir: &Path, index_path: &Path) -> Result<IndexDirectory, ::Error> {
    let index_path : PathBuf = if index_path.has_root() {
      index_path.to_path_buf()
    } else {
      data_dir.join(index_path)
    };

    ::prompt::print_debug(&format!("Creating index directory at {:?}", index_path));
    if let Err(e) = fs::create_dir(&index_path) {
      return Err(format!("error while creating index directory: {}", e));
    }

    return Ok(IndexDirectory {
      index_path: index_path,
      index_files: Vec::<IndexReference>::new(),
    });
  }

  pub fn latest(self: &Self) -> Option<IndexReference> {
    return self.index_files.get(0).cloned();
  }

  pub fn list(self: &Self) -> &Vec<IndexReference> {
    return &self.index_files;
  }

  pub fn load(self: &Self, reference: &IndexReference) -> Result<IndexSnapshot, ::Error> {
    ::prompt::print_debug(&format!("Loading index snapshot {:?}", reference.filename()));
    let snapshot_path = self.index_path.join(&reference.filename());
    let mut snapshot_data_compressed = Vec::<u8>::new();
    let read_result =
        File::open(&snapshot_path)
        .and_then(|mut f| f.read_to_end(&mut snapshot_data_compressed));
    if let Err(e) = read_result {
      return Err(e.to_string());
    }

    let snapshot_data = match inflate::inflate_bytes_zlib(&snapshot_data_compressed) {
      Ok(data) => data,
      Err(e) => return Err(e.to_string()),
    };

    let mut snapshot = ::IndexSnapshot::decode(&snapshot_data, reference.timestamp_us)?;
    let snapshot_checksum = ::checksum::compute(
        snapshot.checksum_function.clone(),
        &snapshot_data_compressed);

    if snapshot_checksum != reference.checksum {
      return Err(format!("Checksum mismatch for index file: {:?}", snapshot_path));
    }

    return Ok(snapshot);
  }

  pub fn append(
      self: &mut Self,
      snapshot: &IndexSnapshot,
      snapshot_timestamp_us: i64) -> Result<IndexReference, ::Error> {
    if let Some(latest) = self.latest() {
      if snapshot_timestamp_us <= latest.timestamp_us {
        return Err(format!("a newer snapshot exists. did we go back into the future?"));
      }
    }

    let snapshot_encoded = snapshot.encode(snapshot_timestamp_us);
    let snapshot_encoded_compressed = deflate::deflate_bytes_zlib(&snapshot_encoded);
    let snapshot_checksum = ::checksum::compute(
        snapshot.checksum_function.clone(),
        &snapshot_encoded_compressed);

    let snapshot_ref = IndexReference {
      timestamp_us: snapshot_timestamp_us,
      checksum: snapshot_checksum.to_owned()
    };

    ::prompt::print_debug(&format!("Writing new index snapshot {:?}", snapshot_ref.filename()));
    let result =
        fs::File::create(self.index_path.join(snapshot_ref.filename()))
        .and_then(|mut f| f.write_all(&snapshot_encoded_compressed));

    return match result {
      Ok(_) => Ok(snapshot_ref),
      Err(e) => Err(format!("error while writing index: {}", e)),
    }
  }

}

impl IndexSnapshot {

  pub fn new(checksum_function: ::checksum::ChecksumFunction) -> IndexSnapshot {
    return IndexSnapshot {
      files: BTreeMap::<String, IndexFileInfo>::new(),
      checksum_function: checksum_function,
      message: None,
    }
  }

  pub fn list(self: &Self) -> Vec<String> {
    return self.files.iter().map(|(path, _)| path.clone()).collect();
  }

  pub fn get(self: &Self, path: &str) -> Option<&IndexFileInfo> {
    return self.files.get(path);
  }

  pub fn get_path(self: &Self, path: &Path) -> Option<&IndexFileInfo> {
    return self.files.get(path.to_str().unwrap());
  }

  pub fn update(self: &mut Self, path: &str, info: &IndexFileInfo) {
    self.files.insert(path.to_owned(), info.to_owned());
  }

  pub fn update_path(self: &mut Self, path: &Path, info: &IndexFileInfo) {
    self.files.insert(path.to_str().unwrap().to_owned(), info.to_owned());
  }

  pub fn delete(self: &mut Self, path: &str) {
    self.files.remove(path);
  }

  pub fn delete_path(self: &mut Self, path: &Path) {
    self.files.remove(path.to_str().unwrap());
  }

  pub fn total_size_bytes(self: &Self) -> u64 {
    return self
        .files
        .iter()
        .fold(0, |acc, (_, finfo)| acc + finfo.size_bytes);
  }

  pub fn total_file_count(self: &Self) -> u64 {
    return self.files.iter().count() as u64;
  }

  pub fn encode(self: &Self, timestamp_us: i64) -> Vec<u8> {
    let mut data = String::new();

    data += &format!(
        "#checksum {}\n",
        ::checksum::checksum_function_to_str(&self.checksum_function));

    data += &format!("#timestamp {}\n", timestamp_us);

    if let &Some(ref message) = &self.message {
      data += &format!("#message {}\n", encode_string(message));
    }

    for (fpath, finfo) in self.files.iter() {
      if finfo.checksum.is_none() {
        panic!("missing checksum");
      }

      data += &format!(
          "{} {} {} {}\n",
          finfo.checksum.as_ref().unwrap_or(&"".to_owned()),
          finfo.size_bytes,
          finfo.modified_timestamp_us.unwrap_or(0),
          encode_string(fpath));
    }

    return data.as_bytes().to_owned();
  }

  pub fn decode(data: &[u8], timestamp_expected_us: i64) -> Result<IndexSnapshot, ::Error> {
    let mut files = BTreeMap::<String, IndexFileInfo>::new();
    let mut checksum_function = String::new();
    let mut message : Option<String> = None;
    let mut timestamp_us : i64 = 0;

    let mut data = String::from_utf8_lossy(data);
    for line in data.lines() {
      let fields = line.split(" ").collect::<Vec<&str>>();

      if fields.len() == 2 && fields[0] == "#checksum" {
        checksum_function = fields[1].into();
        continue;
      }

      if fields.len() == 2 && fields[0] == "#timestamp" {
        timestamp_us = match fields[1].parse::<i64>() {
          Ok(v) => v,
          Err(e) => return Err(e.to_string()),
        };

        continue;
      }

      if fields.len() == 2 && fields[0] == "#message" {
        message = Some(decode_string(fields[1])?);
        continue;
      }

      if fields.len() == 4 {
        let field_checksum = fields[0];
        let field_mtime = fields[2];
        let field_path = decode_string(fields[3])?;
        let field_size = match fields[1].parse::<u64>() {
          Ok(s) => s,
          Err(_) => return Err(format!("invalid index file (invalid size): {:?}", line)),
        };

        files.insert(field_path, ::IndexFileInfo {
          checksum: Some(field_checksum.to_owned()),
          size_bytes: field_size,
          modified_timestamp_us: field_mtime.parse::<i64>().ok(),
        });

        continue;
      }

      return Err(format!("invalid index file: {:?}", line));
    }

    let checksum_function = ::checksum::checksum_function_from_str(&checksum_function)?;

    if timestamp_us != timestamp_expected_us {
      return Err(format!("timestamp does not match"));
    }

    return Ok(IndexSnapshot {
      files: files,
      checksum_function: checksum_function,
      message: message
    });
  }

}

impl IndexReference {

  fn filename(self: &Self) -> String {
    return format!("{}-{}.idx", self.timestamp_us, &self.checksum);
  }

}

fn encode_string(src: &str) -> String {
  let mut dst = String::new();

  for c in src.chars() {
    match c {
      '\n' => dst += "\\n",
      '\\' => dst += "\\\\",
      ' ' => dst += "\\_",
      _ => dst.push(c),
    }
  }

  return dst.to_owned();
}

fn decode_string(src: &str) -> Result<String, ::Error> {
  let mut dst = String::new();
  let mut escape = false;
  for c in src.chars() {
    if escape {
      match c {
        '\\' => dst.push('\\'),
        'n' => dst.push('\n'),
        '_' => dst.push(' '),
        _ => return Err(format!("invalid escape sequence")),
      };

      escape = false;
    } else {
      match c {
        '\\' => escape = true,
        _ => dst.push(c),
      };
    }
  }

  return Ok(dst.to_owned());
}


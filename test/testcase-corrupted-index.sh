#!/bin/bash
set -uex
source test/test-util.sh
mkdir "${TEST_TMPDIR}/repo"
cd "${TEST_TMPDIR}/repo"

echo "A" > testA
echo "B" > testB
echo "C" > testC

touch -m --date='2016-01-01 06:00:01' testA
touch -m --date='2016-01-01 06:00:02' testB
touch -m --date='2016-01-01 06:00:03' testC

fhistory init --checksum md5
fhistory status
fhistory verify

echo "boom" >> .fh/*

if fhistory status --colours=off &> "../status.raw"; then
  echo "exit code must be one"
  exit 1
fi

grep -qE "^ERROR: invalid index file:" ../status.raw

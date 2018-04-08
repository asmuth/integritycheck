#!/bin/bash
# fhistory - https://github.com/asmuth/fhistory
# Copyright (c) 2018, Paul Asmuth <paul@asmuth.com>
#
# This file is part of the "fhistory" project. fhistory is free software
# licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.
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

echo "boom" >> .fh/$(ls -t1 .fh/ | head -n 1)

if fhistory status --colours=off &> "../status.raw"; then
  echo "exit code must be one"
  exit 1
fi

grep -qE "^ERROR: Checksum mismatch for index file:" ../status.raw

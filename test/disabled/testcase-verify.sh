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

touch -m --date='2016-01-01 06:00:00' testA
touch -m --date='2016-01-01 06:00:00' testB
touch -m --date='2016-01-01 06:00:00' testC

fhistory init
fhistory status -v

echo "X" > testB
touch -m --date='2016-01-01 06:00:00' testB

fhistory status

if fhistory verify --colours=off > "../status.raw"; then
  echo "exit code must be one"
  exit 1
fi

cat "../status.raw" | grep -vE "^Repository" | grep -vE "^Last Snapshot" > "../status"

(cat > "../status.expected") <<EOF
Total Size: 6B (3 files)
Status: DIRTY

    modified "testB"

EOF

diff "../status" "../status.expected"

sleep 0.01

fhistory ack -y .
fhistory status

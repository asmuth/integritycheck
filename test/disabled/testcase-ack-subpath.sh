#!/bin/bash
# fhistory - https://github.com/asmuth/fhistory
# Copyright (c) 2018, Paul Asmuth <paul@asmuth.com>
#
# This file is part of the "fhistory" project. fhistory is free software
# licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.
set -uex

source ./test/test-util.sh
mkdir "${TEST_TMPDIR}/repo"
cd "${TEST_TMPDIR}/repo"

echo "A" > testA
echo "B" > testB
echo "C" > testC

fhistory init
fhistory status

echo "X" > testX
echo "C2" > testC

mkdir testDir
touch testDir/1
touch testDir/2
touch testDir/3

if fhistory status --colours=off > "../status.raw"; then
  echo "exit code must be one"
  exit 1
fi

cat "../status.raw" | grep -vE "^Repository" | grep -vE "^Last Snapshot" > "../status"

(cat > "../status.expected") <<EOF
Total Size: 6B (3 files)
Status: DIRTY

    modified "testC" (metadata modifications only)
    created  "testDir/1"
    created  "testDir/2"
    created  "testDir/3"
    created  "testX"

EOF

cat ../status
diff "../status" "../status.expected"

sleep 0.01

fhistory ack -y testX testDir

if fhistory status --colours=off > "../status.raw"; then
  echo "exit code must be one"
  exit 1
fi

cat "../status.raw" | grep -vE "^Repository" | grep -vE "^Last Snapshot" > "../status"

(cat > "../status.expected") <<EOF
Total Size: 8B (7 files)
Status: DIRTY

    modified "testC" (metadata modifications only)

EOF

diff "../status" "../status.expected"

sleep 0.01

fhistory ack -y testC

fhistory status # must be clean

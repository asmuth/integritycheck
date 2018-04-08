#!/bin/bash
set -uex
source test/test-util.sh
mkdir "${TEST_TMPDIR}/repo"
cd "${TEST_TMPDIR}/repo"

touch -m --date='2016-01-01 06:00:00' testA
touch -m --date='2016-01-01 06:00:00' testB
touch -m --date='2016-01-01 06:00:00' testC

fhistory init
fhistory status -v

mv testA testB1
mv testB testA1

if fhistory status --colours=off > "../status.raw"; then
  echo "exit code must be one"
  exit 1
fi

cat "../status.raw" | grep -vE "^Repository" | grep -vE "^Last Snapshot" > "../status"

(cat > "../status.expected") <<EOF
Total Size: 0B (3 files)
Status: DIRTY

    renamed  "testA" -> "testA1"
    renamed  "testB" -> "testB1"

EOF

diff "../status" "../status.expected"

sleep 0.01

fhistory ack .
fhistory status

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

echo "X" > testB

if fhistory ack /tmp --colours=off &> "../output.raw"; then
  echo "exit code must be one"
  exit 1
fi

grep -qE "^ERROR: path is outside of repository:" ../output.raw

if fhistory status --colours=off > "../status.raw"; then
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

fhistory ack .
fhistory status

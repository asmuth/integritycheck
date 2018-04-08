#!/bin/bash
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

sleep 1

fhistory ack .
fhistory status

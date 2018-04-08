#!/bin/bash
set -uex
source test/test-util.sh
mkdir "${TEST_TMPDIR}/repo"
cd "${TEST_TMPDIR}/repo"

echo "XXX" > testA
echo "XXX" > testB
echo "XXX" > testC

touch -m --date='2016-01-01 06:00:01' testA
touch -m --date='2016-01-01 06:00:02' testB
touch -m --date='2016-01-01 06:00:03' testC

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
Total Size: 12B (3 files)
Status: DIRTY

    renamed  "testA" -> "testB1"
    renamed  "testB" -> "testA1"

EOF

diff "../status" "../status.expected"

sleep 1

fhistory ack .
fhistory status

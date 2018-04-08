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

fhistory init
fhistory status
fhistory ack . --colours=off &> "../output.raw"
grep -qE "^Nothing to commit" ../output.raw
test $(ls -1 .fh/*.idx | wc -l) -eq 1

#!/bin/bash
set -ue
source ./test/test-util.sh
mkdir "${TEST_TMPDIR}/repo"
cd "${TEST_TMPDIR}/repo"

echo "A" > testA
echo "B" > testB
echo "C" > testC

fhistory init
fhistory status

echo "X" > testX

if fhistory status; then
  echo "exit code must be one"
  exit 1
fi

sleep 1

fhistory ack testX

fhistory status # must be clean

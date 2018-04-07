#!/bin/bash
source test-util.sh
mkdir "${TEST_TMPDIR}/repo"
cd "${TEST_TMPDIR}/repo"

fhistory init

echo "A" > testA
echo "B" > testB
echo "C" > testC

fhistory ack .

fhistory status || true # FIXME

mv testB testX

if fhistory status &> "${TEST_TMPDIR}/status"; then
  echo "exit code must be one"
  exit 1
fi

sleep 1

fhistory ack .

fhistory status

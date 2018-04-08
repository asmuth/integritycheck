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

fhistory init --set_time 1451624401000000
fhistory status -v

touch -m --date='2016-01-01 10:01:02' testX

fhistory ack -y . -m "Hello World" --set_time 1451624402000000
fhistory status

fhistory log --colours=off > "../output"

(cat > "../output.expected") <<EOF
Repository: ${TEST_TMPDIR}/repo

IndexReference { timestamp_us: 1451624402000000, checksum: "815181b7643e97e926a1615e66126d1b935cbe2d82c2e59ec6f02bcba5c8e984" } Some("Hello World")
IndexReference { timestamp_us: 1451624401000000, checksum: "4e0d097ba76cd14fafdb9aa902dff19bd360b33c577ff5deb3ef368ca7db7590" } None
EOF

diff "../output" "../output.expected"

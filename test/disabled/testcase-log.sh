#!/bin/bash
# integritycheck - https://github.com/asmuth/integritycheck
# Copyright (c) 2018, Paul Asmuth <paul@asmuth.com>
#
# This file is part of the "integritycheck" project. integritycheck is free software
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

ic init --set_time 1451624401000000
ic status -v

touch -m --date='2016-01-01 10:01:02' testX

ic ack -y . -m "Hello World" --set_time 1451624402000000
ic status

ic log --colours=off > "../output"

(cat > "../output.expected") <<EOF
snapshot b2e61b7fd1934df374fd722181da93c380a72c0eeb28dd73c2a4a15f86e0710f
Timestamp: Fri, 01 Jan 2016 06:00:02 +0100
Size: 6B (4 files)

    Hello World

snapshot 514530670e5d6a33e36d9a4a1c99cebc72a6977414b298bed27e6a63a31999a8
Timestamp: Fri, 01 Jan 2016 06:00:01 +0100
Size: 6B (3 files)

    <no message>

EOF

diff "../output" "../output.expected"

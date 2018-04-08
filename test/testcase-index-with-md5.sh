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

fhistory init --checksum md5
fhistory status
fhistory verify

(cat > "../index.expected") <<EOF
#checksum md5
bf072e9119077b4e76437a93986787ef 2 1451624401000000 testA
30cf3d7d133b08543cb6c8933c29dfd7 2 1451624402000000 testB
b39bfc0e26a30024c76e4dcb8a1eae87 2 1451624403000000 testC
EOF

pigz -z -d < .fh/$(ls -t1 .fh/ | head -n 1) > "../index.actual"
diff "../index.actual"  "../index.expected"

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

fhistory init --checksum sha256 -v
fhistory status

sleep 0.01

touch -m --date='2016-01-01 06:00:03' testX
fhistory ack -y . -m "hello w"$'\n'"or\\ld!"
fhistory status

(cat > "../index.expected") <<EOF
#checksum sha256
#message hello\\_w\\nor\\\\ld!
06f961b802bc46ee168555f066d28f4f0e9afdf3f88174c1ee6f9de004fc30a0 2 1451624401000000 testA
c0cde77fa8fef97d476c10aad3d2d54fcc2f336140d073651c2dcccf1e379fd6 2 1451624402000000 testB
12f37a8a84034d3e623d726fe10e5031f4df997ac13f4d5571b5a90c41fb84fe 2 1451624403000000 testC
e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855 0 1451624403000000 testX
EOF

pigz -z -d < .fh/$(ls -t1 .fh/ | head -n 1) | grep -vE '^#timestamp' > "../index.actual"
diff "../index.actual"  "../index.expected"

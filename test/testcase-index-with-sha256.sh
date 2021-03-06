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
cd "${TEST_TMPDIR}"

echo "A" > repo/testA
echo "B" > repo/testB
echo "C" > repo/testC

touch -m --date='2016-01-01 06:00:01' repo/testA
touch -m --date='2016-01-01 06:00:02' repo/testB
touch -m --date='2016-01-01 06:00:03' repo/testC

(cd repo && ic index . > ../index.actual)

(cat > "index.expected") <<EOF
testA [sha256] 06f961b802bc46ee168555f066d28f4f0e9afdf3f88174c1ee6f9de004fc30a0 2 1451624401000000
testB [sha256] c0cde77fa8fef97d476c10aad3d2d54fcc2f336140d073651c2dcccf1e379fd6 2 1451624402000000
testC [sha256] 12f37a8a84034d3e623d726fe10e5031f4df997ac13f4d5571b5a90c41fb84fe 2 1451624403000000
EOF

diff "index.actual"  "index.expected"

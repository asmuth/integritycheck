#!/bin/bash
source test/env.sh

mkdir "files"

echo "123" > files/a
echo "xxx" > files/b
echo "789" > files/c

(cat > "index.lst") <<EOF
a8fdc205a9f19cc1c7507a60c4f01b13d11d7fd0 a
f9e21473daaa2674d862b67a1339f4570e86de17 b
cc4bba312861563053a8437e4986054961167de0 c
EOF

(
  find files -type f | index_search index.lst
) > output

(cat > "output_expected") <<EOF
hit files/c
miss files/b
hit files/a
EOF

diff "output"  "output_expected"

set -ue

TEST_TMPDIR="$(mktemp -d "/tmp/fhistory-test-XXXXXXX")"
trap "rm -rf ${TEST_TMPDIR};" EXIT

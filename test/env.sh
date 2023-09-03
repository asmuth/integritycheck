TEST_SRCDIR="$(dirname "$(realpath "$0")")"
TEST_TMPDIR="$(mktemp -d "/tmp/ic-test-XXXXXXX")"
trap "rm -rf ${TEST_TMPDIR};" EXIT
cd "${TEST_TMPDIR}"

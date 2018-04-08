TEST_SRCDIR="$(dirname "$(realpath "$0")")"
TEST_TMPDIR="$(mktemp -d "/tmp/fhistory-test-XXXXXXX")"
trap "rm -rf ${TEST_TMPDIR};" EXIT

print_yellow () {
  printf "\033[1;33m%s\033[0m" "$1"
}

print_green () {
  printf "\033[1;32m%s\033[0m" "$1"
}

print_red () {
  printf "\033[1;31m%s\033[0m" "$1"
}

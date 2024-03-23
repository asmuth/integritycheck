#!/bin/bash
set -ue -o pipefail

TEST_BINDIR="$1"
TEST_SRCDIR="$(dirname "$(realpath "$0")")"
TEST_TMPDIR="$(mktemp -d "/tmp/flix-test-XXXXXXX")"
trap "rm -rf ${TEST_TMPDIR};" EXIT

print_info () {
  printf "\033[1;34m%s\033[0m" "$1"
}

print_success () {
  printf "\033[1;32m%s\033[0m" "$1"
}

print_fail () {
  printf "\033[1;31m%s\033[0m" "$1"
}

run_test () {
  mkdir "${TEST_TMPDIR}/$1"

  (
    cd "${test_path}";
    TEST_SRCDIR="${TEST_SRCDIR}/$1" \
    TEST_TMPDIR="${TEST_TMPDIR}/$1" \
    "${TEST_BINDIR}/$1"
  ) &> "${TEST_TMPDIR}/$1.log"
}

num_total=0
num_passed=0
num_failed=0
failed=()

print_info "Running tests..."
echo

for test_path in $(find "${TEST_SRCDIR}" -maxdepth 1 -name "test-*" -type d); do
  test_name="$(basename "${test_path}")"

  echo -n " + $(echo "$test_name") "
  num_total=$[ $num_total + 1 ]

  if run_test "${test_name}"; then
    print_success "PASS"
    echo
    num_passed=$[ $num_passed + 1 ]
  else
    print_fail "FAIL"
    echo
    num_failed=$[ $num_failed + 1 ]
    failed+=("${test_name}")
  fi
done
echo

for failed in ${failed[@]}; do
  print_fail "FAIL: "
  echo "${failed}"
  sed -e 's/^/  | /' < "${TEST_TMPDIR}/${failed}.log"
  echo
done

print_info "Test Summary: "
if [[ ${num_passed} -eq ${num_total} && ${num_total} -gt 0 ]]; then
  print_success "PASS (${num_passed}/${num_total})"
  echo
  exit 0
else
  print_fail "FAIL (${num_failed}/${num_total})"
  echo
  exit 1
fi

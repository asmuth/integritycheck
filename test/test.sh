#!/bin/bash
source test/env.sh

print_info () {
  printf "\033[1;34m%s\033[0m" "$1"
}

print_success () {
  printf "\033[1;32m%s\033[0m" "$1"
}

print_fail () {
  printf "\033[1;31m%s\033[0m" "$1"
}

num_total=0
num_passed=0
num_failed=0
failed_logs=()

print_info "Running tests..."
echo

for f in $(cd "${TEST_SRCDIR}" && find . -maxdepth 1 -name "test-*.sh" -type f); do
  echo -n " + $(echo "$f" | sed -e 's/^\./test/') "
  num_total=$[ $num_total + 1 ]

  if "${TEST_SRCDIR}/$f" &> "${TEST_TMPDIR}/$f.log"; then
    print_success "PASS"
    echo
    num_passed=$[ $num_passed + 1 ]
  else
    print_fail "FAIL"
    echo
    num_failed=$[ $num_failed + 1 ]
    failed_logs+=($f.log)
  fi
done
echo

for failed in ${failed_logs[@]}; do
  print_fail "FAIL: "
  echo "${failed}"
  sed -e 's/^/  | /' < "${TEST_TMPDIR}/${failed}"
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

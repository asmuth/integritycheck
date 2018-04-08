#!/bin/bash
set -ue
source test/test-util.sh

num_total=0
num_passed=0
num_failed=0
failed_logs=()

print_yellow "Running tests..."
echo

for f in $(cd "${TEST_SRCDIR}" && find . -name "testcase-*" -type f); do
  echo -n " + $(echo "$f" | sed -e 's/^\./test/') "
  num_total=$[ $num_total + 1 ]

  if "${TEST_SRCDIR}/$f" &> "${TEST_TMPDIR}/$f.log"; then
    print_green "PASS"
    echo
    num_passed=$[ $num_passed + 1 ]
  else
    print_red "FAIL"
    echo
    num_failed=$[ $num_failed + 1 ]
    failed_logs+=($f.log)
  fi
done
echo

for failed in ${failed_logs[@]}; do
  print_red "FAIL: "
  echo "${failed}"
  sed -e 's/^/  | /' < "${TEST_TMPDIR}/${failed}"
  echo
done

print_yellow "Test Summary: "
if [[ ${num_passed} -eq ${num_total} && ${num_total} -gt 0 ]]; then
  print_green "PASS (${num_passed}/${num_total})"
  echo
  exit 0
else
  print_red "FAIL (${num_failed}/${num_total})"
  echo
  exit 1
fi

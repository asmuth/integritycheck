package assert

import (
	"bytes"
	"fmt"
	"os"
)

func Fail(err error) {
	fmt.Fprintln(os.Stderr, err)
	os.Exit(1)
}

func Expect(desc string, cond bool) {
	if !cond {
		fmt.Fprintf(os.Stderr, "EXPECT: %s\n", desc)
		os.Exit(1)
	}
}

func CompareIndexFiles(actualPath, expectedPath string) {
	expected, err := os.ReadFile(expectedPath)
	if err != nil {
		Fail(err)
	}

	actual, err := os.ReadFile(actualPath)
	if err != nil {
		Fail(err)
	}

	if !bytes.Equal(expected, actual) {
		Fail(fmt.Errorf(
			"index files do not match\n--- expected ---\n%s--- actual ---\n%s",
			expected,
			actual,
		))
	}
}

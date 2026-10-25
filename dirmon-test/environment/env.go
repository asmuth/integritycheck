package environment

import (
	"os"
	"path/filepath"
)

func TempPath(path string) string {
	tempDir := os.Getenv("TEST_TMPDIR")
	if tempDir == "" {
		panic("TEST_TMPDIR is not set")
	}

	return filepath.Join(tempDir, path)
}

func TestPath(path string) string {
	testDir := os.Getenv("TEST_SRCDIR")
	if testDir == "" {
		panic("TEST_SRCDIR is not set")
	}

	return filepath.Join(testDir, path)
}

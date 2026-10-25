package main

import (
	"dirmon-test/assert"
	"dirmon-test/environment"
	"dirmon-test/generate"
	"dirmon-test/shell"
)

func main() {
	generate.CreateDirectory("root")
	generate.CreateFile("root/file1.bin", []byte("fnord"))
	generate.CreateFile("root/file2.bin", []byte("blah"))

	indexPath := environment.TempPath("index.json")
	dataPath := environment.TempPath("root")

	result := shell.RunCommand(
		"scan",
		"-i", indexPath,
		"-d", dataPath,
		"-P",
	)

	assert.Expect("exit code 0", result.ExitCode == 0)
	assert.Expect("stdout empty", result.Stdout == "")
	assert.Expect("stderr empty", result.Stderr == "")
	assert.CompareIndexFiles(environment.TestPath("index.json"), indexPath)
}

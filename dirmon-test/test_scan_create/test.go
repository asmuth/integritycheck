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
	generate.CreateDirectory("root/a")
	generate.CreateFile("root/a/file with whitespace.txt", []byte("blah"))
	generate.CreateDirectory("root/exce\\\\ent\n🔥file\\\n\\ name")
	generate.CreateGeneratedFile("root/exce\\\\ent\n🔥file\\\n\\ name/zero", 100_000, generate.GenerateZeroes())

	indexPath := environment.TempPath("index.json")
	dataPath := environment.TempPath("root")

	result := shell.RunCommand(
		"scan",
		"--index", indexPath,
		"--data", dataPath,
		"--noprogress",
	)

	assert.Expect("exit code 0", result.ExitCode == 0)
	assert.Expect("stdout empty", result.Stdout == "")
	assert.Expect("stderr empty", result.Stderr == "")
	assert.CompareIndexFiles(environment.TestPath("index.json"), indexPath)
}

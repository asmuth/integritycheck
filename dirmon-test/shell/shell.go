package shell

import (
	"bytes"
	"errors"
	"os"
	"os/exec"
)

type CommandResult struct {
	ExitCode int
	Stdout   string
	Stderr   string
}

func RunCommand(args ...string) CommandResult {
	progPath := os.Getenv("TEST_PROG")
	if progPath == "" {
		panic("TEST_PROG is not set")
	}

	command := exec.Command(progPath, args...)

	var stdout bytes.Buffer
	var stderr bytes.Buffer
	command.Stdout = &stdout
	command.Stderr = &stderr

	err := command.Run()
	exitCode := 0
	if err != nil {
		var exitError *exec.ExitError
		if !errors.As(err, &exitError) {
			panic(err)
		}
		exitCode = exitError.ExitCode()
	}

	return CommandResult{
		ExitCode: exitCode,
		Stdout:   stdout.String(),
		Stderr:   stderr.String(),
	}
}

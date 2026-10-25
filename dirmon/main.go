package main

import (
	"dirmon/scan"
	"fmt"
	"os"
)

const MainHelpText = "" +
	"Usage: $ dirmon <command> [<option>]\n" +
	"\n" +
	"Commands:\n" +
	"    scan       Create or update an index file\n"

func main() {
	if len(os.Args) < 2 {
		fmt.Print(MainHelpText)
		os.Exit(1)
	}

	var err error
	switch os.Args[1] {
	case "scan":
		err = scan.ScanCommand()
	default:
		fmt.Println("Invalid command")
		os.Exit(1)
	}

	if err != nil {
		fmt.Fprintf(os.Stderr, "ERROR: %v\n", err)
		os.Exit(1)
	}
}

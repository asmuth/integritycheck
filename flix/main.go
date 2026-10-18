package main

import (
	"flix/commands"
	"fmt"
	"os"
)

const ScanHelpText = "" +
	"Usage: $ flix scan [<option>]\n" +
	"   -i, --index=<path>            Index file path\n" +
	"   -d, --data=<path>             Data directory path (default: '.')\n" +
	"   -?, --help                    Display this help text and exit\n" +
	"   -V, --version                 Display the version of this program and exit\n" +
	"\n" +
	"Output format:\n" +
	"   -p, --progress                Enable progress output to STDERR\n" +
	"   -P, --noprogress              Disable progress output to STDERR\n" +
	"\n" +
	"Examples:\n" +
	"   $ filecheck -i index.lst\n" +
	"   $ filecheck -i index.lst -D path/to/files\n" +
	"   $ filecheck -i index.lst -u\n" +
	"   $ filecheck -i index.lst -s file1 file2\n" +
	"   $ filecheck -i index.lst -s - < file_list.txt\n"

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage: $ flix <command> [<option>]")
		fmt.Println("")
		fmt.Println("Commands:")
		fmt.Println("    scan       Create or update an index file")
		os.Exit(1)
	}

	var err error
	switch os.Args[1] {
	case "scan":
		err = commands.Scan()
	default:
		fmt.Println("Invalid command")
		os.Exit(1)
	}

	if err != nil {
		fmt.Fprintf(os.Stderr, "ERROR: %v\n", err)
		os.Exit(1)
	}
}

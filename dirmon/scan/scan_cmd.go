package scan

import (
	"dirmon/progress"
	"dirmon/tty"
	"flag"
	"fmt"
	"os"
)

const ScanHelpText = "" +
	"Usage: $ dirmon scan [<option>]\n" +
	"   -i, --index=<path>            Index file path\n" +
	"   -d, --data=<path>             Data directory path (default: '.')\n" +
	"   -h, --help                    Display this help text and exit\n" +
	"   -V, --version                 Display the version of this program and exit\n" +
	"\n" +
	"Output format:\n" +
	"   -p, --progress                Enable progress output to STDERR\n" +
	"   -P, --noprogress              Disable progress output to STDERR\n" +
	"\n" +
	"Examples:\n" +
	"   $ dirmon scan -i index.json -D path/to/files\n" +
	"   $ dirmon scan -i path/to/index.json\n"


func ScanCommand() error {
	op := ScanOp{}

	flagHelp := false
	flagProgress := true
	flagNoProgress := false
	flags := flag.NewFlagSet("scan", flag.ExitOnError)
	flags.Usage = func() { fmt.Print(ScanHelpText) }
	flags.StringVar(&op.indexPath, "i", "", "index")
	flags.StringVar(&op.indexPath, "index", "", "index")
	flags.StringVar(&op.dataPath, "d", "", "data")
	flags.StringVar(&op.dataPath, "data", "", "data")
	flags.BoolVar(&flagHelp, "h", false, "help")
	flags.BoolVar(&flagHelp, "?", false, "help")
	flags.BoolVar(&flagProgress, "p", true, "progress")
	flags.BoolVar(&flagProgress, "progress", true, "progress")
	flags.BoolVar(&flagNoProgress, "P", false, "noprogress")
	flags.BoolVar(&flagNoProgress, "noprogress", false, "noprogress")

	flags.Parse(os.Args[2:])

	if flagHelp {
		flags.Usage()
		os.Exit(0)
	}

	if op.indexPath == "" {
		fmt.Fprintln(os.Stderr, "ERROR: need an index path (--index)")
		os.Exit(0)
	}

	if flagProgress && !flagNoProgress {
		op.progress = func(p progress.Progress) {
			tty.PrintProgress(p)
		}
	}

	err := Scan(op)
	if err != nil {
		return err
	}

	return nil
}

package scan

import (
	"flag"
	"dirmon/progress"
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
	flags := flag.NewFlagSet("scan", flag.ExitOnError)
	flags.Usage = func() { fmt.Print(ScanHelpText) }
	flags.StringVar(&op.indexPath, "i", "", "index")
	flags.StringVar(&op.indexPath, "index", "", "index")
	flags.StringVar(&op.dataPath, "d", "", "data")
	flags.StringVar(&op.dataPath, "data", "", "data")
	flags.BoolVar(&flagHelp, "h", false, "help")
	flags.BoolVar(&flagHelp, "?", false, "help")
	flags.Parse(os.Args[2:])

	if (flagHelp) {
		flags.Usage()
		os.Exit(0)
	}

	fmt.Printf("%+v\n", op)
	if (op.indexPath == "") {
		fmt.Fprintln(os.Stderr, "ERROR: need an index path (--index)")
		os.Exit(0)
	}

	op.progress = func(p progress.Progress) {
		fmt.Printf("%+v\n", p)
	}

	idx, err := Scan(op)
	if err != nil {
		return err
	}

	fmt.Println(idx)
	return nil
}

package commands

import (
	"flag"
	"flix/scan"
	"fmt"
	"os"
)

const ScanHelpText = "" +
	"Usage: $ flix scan [<option>]\n" +
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
	"   $ flix scan -i index.json -D path/to/files\n" +
	"   $ flix scan -i path/to/index.json\n"

type ScanOptions struct {
  indexPath string
  dataPath string
  help bool
}

func Scan() error {
	options := ScanOptions{}

  flags := flag.NewFlagSet("scan", flag.ExitOnError)
	flags.Usage = func() { fmt.Print(ScanHelpText) }
	flags.StringVar(&options.indexPath, "i", "", "index")
	flags.StringVar(&options.indexPath, "index", "", "index")
	flags.StringVar(&options.dataPath, "d", "", "data")
	flags.StringVar(&options.dataPath, "data", "", "data")
	flags.BoolVar(&options.help, "h", false, "help")
	flags.BoolVar(&options.help, "?", false, "help")
	flags.Parse(os.Args[2:])

  if (options.help) {
    flags.Usage()
    os.Exit(0)
  }

  fmt.Printf("%+v\n", options)
  if (options.indexPath == "") {
    fmt.Fprintln(os.Stderr, "ERROR: need an index path (--index)")
    os.Exit(0)
  }

	idx, err := scan.Scan(options.dataPath)
	if err != nil {
		return err
	}

	fmt.Println(idx)
	return nil
}

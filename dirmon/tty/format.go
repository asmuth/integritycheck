package tty

import (
	"fmt"
)

func PrintSuccess(s string) string {
	return fmt.Sprintf("\033[1;42m%s\033[0m", s)
}

func PrintWarning(s string) string {
	return fmt.Sprintf("\033[1;43m%s\033[0m", s)
}

func PrintError(s string) string {
	return fmt.Sprintf("\033[1;41m%s\033[0m", s)
}

func PrintValueBytes(x uint64) string {
	switch {
	case x < 1_000:
		return fmt.Sprintf("%dB", x)
	case x < 1_000_000:
		return fmt.Sprintf("%.3fKB", float64(x)/1_000)
	case x < 1_000_000_000:
		return fmt.Sprintf("%.3fMB", float64(x)/1_000_000)
	case x < 1_000_000_000_000:
		return fmt.Sprintf("%.3fGB", float64(x)/1_000_000_000)
	default:
		return fmt.Sprintf("%.3fTB", float64(x)/1_000_000_000_000)
	}
}

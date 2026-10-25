package tty

import (
	"dirmon/progress"
	"fmt"
	"os"
	"time"
)

func PrintProgress(p progress.Progress) {
	percent := float64(0)
	if p.BytesTotal > 0 {
		percent = float64(p.Bytes) / float64(p.BytesTotal) * 100
	}

	fmt.Fprintf(
		os.Stderr,
		"[%s] %.2f%%, %s/%s, %d/%d files\n",
		time.Now().Format("2006-01-02 15:04:05 -0700"),
		percent,
		PrintValueBytes(p.Bytes),
		PrintValueBytes(p.BytesTotal),
		p.Files,
		p.FilesTotal,
	)
}

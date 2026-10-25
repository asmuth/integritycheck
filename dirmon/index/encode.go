package index

import (
	"encoding/json"
	"io"
	"os"
)

func writeHeader(output io.Writer) error {
	if _, err := io.WriteString(output, "[\n"); err == nil {
		return nil
	} else {
		return err
	}
}

func writeFooter(output io.Writer) error {
	if _, err := io.WriteString(output, "]\n"); err == nil {
		return nil
	} else {
		return err
	}
}

func writeRow(output io.Writer, row []any, last bool) error {
	if _, err := io.WriteString(output, "  ["); err != nil {
		return err
	}

	for i, v := range row {
		if i > 0 {
			if _, err := io.WriteString(output, ", "); err != nil {
				return err
			}
		}

		b, err := json.Marshal(v)
		if err != nil {
			return err
		}

		if _, err := output.Write(b); err != nil {
			return err
		}
	}

	if last {
		if _, err := io.WriteString(output, "]\n"); err != nil {
			return err
		}
	} else {
		if _, err := io.WriteString(output, "],\n"); err != nil {
			return err
		}
	}

	return nil
}

func writePreamble(output io.Writer, idx *Index) error {
	data := []any{"path", "size", idx.ChecksumType}

	if err := writeRow(output, data, len(idx.Entries) == 0); err != nil {
		return err
	}

	return nil
}

func writeRecords(output io.Writer, idx *Index) error {
	for i, rec := range idx.Entries {
		recData := []any{rec.Path, rec.Size, rec.Checksum}
		if err := writeRow(output, recData, i == len(idx.Entries) - 1); err != nil {
			return err
		}
	}

	return nil
}

func (idx *Index) Write(output io.Writer) error {
	if err := writeHeader(output); err != nil {
		return err
	}

	if err := writePreamble(output, idx); err != nil {
		return err
	}

	if err := writeRecords(output, idx); err != nil {
		return err
	}

	if err := writeFooter(output); err != nil {
		return err
	}

	return nil
}

func (idx *Index) WriteFile(path string) error {
	file, err := os.OpenFile(path, os.O_CREATE | os.O_EXCL | os.O_WRONLY, 0666)
	if err != nil {
		return err
	}

	if err := idx.Write(file); err != nil {
		file.Close()
		return err
	}

	if err := file.Close(); err != nil {
		return err
	}

	return nil
}

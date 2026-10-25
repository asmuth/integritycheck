package scan

import (
	"path"
	"path/filepath"
	"io/fs"
	"dirmon/index"
	"fmt"
)

func listFiles(dataPath string) (index.Index, error) {
	idx := index.Index{}

	err := filepath.Walk(dataPath, func(filePath string, fileInfo fs.FileInfo, err error) error {
		if err != nil {
			return fmt.Errorf("while scanning %q: %w", filePath, err)
			return err
		}

		if !fileInfo.Mode().IsRegular() {
			return nil
		}

		filePathRel, err := filepath.Rel(dataPath, filePath)
		if err != nil {
			return fmt.Errorf("while scanning %q: %w", filePath, err)
		}

		idx.AddEntry(index.IndexRecord {
			Path: filePathRel,
			Size: fileInfo.Size(),
		})

		return nil
	})

	if err != nil {
		return index.Index{}, err
	}

	return idx, nil
}

func computeChecksum(dataPath string, rec *index.IndexRecord) error {
	filePath := path.Join(dataPath, rec.Path)
	fileChecksum, err := index.ComputeMD5(filePath)
	if err != nil {
		return fmt.Errorf("while scanning %q: %w", filePath, err)
	}

	rec.Checksum = fileChecksum

	return nil
}

func computeChecksums(dataPath string, idx *index.Index) error {
	for i := range idx.Entries {
		if err := computeChecksum(dataPath, &idx.Entries[i]); err != nil {
			return err
		}
	}

	return nil
}

func Scan(dataPath string) (index.Index, error) {
	idx, err := listFiles(dataPath)
	if err != nil {
		return index.Index{}, err
	}

	err = computeChecksums(dataPath, &idx)
	if err != nil {
		return index.Index{}, err
	}

	return idx, nil
}

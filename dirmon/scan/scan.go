package scan

import (
	"path"
	"path/filepath"
	"io/fs"
	"dirmon/index"
	"dirmon/progress"
	"fmt"
)

type ScanOp struct {
	indexPath string
	dataPath string
	progress progress.ProgressFunc
}

func listFiles(op ScanOp, progress *progress.Progress) (index.Index, error) {
	idx := index.Index{ChecksumType: "md5"}

	err := filepath.Walk(op.dataPath, func(filePath string, fileInfo fs.FileInfo, err error) error {
		if err != nil {
			return fmt.Errorf("while scanning %q: %w", filePath, err)
			return err
		}

		if !fileInfo.Mode().IsRegular() {
			return nil
		}

		filePathRel, err := filepath.Rel(op.dataPath, filePath)
		if err != nil {
			return fmt.Errorf("while scanning %q: %w", filePath, err)
		}

		fileSize := fileInfo.Size()

		idx.AddEntry(index.IndexRecord {
			Path: filePathRel,
			Size: fileSize,
		})

		progress.FilesTotal += 1
		progress.BytesTotal += uint64(fileSize)

		if op.progress != nil {
			op.progress(*progress)
		}

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

func computeChecksums(op ScanOp, idx *index.Index, progress *progress.Progress) error {
	for i := range idx.Entries {
		if err := computeChecksum(op.dataPath, &idx.Entries[i]); err != nil {
			return err
		}

		progress.Files += 1
		progress.Bytes += uint64(idx.Entries[i].Size)

		if op.progress != nil {
			op.progress(*progress)
		}
	}

	return nil
}

func Scan(op ScanOp) (error) {
	progress := progress.Progress {}

	idx, err := listFiles(op, &progress)
	if err != nil {
		return err
	}

	if err := computeChecksums(op, &idx, &progress); err != nil {
		return err
	}

	idx.Sort()

	if err := idx.WriteFile(op.indexPath); err != nil {
		return err
	}

	return nil
}

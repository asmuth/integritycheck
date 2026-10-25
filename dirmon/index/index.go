package index

import (
	"sort"
)

type Index struct {
	ChecksumType string
	Entries      []IndexRecord
}

type IndexRecord struct {
	// The relative path of the file
	Path string
	// The size of the file in bytes
	Size int64
	// The checksum of the file
	Checksum string
}

func (idx *Index) AddEntry(rec IndexRecord) {
	idx.Entries = append(idx.Entries, rec)
}

func (idx *Index) Sort() {
	sort.Slice(idx.Entries, func(i, j int) bool {
		a := idx.Entries[i]
		b := idx.Entries[j]
		return b.Path > a.Path
	})
}

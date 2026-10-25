package index

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


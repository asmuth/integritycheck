package progress

type Progress struct {
	Files uint64
	FilesTotal uint64
	Bytes uint64
	BytesTotal uint64
}

type ProgressFunc func(Progress)

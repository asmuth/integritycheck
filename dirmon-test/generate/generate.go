package generate

import (
	"dirmon-test/environment"
	"os"
)

type DataGenerator func(int) byte

func CreateFile(path string, data []byte) {
	if err := os.WriteFile(environment.TempPath(path), data, 0o666); err != nil {
		panic(err)
	}
}

func CreateGeneratedFile(path string, size int, generate DataGenerator) {
	data := make([]byte, size)
	for i := range data {
		data[i] = generate(i)
	}

	CreateFile(path, data)
}

func CreateDirectory(path string) {
	if err := os.Mkdir(environment.TempPath(path), 0o777); err != nil {
		panic(err)
	}
}

func GenerateZeroes() DataGenerator {
	return func(int) byte { return 0 }
}

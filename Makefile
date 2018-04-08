build:
	cargo build

install:
	cargo install --root /usr/local

test:
	@PATH=$(realpath ./target/debug):$(PATH) ./test/test-runner.sh

.PHONY: build install test

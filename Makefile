build:
	cargo build

install:
	cargo install --root /usr/local --force

test:
	@PATH=$(realpath ./target/debug):$(PATH) ./test/test-runner.sh

.PHONY: build install test

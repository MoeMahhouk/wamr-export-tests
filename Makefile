C := /opt/wasi-sdk-12.0/bin/clang
CFLAGS := --sysroot=/opt/wasi-sdk-12.0/share/wasi-sysroot -Wl,--no-entry -O3

.PHONY: test

test: test.wasm

clean:
	rm -f test.wasm
%.wasm: %.c
	$(C) $(CFLAGS) -o $@ $<

C := ../wasi-sdk/bin/clang
#CFLAGS := --sysroot=../wasi-sdk-12.0/share/wasi-sysroot -Wl,--no-entry,--allow-undefined,--export-all -O3
CFLAGS := --sysroot=../wasi-sdk/share/wasi-sysroot -Wl,--no-entry -O3

.PHONY: test add multadd key_value all

add: add.wasm
multadd: multadd.wasm
test: test.wasm
key_value: key_value.wasm

all: test add multadd key_value

clean:
	rm -f *.wasm
%.wasm: %.c
	$(C) $(CFLAGS) -o $@ $<

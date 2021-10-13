C := ./wasi-sdk/bin/clang
#CFLAGS := --sysroot=/opt/wasi-sdk-12.0/share/wasi-sysroot -Wl,--no-entry,--allow-undefined,--export-all -O3
CFLAGS := --target=wasm32 -nostdlib --sysroot=/opt/wasi-sdk-12.0/share/wasi-sysroot -Wl,--no-entry -O3

.PHONY: test add multadd all

add: add.wasm
multadd: multadd.wasm
test: test.wasm

all: test add multadd

clean:
	rm -f *.wasm
%.wasm: %.c
	$(C) $(CFLAGS) -o $@ $<

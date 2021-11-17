BENCH_ITERATIONS?=1000

C := ../wasi-sdk/bin/clang
#CFLAGS := --sysroot=../wasi-sdk-12.0/share/wasi-sysroot -Wl,--no-entry,--allow-undefined,--export-all -O3
CFLAGS := --sysroot=../wasi-sdk/share/wasi-sysroot -Wl,--no-entry -O3 -DBENCH_ITERATIONS=$(BENCH_ITERATIONS)

.PHONY: test add multadd key_value key_value_bench kv_shared_mem all

add: add.wasm
multadd: multadd.wasm
test: test.wasm
key_value: key_value.wasm
key_value_bench: key_value_bench.wasm
kv_shared_mem: kv_shared_mem.wasm

all: test add multadd key_value key_value_bench kv_shared_mem

clean:
	rm -f *.wasm
%.wasm: %.c
	$(C) $(CFLAGS) -o $@ $< 
	@echo "The benchmark iterations are set to $(BENCH_ITERATIONS)!"
BIN="cpu_frequency"

all:
	@bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h
	@clang -g -O3 -target bpf -D__TARGET_ARCH_x86 -c $(BIN).bpf.c -o $(BIN).bpf.o
	@bpftool gen skeleton $(BIN).bpf.o > $(BIN).skel.h
	@clang $(BIN).c -lbpf -lelf -o $(BIN)

.PHONY: clean
clean:
	@rm -rf *.o *.skel.h vmlinux.h $(BIN)

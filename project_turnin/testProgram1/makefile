.PHONY: clean
CLANG=../llvm_build/bin/clang
MEMLOG_PASS=../llvm_build/bin/opt -load ../llvm_build/lib/LLVMMemlog.so -memlog
LLVM_LINK=../llvm_build/bin/llvm-link
LLI=../llvm_build/bin/lli

all: main.bc anotherFile.bc
	$(LLVM_LINK) main.bc anotherFile.bc -o test1.bc
	$(LLI) test1.bc

clean:
	rm -f *.bc memlog.txt

main.bc: main.c
	$(CLANG) -emit-llvm -c main.c -o main_tmp.bc
	$(MEMLOG_PASS) main_tmp.bc -o main.bc
	rm -f main_tmp.bc

anotherFile.bc: anotherFile.c
	$(CLANG) -emit-llvm -c anotherFile.c -o anotherFile_tmp.bc
	$(MEMLOG_PASS) anotherFile_tmp.bc -o anotherFile.bc
	rm -f anotherFile_tmp.bc

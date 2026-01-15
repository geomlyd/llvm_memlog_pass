.PHONY: clean
CLANG=../llvm_build/bin/clang
MEMLOG_PASS=../llvm_build/bin/opt -load ../llvm_build/lib/LLVMMemlog.so -memlog
LLVM_LINK=../llvm_build/bin/llvm-link
LLI=../llvm_build/bin/lli

all: testsymtab.bc symtablehash.bc
	$(LLVM_LINK) testsymtab.bc symtablehash.bc -o hash.bc
	$(LLI) hash.bc

clean:
	rm -f *.bc memlog.txt

testsymtab.bc: testsymtab.c symtable.h
	$(CLANG) -emit-llvm -c testsymtab.c -o testsymtab_tmp.bc
	$(MEMLOG_PASS) testsymtab_tmp.bc -o testsymtab.bc
	rm -f testsymtab_tmp.bc

symtablehash.bc: symtablehash.c symtable.h
	$(CLANG) -emit-llvm -c symtablehash.c -o symtablehash_tmp.bc
	$(MEMLOG_PASS) symtablehash_tmp.bc -o symtablehash.bc
	rm -f symtablehash_tmp.bc

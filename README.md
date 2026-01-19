# LLVM Memory Logging Pass

Tracks all `malloc`/`free` calls in C programs, and logs number of bytes and source lines to a log file.
Tested on small demos and on a hash table workload generating ~15,000 randomized allocation/free events.
The showcased functionality could serve as a building block for memory debugging tools like valgrind.
Developed as part of the graduate course ["Type systems and programming languages"](https://www.csd.uoc.gr/~hy546/) (Department of Computer Science, University of Crete).

## Features

- Custom LLVM IR pass for tracking allocations.
- Handles multi-file C programs.
- Generates a log file (`memlog.txt`) with details on memory sizes allocated/freed, along with corresponding source lines.

## Setup

1. Code was developed with LLVM 3.2 and clang 3.2, building today may require a legacy environment.
2. Download suitable versions of LLVM and clang sources, place clang source in LLVM's `tools` directory.
2. Copy `MemlogPass` into `llvm_src/lib/Transforms/`.
3. Build LLVM with `cmake`/`make`.
4. Run tests in `testProgram1/2` directories (`make all`).
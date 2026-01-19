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

# Set-up instructions


1. `cd` to the top-level directory containing this README file
2. Execute the following in order to set up LLVM and clang with the versions used in this project:
```
wget https://releases.llvm.org/3.2/llvm-3.2.src.tar.gz
tar xvzf llvm-3.2.src.tar.gz
mv llvm-3.2.src llvm_src
wget https://releases.llvm.org/3.2/clang-3.2.src.tar.gz
tar xvzf clang-3.2.src.tar.gz
mv clang-3.2.src clang
```
3. Execute the following to set up our instance of LLVM:
```
mv clang llvm_src/tools/clang
mkdir llvm_src/lib/Transforms/MemlogPass
cp -r ./MemlogPass/* ./llvm_src/lib/Transforms/MemlogPass
cp ./CMakeLists.txt llvm_src/lib/Transforms/CMakeLists.txt
```
4. Due to the rather old versions of the tools used, it is necessary to activate a version 2 Python before continuing.
Inside the top-level directory containing this README, execute the following (expect the final `make` to take some time):
```
mkdir llvm_build
cd llvm_build
cmake -G "Unix Makefiles" ../llvm_src
make
```
5. At this point you should have built LLVM.
6. Inside each test directory, `testProgram1`, `testProgram2` is a makefile, run `make all` inside any of the directories to run the corresponding test.
10. Running should produce a file named `memlog.txt` in each directory
11. `testProgram1` is a simple example that demonstrates the results of the pass, with a few mallocs and frees across different functions and .c files. 
It also prints some output to stdout that can verify what is written inside memlog.txt
12. `testProgram2` is a more complicated test that constructs a hash table and performs several insertions, deletions and lookups on it. 
When prompted for input, input `2` to run a randomized test of these functionalities.
A `memlog.txt` file is once again produced, containing about 15,000 lines of mallocs and frees.
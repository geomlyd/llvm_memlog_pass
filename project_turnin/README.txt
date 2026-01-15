HY-546 project instructions

1. Create a directory, e.g. geolyd_project
2. cd to the directory created
3. Download LLVM 3.2 source by executing:

 wget https://releases.llvm.org/3.2/llvm-3.2.src.tar.gz

(inside the same directory)

4. Untar (via tar xvzf), rename resulting directory to "llvm_src"
5. Download Clang 3.2 source by executing:

 wget https://releases.llvm.org/3.2/clang-3.2.src.tar.gz 

(inside the same directory)
 
6. Untar (via tar xvzf), rename resulting directory to "clang"
7. mv clang llvm_src/tools/clang (move clang source inside llvm source tools)
8. mkdir llvm_src/lib/Transforms/MemlogPass
9. Move the contents of the "MemlogPass" directory I've turned in to the dir created in (8)
10. Replace "CMakeLists.txt" in the directory llvm_src/lib/Transforms with the
CMakeLists.txt I've turned in (the top directory one, in the same dir as the report etc.)
11. cd to the directory "geolyd_project" created in (1)
12. mkdir llvm_build
13. cd llvm_build
14. cmake -G "Unix Makefiles" ../llvm_src
15. make
16. At this point you should have built LLVM (step 15 is the most time-consuming)
17. Place folders "testProgram1", "testProgram2" inside "geolyd_project"
18. Inside each one of these is a makefile, run "make all" to run each test
19. Running should produce memlog.txt in each directory
20. "testProgram1" is a simple example that demonstrates the results of the pass,
with a few mallocs and frees across different functions and .c files. It also prints
some output to stdout that can "verify" what is written inside memlog.txt
21. "testProgram2" is an old exercise of mine from HY-255. When prompted for input,
input "2", and a number of random string insertions in a hash table will be run.
A memlog.txt file is once again produced, with a size of about 15.000 lines of
mallocs and frees.
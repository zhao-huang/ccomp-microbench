This is a set of small benchmarks for testing quality of code produced by
C compilers. In particular the benchmarks are designed to test for specific
optimisation capabilities.

These are micro-benchmarks and so do not necessarily give an overall view
of compiler quality, but it can be interesting to see how different compilers
deal with the same input. Therefore, the result times of running the
output is arguably less interesting than the actual assembly code. Note
that the tests are designed to check for specific optimisations and amplify
the performance effect of the tested optimisation; therefore the runtime
of one compiler output versus another compiler output can be very different,
although in typical real-world programs it's unlikely to ever see such a
performance variation for this kind of simple optimisation.

This is all in a somewhat rough state. There is little infrastructure, although
a contributed bash script to compile and run the benchmarks with different
compilers is provided (requires GNU time). Alternatively, you can individually
compile and run the benchmarks with the different compilers you are interested
in, and time the result.

Each directory bm1..bmXX contains one benchmark test.

Typically, there is a "driver" program (driver.c) and an additional module
which is the benchmark itself (bm1.c, bm2.c, etc). The reason for having
two separate files is to prevent whole-program optimization that would skew
the results. To get a proper result you should:

 - compile the driver.c file *once* with any compiler, to produce a
   "driver.o" object file
 - compile the benchmark module (bmX.c) with each compiler you want to test
   and link the result with the driver.o file. eg:

	gcc -O3 -fomit-frame-pointer -std=c99 driver.o bm1.c

 - run and time the result in each case:
	time ./a.out

 * Just to be clear, to compare two compilers, you should *not* recompile
   driver.c with the second compiler. The performance of the driver should
   not be measured. Link-time optimisation should not be used.
 * Note that the benchmarks are written in the C99 dialect.
 * Further information about individual benchmarks is usually in the
   comments in the bmX.c file
 * If anyone improves the infrastructure or comes up with interesting new
   test programs, I would appreciate being provided with patches.

bm1 just tests that a switch…case statement with values outside the range of the switch’d datatype are optimized away. gcc 3.4.6 doesn’t do this, which is why it compares badly against gcc 4.3.3 and 4.4.0 (which do).

bm2 tests alias analysis of the compiler. It has a function which takes an argument pointer to a certain large structure s1, but then allocates space for a slightly smaller structure s2 on the stack (as a local variable). It copies memory (using memcpy) from s1 to s2, then makes a modification of one field of s2, before finally copying s2 back to s1. This could be optimized by just changing the field directly in s1 without doing the two memory copy operations; however, none of the tested compilers manage to do this.

bm3 is like bm2, but the s2 structure is much smaller. Most compilers then seem to be able to perform decent optimization; gcc 4.3.3 notably fails.

bm4 is a simple memory copy written using a while loop with two char pointers. It’s amazing how much difference there is between the different compilers. Notably, a simple hand-coded assembly “rep movsb” does way, way better than any of the compilers (0.036 seconds). [Edit 2012/2/29: That just can’t be right. Need to re-check].

bm5 is a small testcase taken from a gcc PR. It contains branches and potential for common subexpression elimination; it probably puts some pressure on the register allocator. The regression affects gcc 4.3/4.4 series which is reflected in the benchmark results (gcc 3.4.6 really shines).

bm6 is another gcc PR. It’s essentially testing common subexpression elimination in combination with loop invariant hoisting. gcc-3.4.6 does shockingly badly, but gcc-4.3.3 and gcc-4.4.0 still get whipped by llvm-2.5.

bm7 calls a function which returns a (large) structure and passes the result straight into another function. The ABI allows for returning the structure directly into the stack location from which it is then used as a parameter, thus avoiding the need to copy the structure; however, not all compilers manage this.

bm8 tests the compiler’s ability to remove trivial loops (loops with empty bodies, but a large number of iterations). The loops are implemented with “goto” rather than a C loop structure, and there is an inner and outer loop. No tested compiler manages to  remove the loops.

bm9 tests the compiler’s ability to break up a loop into two separate loops, when doing so avoids an efficiency problem caused by a potential aliasing pointer. The function in the benchmark takes two arguments: an int array and an int pointer “p”. The inner part of the loop uses the value pointed at by “p” as part of an expression assigned to each array element in turn. Because “p” might alias one of the array elements, the stored value must be re-loaded each iteration, unless the compiler breaks the loop into two. None of the tested compilers perform this optimization.

bm10 tests that a shift-left of a value inside a large loop can be reduced to a constant (due to having shifted all the bits out). None of the tested compilers manage this.

Davin McCall - davmac@davmac.org

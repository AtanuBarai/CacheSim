# CacheSim
Simple Trace-driven Cache Simulator in C.

It is a implementation of cache described in http://www.cs.umd.edu/class/spring2012/cmsc411/p1/

Code skeleton taken from http://www.cs.umd.edu/class/spring2012/cmsc411/p1/

The type of simulator is known as a trace-driven simulator because it takes as input a memory references trace.
The trace, which is also uploaded here, was acquired on another machine.
In this implementation, the memory reference events specified in the trace(s) is used by the simulator 
to drive the movement of data into and out of the cache, thus simulating its behavior. 

Your cache simulator is configurable based on arguments given at the command line, and supports the following functionality:

  * Total cache size
  * Block size
  * Unified vs. split I- and D-caches
  * Associativity
  * Write back vs. write through
  * Write allocate vs. write no allocate


Files
There are five program files in total, as indicated below which lists the file names and a short description of their contents.

  Makefile   Builds the simulator.
  main.c     Top-level routines for driving the simulator.
  main.h     Header file for main.c.
  cache.c    Cache model.
  cache.h   Header file for cache.c.
  
The “Makefile” is a UNIX make file. Try typing make in the local directory where you’ve
copied the files. This will build the simulator from the program files that have been provided, and
will produce an executable called “sim.”

main.c contains the top-level driver for the simulator. It has a front-end routine called parse args() that parses command 
line arguments to allow configuring the cache model with all the different parameters specified earlier. To see a list of 
valid command line arguments, try typing sim -h (after compiling the template files). For each trace element read, play 
trace() calls the cache model, via the routine perform access(), to simulate a single memory reference to the cache.

cache.c contains the cache model itself. There are three routines in this file which you
should be able to use without modification. set cache param() is the cache model interface to the
argument parsing routine in main.c. It intercepts all the parameter requests and sets the proper
parameter values which have been declared as static globals in cache.c. delete and insert
are deletion and insertion routines for a doubly linked list data structure, which we will explain
below. dump settings() prints the cache configuration based on the configured parameters, and
print stats() prints the statistics that you will gather. In addition to these five routines, there
are three template functions which you will have to write. init cache() is called once to build
and initialize the cache model data structures. perform access() is called once for each iteration
of the simulator loop to simulate a single memory reference to the cache. And flush() is called at
the very end of the simulation to purge the cache of its contents. Note that the simulation is not
finished until all dirty cache lines (if there are any) are flushed out of the cache, and all statistics
are updated as a result of such flushes.

main.h is self-explanatory. cache.h contains several constants for initializing and changing
the cache configuration, and contains the data structures used to implement the cache model (we
will explain these in the next section). Finally, cache.h also contains a macro for computing the
base-2 logarithm, called LOG2, which should become useful as you build the cache model.
In addition to the five program files, there are also three trace files that you will use to drive
your simulator. Their names are “spice.trace,” “cc.trace,” and “tex.trace.” These files are the result
of tracing the memory reference behavior of the spice circuit simulator, a C compiler, and the TeX
text formatting program, respectively. They represent roughly 1 million memory references each.
The trace files are in ASCII format, so they are in human-readable form. Each line in the
trace file represents a single memory reference and contains two numbers: a reference type, which is
a number between 0–2, and a memory address. All other text following these two numbers should
be ignored by your simulator. The reference number specifies what type of memory reference is
being performed with the following encoding:
0 Data load reference
1 Data store reference
2 Instruction load reference

The number following the reference type is the byte address of the memory reference itself. This
number is in hexadecimal format, and specifies a 32-bit byte address in the range 0-0xffffffff,
inclusive.

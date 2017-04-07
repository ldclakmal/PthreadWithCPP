/* File: main.cpp
 *
 * Purpose:
 *    Use condition wait barriers to synchronize threads.
 *
 * Compile:
 *    g++ main.cpp -o main.out -pthread
 *    timer.h must be available
 *
 * Usage:
 *    main.out
 *
 * Input:
 *    Program type, No of threads
 * Output:
 *    Elapsed time for linkedlist operation at following cases
 *      a) Serial program
 *      b) Parallel program (based on Pthreads) with one mutex for the entire linked list
 *      c) Parallel program (based on Pthreads) with read-write locks for the entire linked list
 */
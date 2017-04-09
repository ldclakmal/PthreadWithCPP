File: main.cpp

Purpose:
    Implement a linked list as a:
        a) Serial program
        b) Parallel program (based on Pthreads) with one mutex for the entire linked list
        c) Parallel program (based on Pthreads) with read-write locks for the entire linked list
    Your implementation should support Member( ), Insert( ), and Delete( ) functions. Populate the linked list with n random,
    but unique values. Make sure to set a different random seed for each execution. Each value should be between 0 and
    2^16 – 1. Then perform m random Member, Insert, and Delete operations (simultaneously) on the link list.
    Let m(Member), m(Insert), and m(Delete) be the fractions of operations of each type. You may use any values within
    0 and 2^16 – 1 while performing these three operations. However, to simplify the implementation, a new value inserted
    into the list cannot be a value already in the list (it could be a value that was initially added to the list, but
    later removed).

Compile:
    g++ main.cpp -o main.out -pthread
    * timer.h must be available
Run:
    main.out <program type> <number of threads>
Usage:
    main.out
Input:
    Program type, No of threads
Output:
    Elapsed time for linkedlist operation at following cases
    a) Serial program
    b) Parallel program (based on Pthreads) with one mutex for the entire linked list
    c) Parallel program (based on Pthreads) with read-write locks for the entire linked list
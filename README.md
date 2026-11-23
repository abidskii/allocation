# allocation (C++)

Overview
--------
Developed a custom memory allocator in C++. This implementation manages a fixed-size heap using a first-fit allocation strategy, block splitting, and coalescing to allocate, free, and reuse memory. The purpose of this project is to gain hands-on experience with systems programming and low-level memory management.

Features
--------

This implementation uses a first fit allocation strategy to find free memory. In order to ensure that all allocation payloads are properly aligned, payloads were aligned to std::max_align_t. 

Allocation of memory splits the memory into smaller blocks which can be freed and reused. 

To reduce fragmentation adjacent free block are combined into larger blocks (coalescing). 

GoogleTest is used for unit testing to verify allocation, deallocation, memory alignment, block reuse, and coalescing.

Memory Layout & Architecture
--------


Allocation & Deallocation Algorithms
--------



Building & Running
--------


Testing
--------


Design Decisions & Limitations
--------

Future Improvements
--------

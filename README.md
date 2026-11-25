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
The heap is composed of blocks. Each block can be either allocated or free. Every block contains a header that stores metadata about its state, such as its size, availability, and a pointer to the next block in the linked list.

<img width="800" height="315" alt="image" src="https://github.com/user-attachments/assets/4c0075f0-44c5-4724-835c-088743a84130" />

Allocation & Deallocation Algorithms
--------

The heap is divided into blocks that are organized as a linked list. The allocator uses a first-fit allocation strategy to find available memory. The search starts at the head of the linked list where the allocator looks for a free block in the heap that has enough space to hold the payload along with the necessary padding. The allocator marks the selected block as allocated and returns a pointer to its payload.. If enough space remains to accommodate another block header and usable free space, the allocator will split the block, creating the remaining space as a free block. If no free block has sufficient space, the allocator returns a null pointer (nullptr).

In order to deallocate memory, the allocator will check if the address of the pointer matches the starting address of any block's payload in the linked list. If a match is found and the block is allocated, the allocator will mark that block as free. The allocator will then coalesce adjacent free blocks into one block combining their sizes.

Building & Running
--------
## Building and Running

### Prerequisites

- C++20-compatible compiler (GCC or Clang)
- CMake 3.16 or newer
- Git

### Building

Clone the repository:

```bash
git clone https://github.com/abidskii/allocation.git
cd allocation
```

Configure the project using CMake:

```bash
cmake -S . -B build
```

Compile the allocator and test executable:

```bash
cmake --build build
```

### Running Tests

Run the GoogleTest suite through CTest:

```bash
ctest --test-dir build --output-on-failure
```

The tests verify allocation behavior, memory alignment, block reuse, coalescing, and preservation of allocated data.

Testing
--------
Google test was used to write tests and CTest was used to run them.

The test suite covers:

## Testing

GoogleTest is used to write unit tests for the allocator, while CTest is used to execute the test suite.

The tests cover the following functionality:

- **Basic allocation** 
- **Memory alignment**   
- **Allocation boundaries** 
- **Memory reuse**  
- **Pointer validation**  
- **Double-free handling**  
- **Block coalescing**  
- **Data integrity**  

### Running Tests

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Design Decisions & Limitations
--------
The allocator is limited to a fixed heap size that cannot grow dynamically. Data types are assumed to adhere to fundamental alignment requirements through std::max_align_t, so any data type with a stricter alignment is not supported. Additionally, the allocator cannot handle multiple concurrent threads.

#pragma once

#include <cstddef>

inline constexpr std::size_t ALIGNMENT = alignof(std::max_align_t);
inline constexpr std::size_t HEAP_SIZE{1024 * 1024};

// Allocates size bytes. Returns nullptr if size is zero
// or if insufficient memory is available.
void *my_alloc(std::size_t size);

// Frees an allocation previously returned by my_alloc.
// Null, invalid, and already-freed pointers are ignored.
void my_free(void *ptr);

// Testing utility. Discards all existing allocations.
// Previously returned pointers must not be used after reset.
void reset_allocator();

// Returns the maximum payload size supported by an empty heap.
std::size_t allocator_max_payload_size();

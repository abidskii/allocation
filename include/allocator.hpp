#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>

inline constexpr std::size_t ALIGNMENT = alignof(std::max_align_t);
inline constexpr std::size_t HEAP_SIZE{1024 * 1024};

size_t calculate_padding(std::uintptr_t address, size_t ALIGNMENT);
std::byte *align(std::byte *current_ptr, size_t alignment);
void *my_alloc(size_t size);
void *my_dealloc();
void reset_allocator();
std::size_t allocator_max_payload_size();

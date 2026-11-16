#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdint>


size_t calculate_padding(std::uintptr_t address, size_t ALIGNMENT);
std::byte * align(std::byte * current_ptr, size_t alignment);
void * my_alloc(size_t size);
void * my_dealloc();

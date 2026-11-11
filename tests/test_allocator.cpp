#include "../src/allocator.hpp"
#include <iostream>
#include <cassert>
#include <cstdint>

constexpr std::size_t ALIGNMENT = alignof(std::max_align_t);

void test_allocator(){


	void * test_ptr = my_alloc(12);

	assert(test_ptr!=nullptr);

	std::uintptr_t address = reinterpret_cast<std::uintptr_t> (test_ptr);

	assert(address % ALIGNMENT == 0);


	std::cout << "Payload address: " << test_ptr << '\n';
	std::cout << "Alignment test passed!\n";

}



int main (){
	return 0;
}

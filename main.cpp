#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <iostream>
#include <cstdint>

struct BlockHeader{

	size_t size;
	bool free;
	BlockHeader * next;

};

constexpr std::size_t HEAP_SIZE{1024*1024};
constexpr std::size_t HEAP_ALIGNMENT= std::max(alignof(std::max_align_t),alignof(BlockHeader));
constexpr std::size_t ALIGNMENT = alignof(std::max_align_t);

alignas(HEAP_ALIGNMENT) std::array<std::byte,HEAP_SIZE > heap{};

BlockHeader * head {nullptr};
BlockHeader * latest_header {nullptr};

void * align(BlockHeader * header){


	std::byte *current_ptr = reinterpret_cast<std::byte *> (head) + sizeof(BlockHeader) ;
	std::uintptr_t address = reinterpret_cast<std::uintptr_t>(current_ptr);
	std::size_t remainder = address % ALIGNMENT;
	if(remainder == 0){
		return current_ptr;
	}else {
		size_t offset = ALIGNMENT - remainder;
		current_ptr += offset;
		return current_ptr;
	}


	return nullptr;
}

void * my_alloc(size_t size){


	BlockHeader * current;
	if (!head){
		head = new (heap.data()) BlockHeader{HEAP_SIZE - sizeof(BlockHeader), true, nullptr};
		current = head;
        }

        void * aligned_ptr = align(current);

	void * next_header_ptr = reinterpret_cast<std::byte *> (aligned_ptr) + size ;

	current->next = static_cast<BlockHeader*>(next_header_ptr);
	current->free = false;

	return aligned_ptr;
}

void * my_dealloc(){


	my_alloc(4);
	return nullptr;

}

int main (){
	void * num = my_alloc(4);
	return 0;
}

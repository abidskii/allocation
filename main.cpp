#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
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

size_t calculate_padding(std::uintptr_t address, size_t ALIGNMENT){

	std::size_t remainder = address % ALIGNMENT;
	if(remainder == 0){
		return 0;
	}else {
		size_t  offset = ALIGNMENT - remainder;
		return offset;
	}

}

std::byte * align(std::byte * current_ptr, size_t alignment){


	std::uintptr_t address = reinterpret_cast<std::uintptr_t>(current_ptr);
	size_t offset =calculate_padding(address, ALIGNMENT);
	current_ptr += offset;
	return current_ptr;
}

void * my_alloc(size_t size){

	BlockHeader * current;

	if (!head){
		head = new (heap.data()) BlockHeader{size, false, nullptr};
		current = head;
        }else{
		while (current){
			current =current->next;
		}
		current = new (heap.data()) BlockHeader{size, false, nullptr};
        }


	//Convert pointer to a byte pointer then find the correct allignment for the paylod
	std::byte *  end_of_header= reinterpret_cast<std::byte*>(current);
        std::byte * start_of_payload = align(end_of_header,ALIGNMENT);

	//Find the end of the payload to then add the necessary padding needing to initalize the next header
	std::byte * end_of_payload = reinterpret_cast<std::byte *> (start_of_payload) + size;
	std::byte * begining_of_next_header = align(end_of_payload,ALIGNMENT);

	current->next = reinterpret_cast<BlockHeader*>(begining_of_next_header);

	return start_of_payload;
}

void * my_dealloc(){

	return nullptr;

}

int main (){
	void * num = my_alloc(4);
	return 0;
}

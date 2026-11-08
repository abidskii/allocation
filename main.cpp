#include <array>
#include <cstddef>
#include <new>


constexpr std::size_t HEAP_SIZE{1024*1024};

alignas(std::max_align_t) std::array<std::byte,HEAP_SIZE > heap{};

struct BlockHeader{

	size_t size;
	bool free;
	BlockHeader * next;

};

BlockHeader * head {nullptr};

BlockHeader * current {nullptr};

void * my_alloc(size_t size){

	if (!head){
		head = new (heap.data()) BlockHeader{HEAP_SIZE - sizeof(BlockHeader), true, nullptr};
	}






	return nullptr;
}

void * my_dealloc(){}

int main (){
	void * num = my_alloc(4);
	return 0;
}

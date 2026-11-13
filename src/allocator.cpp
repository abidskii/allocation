#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <new>

struct BlockHeader {

        size_t size;
        bool free;
        BlockHeader *next;
};

constexpr std::size_t HEAP_SIZE{1024 * 1024};
constexpr std::size_t HEAP_ALIGNMENT =
    std::max(alignof(std::max_align_t), alignof(BlockHeader));
constexpr std::size_t ALIGNMENT = alignof(std::max_align_t);
constexpr std::size_t HEADER_SIZE =
    ((sizeof(BlockHeader) + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT;

alignas(HEAP_ALIGNMENT) std::array<std::byte, HEAP_SIZE> heap{};

BlockHeader *head{nullptr};
BlockHeader *latest_header{nullptr};

size_t calculate_padding(std::uintptr_t address, size_t ALIGNMENT) {

        std::size_t remainder = address % ALIGNMENT;
        if (remainder == 0) {
                return 0;
        } else {
                size_t offset = ALIGNMENT - remainder;
                return offset;
        }
}

size_t calculate_alignment_offset(std::byte *current_ptr, size_t alignment) {

        std::uintptr_t address = reinterpret_cast<std::uintptr_t>(current_ptr);
        size_t offset = calculate_padding(address, ALIGNMENT);
        return offset;
}

void *my_alloc(size_t size) {

        BlockHeader *current;
        size_t block_size = 0;

        if (!head) {
                head = new (heap.data()) BlockHeader{HEAP_SIZE, true, nullptr};
        }

        current = head;

        while (current->next != nullptr) {
                current = current->next;
        }

        if (!current->free) {
                return nullptr;
        }

        // Convert pointer to a byte pointer then find the correct allignment
        // for the paylod
        std::byte *start_of_header = reinterpret_cast<std::byte *>(current);
        std::byte *end_of_header = start_of_header + sizeof(BlockHeader);
        block_size += sizeof(BlockHeader);
        size_t offset = calculate_alignment_offset(end_of_header, ALIGNMENT);
        std::byte *start_of_payload = end_of_header + offset;
        block_size += offset;

        // We need to check what free space there is after the header and see it
        // there is enoungh space to allocate
        if (block_size > current->size) {
                return nullptr;
        }
        size_t size_without_header = current->size - block_size;
        if (size > size_without_header) {
                return nullptr;
        }

        // Find the end of the payload to then add the necessary padding needing
        // to initalize the next header
        std::byte *end_of_payload = (start_of_payload) + size;
        block_size += size;

        current->free = false;

        // At this point, block_size includes:
        // original header + padding + requested payload

        size_t space_after_payload = current->size - block_size;

        // Padding needed to align the next header
        offset = calculate_alignment_offset(end_of_payload, ALIGNMENT);

        // Only attempt to split if the padding fits
        if (offset <= space_after_payload) {

                size_t padded_block_size = block_size + offset;

                // Space remaining for another block
                size_t remaining_size = current->size - padded_block_size;

                if (remaining_size >= HEADER_SIZE) {

                        std::byte *beginning_of_next_header =
                            end_of_payload + offset;

                        current->next = new (beginning_of_next_header)
                            BlockHeader{remaining_size, true, current->next};

                        current->size = padded_block_size;

                        assert(reinterpret_cast<std::uintptr_t>(current->next) %
                                   alignof(BlockHeader) ==
                               0);
                }
        }

        return start_of_payload;
}

void *my_dealloc() { return nullptr; }

int main() {
        void *num = my_alloc(4);
        return 0;
}

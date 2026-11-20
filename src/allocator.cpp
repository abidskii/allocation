#include "allocator.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <new>

namespace {
struct BlockHeader {

        size_t size;
        bool free;
        BlockHeader *next;
};

constexpr std::size_t HEAP_ALIGNMENT =
    std::max(alignof(std::max_align_t), alignof(BlockHeader));
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
        size_t offset = calculate_padding(address, alignment);
        return offset;
}

std::byte *allocate_header(BlockHeader *current, size_t *block_size) {

        // Convert pointer to a byte pointer then find the correct
        // allignment for the paylod
        std::byte *start_of_header = reinterpret_cast<std::byte *>(current);
        std::byte *end_of_header = start_of_header + sizeof(BlockHeader);
        *block_size += sizeof(BlockHeader);
        size_t offset = calculate_alignment_offset(end_of_header, ALIGNMENT);
        std::byte *start_of_payload = end_of_header + offset;
        *block_size += offset;
        return start_of_payload;
}

std::byte *get_payload_start(BlockHeader *current) {
        return reinterpret_cast<std::byte *>(current) + HEADER_SIZE;
}

void coalesce_free_blocks() {}

} // namespace

void *my_alloc(size_t size) {

        BlockHeader *current;

        if (!head) {
                head = new (heap.data()) BlockHeader{HEAP_SIZE, true, nullptr};
        }

        current = head;

        while (current != nullptr) {
                if (current->free) {
                        if (current->size >= HEADER_SIZE) {

                                std::size_t available_payload =
                                    current->size - HEADER_SIZE;

                                if (size <= available_payload) {
                                        break;
                                }
                        }
                }
                current = current->next;
        }

        if (current == nullptr) {
                return nullptr;
        }

        // Verify that the selected header is aligned.
        assert(reinterpret_cast<std::uintptr_t>(current) % ALIGNMENT == 0);

        std::byte *start_of_payload = get_payload_start(current);

        std::size_t block_size = HEADER_SIZE + size;

        std::byte *end_of_payload = start_of_payload + size;

        current->free = false;

        // At this point, block_size includes:
        // original header + padding + requested payload
        size_t space_after_payload = current->size - block_size;

        // Padding needed to align the next header
        //
        size_t offset = calculate_alignment_offset(end_of_payload, ALIGNMENT);

        // Only attempt to split if the padding fits
        if (offset <= space_after_payload) {

                size_t padded_block_size = block_size + offset;

                // Space remaining for another block
                size_t remaining_size = current->size - padded_block_size;

                if (remaining_size > HEADER_SIZE) {

                        std::byte *beginning_of_next_header =
                            end_of_payload + offset;

                        current->next = new (beginning_of_next_header)
                            BlockHeader{remaining_size, true, current->next};

                        current->size = padded_block_size;

                        assert(reinterpret_cast<std::uintptr_t>(current->next) %
                                   ALIGNMENT ==
                               0);
                }
        }

        return start_of_payload;
}

void reset_allocator() { head = nullptr; }

std::size_t allocator_max_payload_size() { return HEAP_SIZE - HEADER_SIZE; }

void my_free(void *current) {

        if (current == nullptr) {
                return;
        }

        BlockHeader *ptr = head;

        std::uintptr_t current_address =
            reinterpret_cast<std::uintptr_t>(current);

        while (ptr != nullptr) {

                std::byte *payload_start = get_payload_start(ptr);
                std::uintptr_t address_ptr =
                    reinterpret_cast<std::uintptr_t>(payload_start);
                if (current_address == address_ptr) {
                        break;
                }
                ptr = ptr->next;
        }

        if (ptr == nullptr) {
                return;
        }

        if (!ptr->free) {
                ptr->free = true;
        }

        coalesce_free_blocks();
}

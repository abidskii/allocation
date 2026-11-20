#include "allocator.hpp"
#include "gtest/gtest.h"
#include <cstddef>
#include <cstdint>

class AllocatorTest : public ::testing::Test {

      protected:
        void SetUp() override { reset_allocator(); }
};

TEST_F(AllocatorTest, BasicAllocation) {

        void *ptr = my_alloc(13);

        EXPECT_NE(ptr, nullptr);
}

TEST_F(AllocatorTest, PayloadAlignment) {

        void *ptr = my_alloc(13);
        ASSERT_NE(ptr, nullptr);

        std::uintptr_t address = reinterpret_cast<std::uintptr_t>(ptr);
        EXPECT_EQ(address % ALIGNMENT, 0u);
}

TEST_F(AllocatorTest, MultipleAllocations) {
        void *ptr1 = my_alloc(13);
        void *ptr2 = my_alloc(13);

        EXPECT_NE(ptr1, nullptr);
        EXPECT_NE(ptr2, nullptr);

        EXPECT_NE(ptr1, ptr2);
}

TEST_F(AllocatorTest, NonOverlappingAllocations) {

        auto *ptr1 = static_cast<std::byte *>(my_alloc(13));
        auto *ptr2 = static_cast<std::byte *>(my_alloc(20));

        EXPECT_NE(ptr1, nullptr);
        EXPECT_NE(ptr2, nullptr);

        auto address1 = reinterpret_cast<std::uintptr_t>(ptr1);
        auto address2 = reinterpret_cast<std::uintptr_t>(ptr2);

        EXPECT_TRUE(address1 + 13 <= address2 || address2 + 20 <= address1);
}

TEST_F(AllocatorTest, OversizedAllocation) {

        void *ptr = my_alloc(HEAP_SIZE);
        EXPECT_EQ(ptr, nullptr);
}

TEST_F(AllocatorTest, AllocationLargerThanHeap) {

        void *ptr = my_alloc(HEAP_SIZE + 1);
        EXPECT_EQ(ptr, nullptr);
}

TEST_F(AllocatorTest, ExactFitAllocation) {

        std::size_t capacity = allocator_max_payload_size();

        void *ptr1 = my_alloc(capacity);

        EXPECT_NE(ptr1, nullptr);

        void *ptr2 = my_alloc(1);
        EXPECT_EQ(ptr2, nullptr);
}

TEST_F(AllocatorTest, ExceedsMaximumPayload) {
        std::size_t capacity = allocator_max_payload_size();

        void *ptr = my_alloc(capacity + 1);

        EXPECT_EQ(ptr, nullptr);
}

TEST_F(AllocatorTest, FailedAllocationPreservesHeap) {
        std::size_t capacity = allocator_max_payload_size();

        EXPECT_EQ(my_alloc(capacity + 1), nullptr);

        EXPECT_NE(my_alloc(capacity), nullptr);
}

TEST_F(AllocatorTest, ReusesFreedBlock) {
        void *ptr1 = my_alloc(64);
        void *ptr2 = my_alloc(128);
        void *ptr3 = my_alloc(64);

        ASSERT_NE(ptr1, nullptr);
        ASSERT_NE(ptr2, nullptr);
        ASSERT_NE(ptr3, nullptr);

        my_free(ptr2);

        // Your task:
        // Allocate another 128 bytes.
        // Verify that the allocator reuses ptr2's address.

        void *new_ptr = my_alloc(128);

        ASSERT_NE(new_ptr, nullptr);
        EXPECT_EQ(new_ptr, ptr2);
}

TEST_F(AllocatorTest, RejectsInteriorPointer) {

        void *ptr1 = my_alloc(64);
        ASSERT_NE(ptr1, nullptr);
        std::byte *ptr_byte = reinterpret_cast<std::byte *>(ptr1);
        ptr_byte += 5;
        my_free(ptr_byte);

        void *ptr2 = my_alloc(64);
        ASSERT_NE(ptr2, nullptr);

        ASSERT_NE(ptr2, ptr1);
        // Before doing pointer arithmetic
}

TEST_F(AllocatorTest, DoubleFree) {

        void *ptr1 = my_alloc(64);
        void *ptr2 = my_alloc(64);
        ASSERT_NE(ptr1, nullptr);
        ASSERT_NE(ptr2, nullptr);

        my_free(ptr1);
        my_free(ptr1);

        void *ptr3 = my_alloc(64);
        ASSERT_NE(ptr3, nullptr);

        ASSERT_EQ(ptr3, ptr1);
        ASSERT_NE(ptr3, ptr2);
}

TEST_F(AllocatorTest, CoalesceTwoAdjacentBlocks) {

        void *ptr1 = my_alloc(128);
        void *ptr2 = my_alloc(128);
        void *ptr3 = my_alloc(128);
        void *ptr5 = my_alloc(128);

        ASSERT_NE(ptr1, nullptr);
        ASSERT_NE(ptr2, nullptr);
        ASSERT_NE(ptr3, nullptr);
        ASSERT_NE(ptr5, nullptr);

        my_free(ptr3);
        my_free(ptr2);

        void *ptr4 = my_alloc(130);
        ASSERT_NE(ptr4, nullptr);

        ASSERT_EQ(ptr4, ptr2);
}

TEST_F(AllocatorTest, CoalesceThreeAdjacentBlocks) {
        void *ptr1 = my_alloc(128);
        void *ptr2 = my_alloc(128);
        void *ptr3 = my_alloc(128);
        void *ptr4 = my_alloc(390);

        ASSERT_NE(ptr1, nullptr);
        ASSERT_NE(ptr2, nullptr);
        ASSERT_NE(ptr3, nullptr);
        ASSERT_NE(ptr4, nullptr);

        my_free(ptr1);
        my_free(ptr2);
        my_free(ptr3);

        void *ptr5 = my_alloc(390);
        ASSERT_NE(ptr5, nullptr);

        ASSERT_EQ(ptr5, ptr1);
}

TEST_F(AllocatorTest, NonAdjacentFreeBlocksPreserveAllocatedData) {
        void *ptr1 = my_alloc(128);
        void *ptr2 = my_alloc(128);
        void *ptr3 = my_alloc(128);
        void *ptr4 = my_alloc(390);
        void *ptr5 = my_alloc(128);

        ASSERT_NE(ptr1, nullptr);
        ASSERT_NE(ptr2, nullptr);
        ASSERT_NE(ptr3, nullptr);
        ASSERT_NE(ptr4, nullptr);
        ASSERT_NE(ptr5, nullptr);

        std::byte *data = reinterpret_cast<std::byte *>(ptr3);
        for (std::size_t i = 0; i < 128; i++) {

                data[i] = std::byte{static_cast<unsigned char>(i)};
        }

        my_free(ptr2);
        my_free(ptr4);

        void *ptr6 = my_alloc(390);
        ASSERT_NE(ptr6, nullptr);

        ASSERT_EQ(ptr6, ptr4);

        for (std::size_t i = 0; i < 128; i++) {

                ASSERT_EQ(data[i], std::byte{static_cast<unsigned char>(i)});
        }
}

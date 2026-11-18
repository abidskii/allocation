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
        EXPECT_NE(ptr, nullptr);

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

TEST_F(AllocatorTest, NonOverlappintAllocations) {

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

        std::size_t header_size = allocator_max_payload_size();

        void *ptr1 = my_alloc(header_size);

        EXPECT_NE(ptr1, nullptr);

        void *ptr2 = my_alloc(10);
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

        EXPECT_NE(my_alloc(64), nullptr);
}

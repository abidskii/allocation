#include "allocator.hpp"
#include "gtest/gtest.h"

TEST(AllocatorTest, BasicAllocation) {
        void *ptr = my_alloc(13);

        EXPECT_NE(ptr, nullptr);
}

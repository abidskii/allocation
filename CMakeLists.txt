cmake_minimum_required(VERSION 3.16)
project(Alloc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(allocator
	src/allocator.cpp
)

target_include_directories(allocator
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include(CTest)

if (BUILD_TESTING)

	include(FetchContent)
	FetchContent_Declare(
		googletest
		URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
	)


	FetchContent_MakeAvailable(googletest)

	add_executable(allocator_tests
        	tests/test_allocator.cpp
        )

	target_link_libraries(allocator_tests PRIVATE allocator GTest::gtest_main )

	include(GoogleTest)
	gtest_discover_tests(allocator_tests)
endif()

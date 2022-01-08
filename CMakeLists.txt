cmake_minimum_required(VERSION 3.20)
project(untitled)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Werror -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-return-type")

include_directories(.)

add_subdirectory(libs/spdlog)
include_directories(libs/spdlog/include)

enable_testing()
add_subdirectory(libs/googletest)
include_directories(libs/googletest/googletest/include)

set(COMMON_SRC
        utils/addressing.cpp
        emulator/emulator.cpp
        core/memoryView.cpp
        core/memoryRef.cpp
        core/pipeline.cpp
        core/memoryRegion.h
        core/registerFile.cpp
        core/registerFile.h)

add_executable(untitled main.cpp ${COMMON_SRC})
target_link_libraries(untitled spdlog)

add_executable(memory_test memory_test.cpp ${COMMON_SRC})
target_link_libraries(memory_test gtest gtest_main)

add_executable(endian_test endian_test.cpp ${COMMON_SRC})
target_link_libraries(endian_test gtest gtest_main)

add_executable(elffile_test elffile_test.cpp ${COMMON_SRC})
target_link_libraries(elffile_test gtest gtest_main)

add_executable(instruction_test test_ins.cpp ${COMMON_SRC})
target_link_libraries(instruction_test gtest gtest_main)

add_test(memory_test memory_test)
add_test(endian_test endian_test)
add_test(elffile_test elffile_test)
add_test(instruction_test instruction_test)

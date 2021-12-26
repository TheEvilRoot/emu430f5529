cmake_minimum_required(VERSION 3.20)
project(untitled)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

include_directories(libs/spdlog/include)
add_subdirectory(libs/spdlog)

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
add_executable(test test.cpp ${COMMON_SRC})
add_executable(test_endian test_endian.cpp ${COMMON_SRC})

target_link_libraries(untitled spdlog)
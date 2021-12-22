cmake_minimum_required(VERSION 3.20)
project(untitled)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

include_directories(libs/spdlog/include)
add_subdirectory(libs/spdlog)

add_executable(untitled
        emulator/emulator.cpp
        core/memoryView.cpp
        core/memoryRef.cpp
        core/pipeline.cpp
        main.cpp)
target_link_libraries(untitled spdlog)
cmake_minimum_required(VERSION 3.20)
project(untitled)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -Os -fconstexpr-steps=9000000 -fconstexpr-depth=900000")

include_directories(.)

add_subdirectory(libs/spdlog)
include_directories(libs/spdlog/include)

add_subdirectory(libs/glfw)
include_directories(libs/glfw/include)

enable_testing()
add_subdirectory(libs/googletest)
include_directories(libs/googletest/googletest/include)

include_directories(libs/imgui)
include_directories(libs/imgui/backends)
include_directories(libs/imgui_club/imgui_memory_editor)

set(GUI_SRC
        gui/emugui.cpp
        libs/imgui/imgui.cpp
        libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_tables.cpp
        libs/imgui/imgui_widgets.cpp
        libs/imgui/backends/imgui_impl_opengl3.cpp
        libs/imgui/backends/imgui_impl_opengl2.cpp
        libs/imgui/backends/imgui_impl_glfw.cpp)

set(COMMON_SRC
        core/interruptController.cpp
        emulator/decompiler.cpp
        utils/addressing.cpp
        emulator/emulator.cpp
        core/memoryView.cpp
        core/memoryRef.cpp
        core/pipeline.cpp
        core/memoryRegion.h
        core/registerFile.cpp
        core/registerFile.h
        core/statusRegister.h)

find_package(OpenGL REQUIRED)
if (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
endif()

add_executable(untitled main.cpp ${COMMON_SRC} ${GUI_SRC})
target_compile_features(untitled PRIVATE cxx_std_20)
target_link_libraries(untitled PRIVATE spdlog glfw ${OPENGL_LIBRARIES} ${COCOA_LIBRARY} ${IOKIT_LIBRARY})

if (APPLE)
    target_link_libraries(untitled PRIVATE "-framework OpenGL")
endif()

add_executable(memory_test memory_test.cpp ${COMMON_SRC})
target_link_libraries(memory_test gtest gtest_main)

add_executable(endian_test endian_test.cpp ${COMMON_SRC})
target_link_libraries(endian_test gtest gtest_main)

add_executable(elffile_test elffile_test.cpp ${COMMON_SRC})
target_link_libraries(elffile_test gtest gtest_main)

add_executable(instruction_test test_ins.cpp ${COMMON_SRC})
target_link_libraries(instruction_test gtest gtest_main)

add_executable(hex430_test test_hex430.cpp ${COMMON_SRC})
target_link_libraries(hex430_test gtest gtest_main)

add_test(memory_test memory_test)
add_test(endian_test endian_test)
add_test(elffile_test elffile_test)
add_test(instruction_test instruction_test)
add_test(hex430_test hex430_test)

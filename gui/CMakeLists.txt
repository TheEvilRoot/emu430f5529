cmake_minimum_required(VERSION 3.10)

add_subdirectory(glfw)

add_library(emu_gui emugui.cpp emugui.h
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_glfw.h)
target_link_libraries(emu_gui PRIVATE glfw)
target_include_directories(emu_gui PRIVATE glfw/include)
target_include_directories(emu_gui PRIVATE imgui/include)

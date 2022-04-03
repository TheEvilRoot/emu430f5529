cmake_minimum_required(VERSION 3.10)

add_subdirectory(../libs/glfw)

find_package(OpenGL REQUIRED)
find_library(COCOA_LIBRARY Cocoa)
find_library(IOKIT_LIBRARY IOKit)

add_library(emu_gui STATIC
        emugui.cpp
        ../libs/imgui/backends/imgui_impl_glfw.cpp
        ../libs/imgui/backends/imgui_impl_opengl3.cpp
        ../libs/imgui/backends/imgui_impl_opengl2.cpp
        ../libs/imgui/imgui.cpp
        ../libs/imgui/imgui_draw.cpp
        ../libs/imgui/imgui_widgets.cpp
        ../libs/imgui/imgui_tables.cpp)
target_compile_features(emu_gui PRIVATE cxx_std_20)
target_link_libraries(emu_gui PUBLIC ../libs/glfw)
target_include_directories(emu_gui PUBLIC ../libs/glfw/include)
target_include_directories(emu_gui PUBLIC ../libs/imgui/)
target_include_directories(emu_gui PUBLIC ../libs/imgui/backends)
target_link_directories(emu_gui PUBLIC ../libs/glfw ../libs/imgui ${OPENGL_LIBRARIES} ${COCOA_LIBRARY} ${IOKIT_LIBRARY})

target_link_directories(emu_gui PUBLIC /usr/local/lib)
target_include_directories(emu_gui PRIVATE /usr/local/include)

if (APPLE)
    target_link_libraries(emu_gui PRIVATE "-framework OpenGL")
endif()

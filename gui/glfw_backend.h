//
// Created by theevilroot on 3/20/22.
//

#ifndef UNTITLED_GLFW_BACKEND_H
#define UNTITLED_GLFW_BACKEND_H

#warning "OpenGL deprecation have been silenced"
#define GL_SILENCE_DEPRECATION

#include <cstdio>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>// Will drag system OpenGL headers

namespace emugui {
    struct GlfwBackend {

        const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        static void glfw_error_callback(int error, const char *description) {
            spdlog::error("GLFW failure {}: {}", error, description);
        }

        GLFWwindow *window{nullptr};

        int initiate(const char *title) {
            glfwSetErrorCallback(glfw_error_callback);
            if (!glfwInit()) {
                spdlog::error("GLFW init failure");
                return 1;
            }

            // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
            // GL ES 2.0 + GLSL 100
            const char *glsl_version = "#version 100";
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
            // GL 3.2 + GLSL 150
            const char *glsl_version = "#version 150";
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);// 3.2+ only
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);          // Required on Mac
#else
            // GL 3.0 + GLSL 130
            const char *glsl_version = "#version 130";
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
            //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
            //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
            window = glfwCreateWindow(1280, 720, title, nullptr, nullptr);
            if (window == nullptr) {
                spdlog::error("GLFW window create failure");
                return 1;
            }
            glfwMakeContextCurrent(window);
            glfwSwapInterval(1);// Enable vsync
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO &io = ImGui::GetIO();
            (void) io;
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            ImGui::StyleColorsDark();
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init(glsl_version);
            spdlog::info("GLFW backend just initialized gui");
            return 0;
        }

        [[nodiscard]] bool isTerminated() const {
            return glfwWindowShouldClose(window);
        }

        void renderPrepare() {
            glfwPollEvents();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        void renderFinalize() {
            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                         clear_color.z * clear_color.w, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }

        void terminate() const {
            // Cleanup
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            glfwDestroyWindow(window);
            glfwTerminate();
        }
    };
}// namespace emugui

#endif//UNTITLED_GLFW_BACKEND_H

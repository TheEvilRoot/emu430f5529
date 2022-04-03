//
// Created by theevilroot on 3/20/22.
//

#ifndef UNTITLED_EMUGUI_H
#define UNTITLED_EMUGUI_H

#include <imgui.h>
#include <imgui_memory_editor.h>

#include <cstdint>

#include <core/registerFile.h>



namespace emugui {
    enum class UserState {
        STEP,
        KILL,
        IDLE
    };

    template<typename Backend>
    struct EmuGui {

        core::RegisterFile& regs;
        core::MemoryView& ram;

        Backend backend{};
        MemoryEditor regsEditor{};
        MemoryEditor ramEditor{};

        ImVec4 colorRunning{0, 1, 0, 1};
        ImVec4 colorIdle{1, 0, 0, 1};

        bool isRunning{false};

        explicit EmuGui(core::RegisterFile& regs, core::MemoryView& ram): regs{regs}, ram{ram} {
            regsEditor.PreviewDataType = ImGuiDataType_U16;
            regsEditor.ReadOnly = true;

            ramEditor.PreviewDataType = ImGuiDataType_U16;
            regsEditor.ReadOnly = true;
        }

        void run() {
            backend.initiate("Emulator");
        }

        [[nodiscard]] auto render() {
            if (backend.isTerminated()) {
                backend.terminate();
                return UserState::KILL;
            }
            backend.renderPrepare();
            bool isStep = false;
            ImGui::Begin("Emulator Control");
            ImGui::Text("Emulator is");
            ImGui::SameLine();
            ImGui::TextColored(isRunning ? colorRunning : colorIdle, isRunning ? "Running" : "Idle");
            if (ImGui::ArrowButton("Run", ImGuiDir_Right)) {
                isRunning = !isRunning;
            }
            if (ImGui::Button("Step")) {
                isStep = true;
            }
            ImGui::End();
            regsEditor.DrawWindow("Registers", regs.memory.data.get(), regs.memory.size);
            ramEditor.DrawWindow("RAM", ram.data.get(), ram.size);
            backend.renderFinalize();
            return (isRunning || isStep) ? UserState::STEP : UserState::IDLE;
        }
    };
}

#endif //UNTITLED_EMUGUI_H

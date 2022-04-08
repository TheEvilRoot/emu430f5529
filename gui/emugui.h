//
// Created by theevilroot on 3/20/22.
//

#ifndef UNTITLED_EMUGUI_H
#define UNTITLED_EMUGUI_H

#include <imgui.h>
#include <imgui_memory_editor.h>

#include <cstdint>

#include <core/registerFile.h>
#include <emulator/decompiler.h>
#include <utils/instructions.h>

namespace emugui {
    enum class UserState {
        STEP,
        KILL,
        IDLE
    };

    template<typename Backend>
    struct EmuGui {

        // references from the emulator
        // must not be copied
        core::RegisterFile &regs;
        core::MemoryView &ram;

        // ui and rendering state
        Backend backend{};
        MemoryEditor regsEditor{};
        MemoryEditor ramEditor{};

        // consts
        const ImVec4 colorRunning{0, 1, 0, 1};
        const ImVec4 colorIdle{1, 0, 0, 1};

        // user-related/controlled emulator state
        // must be separated to emulator controller over EmuGui
        bool isRunning{false};
        std::vector<Decompiler::DecompiledInstruction> decompiled{};

        explicit EmuGui(core::RegisterFile &regs, core::MemoryView &ram) : regs{regs}, ram{ram} {
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

            ImGui::Begin("Decompiler", nullptr, ImGuiWindowFlags_NoCollapse);
            if (ImGui::Button("Update")) {
                const auto dec = Decompiler::get_decompiled(ram.data.get(), ram.size);
                decompiled = dec;
            }
            if (ImGui::BeginListBox("Decompiled output")) {
                for (const auto& i : decompiled) {
                    ImGui::BeginGroup();
                    ImGui::Text("%04X => ", i.pc);
                    ImGui::SameLine();
                    ImGui::Text("%s", i.repr.c_str());
                    ImGui::EndGroup();
                }
                ImGui::EndListBox();
            }
            ImGui::End();

            ImGui::Begin("Emulator Control", nullptr, ImGuiWindowFlags_NoCollapse);
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
}// namespace emugui

#endif//UNTITLED_EMUGUI_H

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

#include <utils/programLoader.h>
#include <utils/instructions.h>

namespace emugui {
    enum class UserState {
        STEP,
        KILL,
        IDLE
    };

    struct Led {
        const int index;
        const std::uint16_t address;
        const std::uint16_t mask;
        const ImVec4 activeColor;
        const ImVec4 inactiveColor;
    };

    template<typename Backend>
    struct EmuGui {

        // consts
        static constexpr ImVec4 colorRunning{0, 1, 0, 1};
        static constexpr ImVec4 colorIdle{1, 0, 0, 1};
        static constexpr ImVec4 ledActive{0, 1, 0, 1};
        static constexpr ImVec4 ledInactive{1, 1, 1, 1};
        static constexpr std::array leds = {
                Led{1, 0x0, 0x0, ledActive, ledInactive},
                Led{2, 0x0, 0x0, ledActive, ledInactive},
                Led{3, 0x0263, 0x2, ledActive, ledInactive},
                Led{4, 0x0263, 0x1, ledActive, ledInactive},
        };

        // references from the emulator
        // must not be copied
        core::RegisterFile &regs;
        core::MemoryView &ram;
        utils::ProgramLoader& loader;

        // ui and rendering state
        Backend backend{};
        MemoryEditor regsEditor{};
        MemoryEditor ramEditor{};

        std::string cwd{std::filesystem::current_path()};
        char openBuffer[256]{};

        // user-related/controlled emulator state
        // must be separated to emulator controller over EmuGui
        bool isRunning{false};
        std::vector<Decompiler::DecompiledInstruction> decompiled{};

        explicit EmuGui(core::RegisterFile &regs, core::MemoryView &ram, utils::ProgramLoader& loader) : regs{regs}, ram{ram}, loader{loader} {
            regsEditor.PreviewDataType = ImGuiDataType_U16;
            regsEditor.ReadOnly = true;

            ramEditor.PreviewDataType = ImGuiDataType_U16;
            regsEditor.ReadOnly = true;
        }

        void run() {
            backend.initiate("Emulator");
        }

        void renderLed(const Led& led, bool state) {
            const auto color = state ? led.activeColor : led.inactiveColor;
            ImGui::BeginGroup();
            ImGui::PushStyleColor(ImGuiCol_Button, color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
            ImGui::Button(fmt::format("LED{}", led.index).c_str());
            ImGui::PopStyleColor(2);
            ImGui::EndGroup();
        }

        [[nodiscard]] auto render() {
            const auto pc_val = regs.get_ref(0x0).get();
            if (backend.isTerminated()) {
                backend.terminate();
                return UserState::KILL;
            }
            backend.renderPrepare();
            bool isStep = false;

            ImGui::Begin("Decompiler", nullptr, ImGuiWindowFlags_NoCollapse);
            ImGui::PushItemWidth(-1);
            if (ImGui::BeginListBox("##Decompiled output", ImVec2(-1, -1))) {
                for (const auto& i : decompiled) {
                    ImGui::BeginGroup();
                    if (ImGui::Button(fmt::format("{:04X}", i.pc).c_str())) {
                        ramEditor.GotoAddrAndHighlight(i.pc, i.pc);
                    }
                    ImGui::SameLine();
                    ImGui::Text(" %s ", i.pc == pc_val ? "=>" : "  ");
                    ImGui::SameLine();
                    ImGui::Text("%s", i.repr.c_str());
                    ImGui::EndGroup();
                }
                ImGui::EndListBox();
            }
            ImGui::End();

            ImGui::Begin("Board");
            // leds
            ImGui::PushStyleColor(ImGuiCol_Text, ImColor(0.0f, 0.0f, 0.0f, 1.0f).Value);
            ImGui::BeginGroup();

            for (const auto& led : leds) {
                const auto value = ram.get_byte(led.address).get();
                const auto state = (value & led.mask) != 0;
                renderLed(led, state);
            }

            ImGui::EndGroup();
            ImGui::PopStyleColor(1);

            ImGui::End();

            ImGui::Begin("Emulator Control", nullptr, ImGuiWindowFlags_NoCollapse);

            ImGui::Text("Emulator is");
            ImGui::SameLine();
            ImGui::TextColored(isRunning ? colorRunning : colorIdle, isRunning ? "Running" : "Idle");

            ImGui::Text("Current directory %s", cwd.c_str());
            ImGui::InputText("##OpenProgramInput", openBuffer, sizeof(openBuffer), ImGuiInputTextFlags_AlwaysInsertMode);
            ImGui::SameLine();
            if (ImGui::Button("Open program")) {
                const auto path = std::string{openBuffer};
                if (!path.empty()) {
                    spdlog::info("loading program from {} by user", path);
                    try {
                        loader.load_program_from_file(path);
                        const auto dec = Decompiler::get_decompiled(ram.data.get(), ram.size);
                        decompiled = dec;
                    } catch (...) {
                        spdlog::error("program loading failure from {}", path);
                    }
                }
            }

            if (ImGui::ArrowButton("Run", ImGuiDir_Right)) {
                isRunning = !isRunning;
            }
            ImGui::SameLine();
            if (ImGui::Button("Step")) {
                isStep = true;
            }
            ImGui::End();
            if (ImGui::Begin("Registers")) {
                for (std::size_t i = 0; i < regs.count; i++) {
                    const auto value = regs.get_ref(i).get();
                    ImGui::Text("%4s: ", msp::addressing::reg_to_string(i).c_str());
                    ImGui::SameLine();
                    ImGui::Text("%04X", value);
                    if (i == 0 || i % 4 != 0) ImGui::SameLine();
                }
                ImGui::End();
            }
            ramEditor.DrawWindow("RAM", ram.data.get(), ram.size);
            backend.renderFinalize();
            return (isRunning || isStep) ? UserState::STEP : UserState::IDLE;
        }
    };
}// namespace emugui

#endif//UNTITLED_EMUGUI_H

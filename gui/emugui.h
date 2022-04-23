//
// Created by theevilroot on 3/20/22.
//

#ifndef UNTITLED_EMUGUI_H
#define UNTITLED_EMUGUI_H

#include <imgui.h>
#include <imgui_memory_editor.h>

#include <cstdint>
#include <filesystem>

#include <core/registerFile.h>
#include <emulator/decompiler.h>

#include <utils/programLoader.h>
#include <utils/instructions.h>
#include <utils/fileExplorer.h>
#include <utils/tickController.h>

namespace emugui {
    enum class UserState {
        STEP,
        KILL,
        IDLE,
        SINGLE_STEP
    };

    struct Led {
        const std::string_view tag;
        const std::uint16_t address;
        const std::uint16_t mask;
        const ImVec4 activeColor;
        const ImVec4 inactiveColor;
    };

    struct Button {
        const std::string_view tag;
        const std::uint16_t address;
        const std::uint16_t mask;
        const ImVec4 activeColor;
        const ImVec4 inactiveColor;
        bool state;
    };

    struct StatusFlag {
        const std::string_view tag;
        const std::uint16_t mask;
    };

    template<typename Backend>
    struct EmuGui {

        // consts
        static constexpr ImVec4 colorRunning{0, 1, 0, 1};
        static constexpr ImVec4 colorIdle{1, 0, 0, 1};
        static constexpr ImVec4 ledActive{0, 1, 0, 1};
        static constexpr ImVec4 ledInactive{1, 1, 1, 1};
        static constexpr ImVec4 buttonActive{59, 37, 36, 1};
        static constexpr std::array leds = {
                Led{"LED1", 0x0202, 0x1, ledActive, ledInactive},
                Led{"LED2", 0x0263, 0x2, ledActive, ledInactive},
                Led{"LED3", 0x0263, 0x4, ledActive, ledInactive},
        };
        static constexpr std::array pads = {
                Led{"PAD1", 0x0202, 0x1, ledActive, ledInactive},
                Led{"PAD2", 0x0202, 0x2, ledActive, ledInactive},
                Led{"PAD3", 0x0202, 0x4, ledActive, ledInactive},
                Led{"PAD4", 0x0202, 0x8, ledActive, ledInactive},
                Led{"PAD5", 0x0202, 0x10, ledActive, ledInactive},
        };
        static constexpr std::array status_flag = {
                StatusFlag{"C", 0x1},
                StatusFlag{"Z", 0x2},
                StatusFlag{"N", 0x4},
                StatusFlag{"GIE", 0x8},
                StatusFlag{"CPU_OFF", 0x10},
                StatusFlag{"OSC_OFF", 0x20},
                StatusFlag{"SCG0", 0x40},
                StatusFlag{"SCG1", 0x80},
                StatusFlag{"V", 0x100}
        };

        std::array<Button, 2> buttons = {
                Button{"S1", 0x0200, 0x80, buttonActive, buttonActive, false},
                Button{"S2", 0x0201, 0x4, buttonActive, buttonActive, false}
        };

        // references from the emulator
        // must not be copied
        core::RegisterFile &regs;
        core::MemoryView &ram;
        utils::ProgramLoader& loader;
        TickController& tick_controller;


        // ui and rendering state
        Backend backend{};
        MemoryEditor regsEditor{};
        MemoryEditor ramEditor{};
        FileExplorer fileExplorer{};

        // user-related/controlled emulator state
        // must be separated to emulator controller over EmuGui
        bool isRunning{false};
        std::vector<Decompiler::DecompiledInstruction> decompiled{};

        explicit EmuGui(core::RegisterFile &regs, core::MemoryView &ram, utils::ProgramLoader& loader, TickController& tick_controller) : regs{regs}, ram{ram}, loader{loader}, tick_controller{tick_controller} {
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
            ImGui::Button(led.tag.data());
            ImGui::PopStyleColor(2);
            ImGui::EndGroup();
        }

        bool renderButton(Button& led) {
            ImGui::BeginGroup();
            ImGui::PushStyleColor(ImGuiCol_Button, led.inactiveColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, led.activeColor);
            ImGui::PushStyleColor(ImGuiCol_Text, led.activeColor);
            ImGui::Checkbox(led.tag.data(), &led.state);
            ImGui::PopStyleColor(3);
            ImGui::EndGroup();
            return led.state;
        }

        void select_file(File file) {
            spdlog::info("loading program from {} by user", file.display_name);
            try {
                loader.load_program_from_file(file.path);
                const auto dec = Decompiler::get_decompiled(ram.data.get(), ram.size);
                decompiled = dec;
            } catch (...) {
                spdlog::error("program loading failure from {}", file.path.string());
            }
        }

        [[nodiscard]] auto render() {
            const auto frequency = tick_controller.frequency;
            const auto pc_val = regs.get_ref(0x0).get();
            const auto status_ref = regs.get_ref(0x2).get();
            if (backend.isTerminated()) {
                backend.terminate();
                return UserState::KILL;
            }
            backend.renderPrepare();
            bool isStep = false;

            if (ImGui::Begin("File explorer")) {
                if (ImGui::InputText("##Working directory", fileExplorer.buffer, FileExplorer::bufferSize, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    const auto path = std::string{fileExplorer.buffer};
                    if (!path.empty()) {
                        fileExplorer.set_current(path);
                    }
                }
                if (ImGui::BeginListBox("##Files", ImVec2(-1, -1))) {
                    for (const auto &file: fileExplorer.files) {
                        if (!file.display_name.empty()) {
                            ImGui::BeginDisabled(!file.is_selectable);
                            if (ImGui::Selectable(file.display_name.c_str(), false)) {
                                if (const auto selected = fileExplorer.select(file); selected) {
                                    select_file(selected.value());
                                }
                            }
                            ImGui::EndDisabled();
                        }
                    }
                    ImGui::EndListBox();
                }
                ImGui::End();
            }

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
            ImGui::SameLine();
            ImGui::BeginGroup();
            for (const auto& led : pads) {
                const auto value = ram.get_byte(led.address).get();
                const auto state = (value & led.mask) != 0;
                renderLed(led, state);
            }
            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::BeginGroup();
            for (auto& led : buttons) {
                auto ref = ram.get_byte(led.address);
                const auto value = ref.get();
                if (renderButton(led)) {
                    ref.set(value | led.mask);
                } else {
                    ref.set(value & ~led.mask);
                }
            }
            ImGui::EndGroup();
            ImGui::PopStyleColor(1);

            ImGui::End();

            ImGui::Begin("Emulator Control", nullptr, ImGuiWindowFlags_NoCollapse);

            ImGui::Text("Emulator is");
            ImGui::SameLine();
            ImGui::TextColored(isRunning ? colorRunning : colorIdle, isRunning ? "Running" : "Idle");

            if (ImGui::ArrowButton("Run", ImGuiDir_Right)) {
                isRunning = !isRunning;
            }
            ImGui::SameLine();
            if (ImGui::Button("Step")) {
                isStep = true;
            }
            ImGui::Text("Frequency %d", frequency);
            ImGui::End();
            if (ImGui::Begin("Registers")) {
                for (std::size_t i = 0; i < regs.count; i++) {
                    const auto value = regs.get_ref(i).get();
                    ImGui::Text("%4s: ", msp::addressing::reg_to_string(i).c_str());
                    ImGui::SameLine();
                    ImGui::Text("%04X", value);
                    if ((i == 0 || i % 4 != 0) && i != regs.count - 1) ImGui::SameLine();
                }

                for (std::size_t i = 0;const auto& flag : status_flag) {
                    ImGui::BeginGroup();
                    ImGui::Text("%7s", flag.tag.data());
                    ImGui::SameLine();
                    ImGui::Text("%s", (status_ref & flag.mask) ? "1" : "0");
                    ImGui::EndGroup();
                    if (i % 4 != 0) ImGui::SameLine();
                    i++;
                }
                ImGui::End();
            }
            ramEditor.DrawWindow("RAM", ram.data.get(), ram.size);
            backend.renderFinalize();
            return (isRunning ? UserState::STEP : (isStep ? UserState::SINGLE_STEP : UserState::IDLE));
        }
    };
}// namespace emugui

#endif//UNTITLED_EMUGUI_H

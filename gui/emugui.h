//
// Created by theevilroot on 3/20/22.
//

#ifndef UNTITLED_EMUGUI_H
#define UNTITLED_EMUGUI_H

#include <imgui.h>
#include <imgui_memory_editor.h>

#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <spdlog/fmt/compile.h>
#include <spdlog/fmt/bundled/format.h>

#include <core/registerFile.h>
#include <core/interruptController.h>
#include <emulator/decompiler.h>

#include <utils/programLoader.h>
#include <utils/instructions.h>
#include <utils/fileExplorer.h>
#include <utils/tickController.h>
#include <utils/breakpointController.h>
#include <utils/ports.h>
#include <utils/measure.h>

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
        static constexpr ImVec4 buttonBreakpoint{255, 0, 0, 1};

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
                Button{"S1", 0x0200, 0x80, false},
                Button{"S2", 0x0201, 0x4, false}
        };

        // references from the emulator
        // must not be copied
        core::RegisterFile &regs;
        core::MemoryView &ram;
        utils::ProgramLoader& loader;
        TickController& tick_controller;
        InterruptController& interrupt_controller;
        utils::BreakpointController& breakpoint_controller;


        // ui and rendering state
        Backend backend{};
        utils::FileExplorer fileExplorer{};

        MemoryEditor ramEditor{};

        // user-related/controlled emulator state
        // must be separated to emulator controller over EmuGui
        std::vector<emu::Decompiler::DecompiledInstruction> decompiled{};

        explicit EmuGui(core::RegisterFile &regs, core::MemoryView &ram, utils::ProgramLoader& loader, TickController& tick_controller, InterruptController& interrupt_controller, utils::BreakpointController& breakpoint_controller) : regs{regs}, ram{ram}, loader{loader}, tick_controller{tick_controller}, interrupt_controller{interrupt_controller}, breakpoint_controller{breakpoint_controller} {
            ramEditor.PreviewDataType = ImGuiDataType_U16;
            ramEditor.ReadOnly = false;
            ramEditor.WriteFn = [](auto*, auto, auto) { };
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
            ImGui::PushStyleColor(ImGuiCol_Text, buttonActive);
            ImGui::Checkbox(led.tag.data(), &led.state);
            ImGui::PopStyleColor(1);
            ImGui::EndGroup();
            return led.state;
        }

        void select_file(utils::File file) {
            spdlog::info("loading program from {} by user", file.display_name);
            try {
                loader.load_program_from_file(file.path.string());
                const auto dec = emu::Decompiler::get_decompiled(ram.data.get(), ram.size);
                decompiled = dec;
            } catch (...) {
                spdlog::error("program loading failure from {}", file.path.string());
            }
        }

        auto renderFileExplorer() {
            if (ImGui::Begin("File explorer")) {
                if (ImGui::InputText("##Working directory", fileExplorer.buffer, utils::FileExplorer::bufferSize, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    const auto path = std::string{fileExplorer.buffer};
                    if (!path.empty()) {
                        fileExplorer.set_current(path);
                    }
                }
                if (ImGui::BeginListBox("##Files", ImVec2(-1, -1))) {
                    if (!fileExplorer.state.empty()) {
                        const auto &state = fileExplorer.state.back();
                        for (const auto &file: state.files) {
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
                    }
                    ImGui::EndListBox();
                }
                ImGui::End();
            }
        }

        auto renderDecompiler(std::uint16_t pc_val) {
            if (ImGui::Begin("Decompiler", nullptr, ImGuiWindowFlags_NoCollapse)) {
                ImGui::PushItemWidth(-1);
                if (ImGui::BeginListBox("##Decompiled output", ImVec2(-1, -1))) {
                    for (const auto &i: decompiled) {
                        const auto has_breakpoint = breakpoint_controller.has_breakpoint(i.pc);
                        if (!i.label.empty()) {
                            ImGui::BeginGroup();
                            ImGui::Text(":%s", i.label.c_str());
                            ImGui::EndGroup();
                        }
                        ImGui::BeginGroup();
                        if (has_breakpoint) {
                            ImGui::PushStyleColor(ImGuiCol_Button, buttonBreakpoint);
                        }
                       if (ImGui::Button(fmt::format("{:04X}", i.pc).c_str())) {
                            if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
                                breakpoint_controller.toggle_breakpoint(i.pc);
                            } else {
                                ramEditor.GotoAddrAndHighlight(i.pc, i.pc);
                            }
                        }
                        if (has_breakpoint) {
                            ImGui::PopStyleColor();
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
            }
        }

        auto renderBoard() {
            if (ImGui::Begin("Board")) {
                // leds
                ImGui::PushStyleColor(ImGuiCol_Text, ImColor(0.0f, 0.0f, 0.0f, 1.0f).Value);
                ImGui::BeginGroup();
                for (const auto &led: leds) {
                    const auto value = ram.get_byte(led.address).get();
                    const auto state = (value & led.mask) != 0;
                    renderLed(led, state);
                }
                ImGui::EndGroup();
                ImGui::SameLine();
                ImGui::BeginGroup();
                for (const auto &led: pads) {
                    const auto value = ram.get_byte(led.address).get();
                    const auto state = (value & led.mask) != 0;
                    renderLed(led, state);
                }
                ImGui::EndGroup();
                ImGui::SameLine();
                ImGui::BeginGroup();
                for (auto &led: buttons) {
                    auto ref = ram.get_byte(led.address);
                    const auto value = ref.get();
                    const auto prev_state = led.state;
                    const auto next_state = renderButton(led);
                    if (next_state) {
                        ref.set(value | led.mask);
                    } else {
                        ref.set(value & ~led.mask);
                    }
                    if (prev_state != next_state) {
                        const auto generated = led.mask == 0x80 ? interrupt_controller.generate_interrupt<utils::Port1Interrupt, 7>(next_state) : interrupt_controller.generate_interrupt<utils::Port2Interrupt, 2>(next_state);
                        spdlog::warn("generated interrupt for {} => {}", led.tag, generated);
                    }
                }
                ImGui::EndGroup();
                ImGui::PopStyleColor(1);

                ImGui::End();
            }
        }

        auto renderEmulatorControl(std::uint64_t frequency, bool &isRunning, bool &isStep) {
            if (ImGui::Begin("Emulator Control", nullptr, ImGuiWindowFlags_NoCollapse)) {
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
                ImGui::Text("Frequency %llu Hz", frequency);
                ImGui::End();
            }
        }

        auto renderRegisters(std::uint16_t status) {
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
                    ImGui::Text("%s", (status & flag.mask) ? "1" : "0");
                    ImGui::EndGroup();
                    if (i % 4 != 0) ImGui::SameLine();
                    i++;
                }
                ImGui::End();
            }
        }

        auto renderRam() {
            ramEditor.DrawWindow("RAM", ram.data.get(), ram.size);
        }

        auto renderPorts() {
            if (ImGui::Begin("I/O Ports")) {
                if (ImGui::BeginTable("Ports", 3)) {
                    utils::ports::for_each_port(ram, [](const auto& label, const auto addr, const auto value) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Text("%s", label.data());
                        ImGui::TableNextColumn();
                        ImGui::Text("%04x", addr);
                        ImGui::TableNextColumn();
                        ImGui::Text("%s",fmt::format("{:08b}", value).c_str());
                    });
                    ImGui::EndTable();
                }
                ImGui::End();
            }
        }

        [[nodiscard]] auto render(UserState previous_state) {
            static measure m = measure{};
            m.template get_time<>(true);
            const auto frequency = tick_controller.frequency;
            const auto pc_val = regs.get_ref(0x0).get();
            const auto status = regs.get_ref(0x2).get();

            if (backend.isTerminated()) {
                backend.terminate();
                return UserState::KILL;
            }

            backend.renderPrepare();
            bool isRunning = previous_state == UserState::STEP;
            bool isStep = false;

#ifdef MEASURE_GUI
            auto prepare = m.template get_time(true);
#endif
            renderFileExplorer();
#ifdef MEASURE_GUI
            auto renderFileExplorer = m.template get_time(true);
#endif
            renderDecompiler(pc_val);
#ifdef MEASURE_GUI
            auto renderDecompiler = m.template get_time(true);
#endif
            renderBoard();
#ifdef MEASURE_GUI
            auto renderBoard = m.template get_time(true);
#endif
            renderEmulatorControl(frequency, isRunning, isStep);
#ifdef MEASURE_GUI
            auto renderEmulatorControl = m.template get_time(true);
#endif
            renderRegisters(status);
#ifdef MEASURE_GUI
            auto renderRegisters = m.template get_time(true);
#endif
            renderRam();
#ifdef MEASURE_GUI
            auto renderRam = m.template get_time(true);
#endif
            renderPorts();
#ifdef MEASURE_GUI
            auto renderPorts = m.template get_time(true);
#endif

            if (ImGui::IsKeyPressed(ImGuiKey_1, false)) {
                buttons[0].state = true;
                auto ref = ram.get_byte(buttons[0].address);
                const auto value = ref.get();
                ref.set(value & ~buttons[0].mask);
                const auto generated = buttons[0].mask == 0x80 ? interrupt_controller.generate_interrupt<utils::Port1Interrupt, 7>(true) : interrupt_controller.generate_interrupt<utils::Port2Interrupt, 2>(true);
                spdlog::warn("keyboard generated interrupt for {} => {}", buttons[0].tag, generated);
            }
            if (ImGui::IsKeyReleased(ImGuiKey_1)) {
                buttons[0].state = false;
                auto ref = ram.get_byte(buttons[0].address);
                const auto value = ref.get();
                ref.set(value | buttons[0].mask);
                const auto generated = buttons[0].mask == 0x80 ? interrupt_controller.generate_interrupt<utils::Port1Interrupt, 7>(false) : interrupt_controller.generate_interrupt<utils::Port2Interrupt, 2>(false);
                spdlog::warn("keyboard  generated interrupt for {} => {}", buttons[0].tag, generated);
            }

            if (ImGui::IsKeyPressed(ImGuiKey_2, false)) {
                buttons[1].state = true;
                auto ref = ram.get_byte(buttons[1].address);
                const auto value = ref.get();
                ref.set(value & ~buttons[1].mask);
                const auto generated = buttons[1].mask == 0x80 ? interrupt_controller.generate_interrupt<utils::Port1Interrupt, 7>(true) : interrupt_controller.generate_interrupt<utils::Port2Interrupt, 2>(true);
                spdlog::warn("keyboard generated interrupt for {} => {}", buttons[1].tag, generated);
            }
            if (ImGui::IsKeyReleased(ImGuiKey_2)) {
                buttons[1].state = false;
                auto ref = ram.get_byte(buttons[1].address);
                const auto value = ref.get();
                ref.set(value | buttons[1].mask);
                const auto generated = buttons[1].mask == 0x80 ? interrupt_controller.generate_interrupt<utils::Port1Interrupt, 7>(false) : interrupt_controller.generate_interrupt<utils::Port2Interrupt, 2>(false);
                spdlog::warn("keyboard generated interrupt for {} => {}", buttons[1].tag, generated);
            }

            backend.renderFinalize();
#ifdef MEASURE_GUI
            auto renderFinalize = m.template get_time(true);
#endif

#ifdef MEASURE_GUI
            spdlog::warn(
                    "prepare = {}ms "
                    "fileExplorer = {}ms "
                    "decompiler = {}ms "
                    "board = {}ms "
                    "emulatorControl = {}ms "
                    "registers = {}ms "
                    "ram = {}ms "
                    "ports = {}ms "
                    "finalize = {}ms",
                    prepare, renderFileExplorer,
                    renderDecompiler,
                    renderBoard,
                    renderEmulatorControl,
                    renderRegisters,
                    renderRam,
                    renderPorts,
                    renderFinalize
                    );
#endif
            return (isRunning ? UserState::STEP : (isStep ? UserState::SINGLE_STEP : UserState::IDLE));
        }
    };
}// namespace emugui

#endif//UNTITLED_EMUGUI_H

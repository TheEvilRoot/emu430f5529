//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_EMULATOR_EMULATOR_H_
#define UNTITLED_EMULATOR_EMULATOR_H_

#include <dispatch/dispatch.h>

#include <core/memoryView.h>
#include <core/pipeline.h>
#include <core/registerFile.h>
#include <core/interruptController.h>

#include <utils/tickController.h>
#include <utils/program.h>
#include <utils/programLoader.h>
#include <utils/interrupts.h>
#include <utils/breakpointController.h>

#include <gui/emugui.h>
#include <gui/glfw_backend.h>

#include <thread>

namespace emu {
    class Emulator {
    private:
        core::MemoryView ram;
        core::RegisterFile regs;
        utils::ProgramLoader loader;

        core::Pipeline pipeline;

        InterruptController interrupt_controller;
        TickController tick_controller;
        utils::BreakpointController breakpoint_controller;
        emugui::EmuGui<emugui::GlfwBackend> gui;

        std::atomic<emugui::UserState> shared_state;

        std::mutex halt_mutex{};
        std::condition_variable halt_state{};
    public:
        Emulator() : ram(0x10000), regs(16), loader{ram, regs}, pipeline(regs, ram), interrupt_controller{regs, ram}, tick_controller(0), breakpoint_controller{}, gui{regs, ram, loader, tick_controller, interrupt_controller, breakpoint_controller}, shared_state{emugui::UserState::IDLE} {
            ram.add_region(core::MemoryRegion{"special function registers", 0x0, 0xF, core::MemoryRegionAccess::ONLY_BYTE});
            ram.add_region(core::MemoryRegion{"8-bit peripheral", 0x10, 0xFF, core::MemoryRegionAccess::ONLY_BYTE});
            ram.add_region(core::MemoryRegion{"16-bit peripheral", 0x100, 0x1FF, core::MemoryRegionAccess::ONLY_WORD});
            ram.add_region(core::MemoryRegion{"ram", 0x200, 0x9FF, core::MemoryRegionAccess::WORD_BYTE});

            auto status_ref = regs.get_ref(0x2);
            status_ref.set(status_ref.get() | 0x8);
        }

        void run() {
            std::thread thread{[this]() {
                auto pc = regs.get_ref(0x0);
                emugui::UserState previous_state{emugui::UserState::IDLE};
                while (true) {
                    const auto current_state = shared_state.load();
                    tick_controller.generate_tick();
                    if (current_state == emugui::UserState::IDLE && previous_state == emugui::UserState::IDLE) {
                        spdlog::warn("halt detected on cpu thread");
                        std::unique_lock lock(halt_mutex);
                        halt_state.wait(lock);
                        spdlog::warn("halt notified");
                    }
                    if (current_state == emugui::UserState::STEP || (current_state == emugui::UserState::SINGLE_STEP && previous_state != emugui::UserState::SINGLE_STEP)) {
                        const auto interrupt = interrupt_controller.consume_interrupt();
                        try {
                            if (interrupt) {
                                pipeline.interrupt_step(interrupt.value());
                            } else {
                                if (previous_state != emugui::UserState::IDLE && breakpoint_controller.has_breakpoint(pc.get())) {
                                    shared_state.store(emugui::UserState::IDLE);
                                } else {
                                    pipeline.step();
                                }
                            }
                        } catch (const MemoryViolationException& e) {
                            spdlog::error("{}", e.what());
                            shared_state.store(emugui::UserState::IDLE);
                        }
                    } else if (current_state == emugui::UserState::KILL) {
                        break;
                    }
                    previous_state = current_state;
                }
            }};
            gui.run();
            emugui::UserState current_state;
            while (shared_state != emugui::UserState::KILL) {
                current_state = shared_state;
                const auto new_state = gui.render(current_state);
                if (new_state != current_state) {
                    shared_state = new_state;
                    if (current_state == emugui::UserState::IDLE && new_state != emugui::UserState::IDLE) {
                        halt_state.notify_all();
                    }
                }
            }
            thread.join();
        }
    };
}// namespace emu

#endif//UNTITLED_EMULATOR_EMULATOR_H_

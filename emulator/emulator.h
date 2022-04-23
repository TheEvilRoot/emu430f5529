//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_EMULATOR_EMULATOR_H_
#define UNTITLED_EMULATOR_EMULATOR_H_

#include <core/memoryView.h>
#include <core/pipeline.h>
#include <core/registerFile.h>
#include <core/interruptController.h>

#include <utils/tickController.h>
#include <utils/program.h>
#include <utils/programLoader.h>
#include <utils/interrupts.h>

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

        TickController tick_controller;
        emugui::EmuGui<emugui::GlfwBackend> gui;

        std::atomic<emugui::UserState> shared_state;

    public:
        Emulator() : ram(0x10000), regs(16), loader{ram, regs}, pipeline(regs, ram), tick_controller(0), gui{regs, ram, loader, tick_controller}, shared_state{emugui::UserState::IDLE} {
            ram.add_region(core::MemoryRegion{"special function registers", 0x0, 0xF, core::MemoryRegionAccess::ONLY_BYTE});
            ram.add_region(core::MemoryRegion{"8-bit peripheral", 0x10, 0xFF, core::MemoryRegionAccess::ONLY_BYTE});
            ram.add_region(core::MemoryRegion{"16-bit peripheral", 0x100, 0x1FF, core::MemoryRegionAccess::ONLY_WORD});
            ram.add_region(core::MemoryRegion{"ram", 0x200, 0x9FF, core::MemoryRegionAccess::WORD_BYTE});
        }

        void run() {
            std::thread thread{[this]() {
                auto pc = regs.get_ref(0x0);
                emugui::UserState current_state{emugui::UserState::IDLE};
                while (true) {
                    const auto state = shared_state.load();
                    tick_controller.generate_tick();
                    if (state == emugui::UserState::STEP || (state == emugui::UserState::SINGLE_STEP && current_state != emugui::UserState::SINGLE_STEP)) {
                        pipeline.step();
                    } else if (state == emugui::UserState::KILL) {
                        break;
                    }
                    current_state = state;
                }
            }};

            gui.run();

            emugui::UserState current_state{emugui::UserState::IDLE};
            while (shared_state != emugui::UserState::KILL) {
                const auto new_state = gui.render();
                if (new_state != current_state) {
                    shared_state = new_state;
                }
                current_state = new_state;
            }
            thread.join();
        }
    };
}// namespace emu

#endif//UNTITLED_EMULATOR_EMULATOR_H_

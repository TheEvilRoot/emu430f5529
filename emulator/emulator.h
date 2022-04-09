//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_EMULATOR_EMULATOR_H_
#define UNTITLED_EMULATOR_EMULATOR_H_

#include <core/memoryView.h>
#include <core/pipeline.h>
#include <core/registerFile.h>

#include <utils/tickController.h>
#include <utils/program.h>
#include <utils/programLoader.h>

#include <gui/emugui.h>
#include <gui/glfw_backend.h>

namespace emu {
    class Emulator {
    private:
        core::MemoryView ram;
        core::RegisterFile regs;
        utils::ProgramLoader loader;

        core::Pipeline pipeline;

        TickController tick_controller;
        emugui::EmuGui<emugui::GlfwBackend> gui;

    public:
        Emulator() : ram(0x10000), regs(16), loader{ram, regs}, pipeline(regs, ram), tick_controller(0), gui{regs, ram, loader} {
            ram.add_region(core::MemoryRegion{"special function registers", 0x0, 0xF, core::MemoryRegionAccess::ONLY_BYTE});
            ram.add_region(core::MemoryRegion{"8-bit peripheral", 0x10, 0xFF, core::MemoryRegionAccess::ONLY_BYTE});
            ram.add_region(core::MemoryRegion{"16-bit peripheral", 0x100, 0x1FF, core::MemoryRegionAccess::ONLY_WORD});
            ram.add_region(core::MemoryRegion{"ram", 0x200, 0x9FF, core::MemoryRegionAccess::WORD_BYTE});
        }

        void run() {
            auto pc = regs.get_ref(0x0);
            gui.run();
            while (true) {
                const auto state = gui.render();
                if (state == emugui::UserState::KILL) {
                    break;
                } else if (state == emugui::UserState::STEP) {
                    pipeline.step();
                    tick_controller.tick_control_sleep();
                }
            }
        }
    };
}// namespace emu

#endif//UNTITLED_EMULATOR_EMULATOR_H_

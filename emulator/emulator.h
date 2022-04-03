//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_EMULATOR_EMULATOR_H_
#define UNTITLED_EMULATOR_EMULATOR_H_

#include <core/memoryView.h>
#include <core/registerFile.h>
#include <core/pipeline.h>

#include <utils/tickController.h>

#include <gui/emugui.h>
#include <gui/glfw_backend.h>

namespace emu {
class Emulator {
 private:
  core::MemoryView ram;
  core::RegisterFile regs;

  core::Pipeline pipeline;

  TickController tick_controller;
    emugui::EmuGui<emugui::GlfwBackend> gui;

 public:
  Emulator(): ram(0x10000), regs(16), pipeline(regs, ram), tick_controller(0), gui{regs, ram} {
      ram.add_region(core::MemoryRegion{"special function registers", 0x0, 0xF, core::MemoryRegionAccess::ONLY_BYTE});
      ram.add_region(core::MemoryRegion{"8-bit peripheral", 0x10, 0xFF, core::MemoryRegionAccess::ONLY_BYTE});
      ram.add_region(core::MemoryRegion{"16-bit peripheral", 0x100, 0x1FF, core::MemoryRegionAccess::ONLY_WORD});
      ram.add_region(core::MemoryRegion{"ram", 0x200, 0x9FF, core::MemoryRegionAccess::WORD_BYTE});
  }

  void load(const char* buffer, std::size_t count, std::size_t virt_addr) {
    for (std::size_t i = 0; i < count; i++) {
      ram.get_byte(virt_addr + i).set(buffer[i]);
    }
    regs.get_ref(0x0).set(virt_addr);
  }

  void load_from_buffer(const unsigned char* data, std::size_t count) {
      const std::size_t ram_addr = 0x200;
      const std::size_t ram_size = 0x9FF - 0x200;
      for (std::size_t i = 0; i < count && i < ram_size; i++) {
          ram.get_byte(ram_addr + i).set(data[i]);
      }
      regs.get_ref(0x0).set(ram_addr);
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
}

#endif //UNTITLED_EMULATOR_EMULATOR_H_

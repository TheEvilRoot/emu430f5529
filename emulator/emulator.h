//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_EMULATOR_EMULATOR_H_
#define UNTITLED_EMULATOR_EMULATOR_H_

#include <core/memoryView.h>
#include <core/pipeline.h>

#include <utils/tickController.h>

namespace emu {
class Emulator {
 private:
  core::MemoryView ram;
  core::MemoryView regs;

  core::Pipeline pipeline;

  TickController tick_controller;

 public:
  Emulator(): ram(0x10000), regs(0x10), pipeline(regs, ram), tick_controller(1000000) {
      ram.add_region(core::MemoryRegion{"special function registers", 0x0, 0xF, core::MemoryRegionAccess::ONLY_BYTE});
      ram.add_region(core::MemoryRegion{"8-bit peripheral", 0x10, 0xFF, core::MemoryRegionAccess::ONLY_BYTE});
      ram.add_region(core::MemoryRegion{"16-bit peripheral", 0x100, 0x1FF, core::MemoryRegionAccess::ONLY_WORD});
      ram.add_region(core::MemoryRegion{"ram", 0x200, 0x9FF, core::MemoryRegionAccess::WORD_BYTE});
  }

  void load_from_buffer(const unsigned char* data, std::size_t count) {
      const std::size_t ram_addr = 0x200;
      for (auto i = 0; i < count; i++) {
          ram.get_byte(ram_addr + i).set(data[i]);
      }
      regs.get_word(0x0).set(ram_addr);
  }

  [[noreturn]] void run() {
    while (true) {
      pipeline.step();
      tick_controller.tick_control_sleep();
    }
  }
};
}

#endif //UNTITLED_EMULATOR_EMULATOR_H_

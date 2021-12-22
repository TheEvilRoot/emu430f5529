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
  Emulator(): ram(0x10000), regs(0x10), pipeline(regs, ram), tick_controller(1000000) { }

  [[noreturn]] void run() {
    while (true) {
      pipeline.step();
      tick_controller.tick_control_sleep();
    }
  }
};
}

#endif //UNTITLED_EMULATOR_EMULATOR_H_

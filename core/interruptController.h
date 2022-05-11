//
// Created by user on 9.04.22.
//

#ifndef UNTITLED_INTERRUPTCONTROLLER_H
#define UNTITLED_INTERRUPTCONTROLLER_H

#include <atomic>
#include <queue>
#include <optional>

#include <core/memoryView.h>
#include <core/registerFile.h>
#include <utils/interrupts.h>

struct InterruptController {

    core::RegisterFile& regs;
    core::MemoryView& ram;

    std::queue<utils::Interrupt> queue;

    explicit InterruptController(core::RegisterFile& regs, core::MemoryView& ram): regs{regs}, ram{ram} {
    }

    [[nodiscard]] std::optional<utils::Interrupt> consume_interrupt() {
        if (queue.empty()) {
            return {};
        }
        const auto top = queue.front();
        queue.pop();
        return top;
    }

    bool generate_interrupt(utils::Interrupt interrupt) {
        const auto status = regs.get_ref(0x2).get();
        const auto gie = (status & 0x8) != 0;
        if (!gie) {
            spdlog::warn("ignore interrupt {} because GIE = 0", utils::interrupts::to_string(interrupt));
            return false;
        }
        if (!utils::interrupts::check_masking(interrupt, ram)) {
            spdlog::warn("ignore interrupt {} because of masking", utils::interrupts::to_string(interrupt));
            return false;
        }
        queue.push(interrupt);
        return true;
    }

};


#endif//UNTITLED_INTERRUPTCONTROLLER_H

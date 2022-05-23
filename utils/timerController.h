#ifndef UNTITLED_UNTILS_TIMER_CONTROLLER_H_
#define UNTITLED_UNTILS_TIMER_CONTROLLER_H_

#include <core/interruptController.h>
#include <core/memoryView.h>

#include <utils/interrupts.h>
#include <utils/timers.h>

namespace utils {
    struct TimerController {
        InterruptController& interrupt_controller;
        core::MemoryView& ram;

        explicit TimerController(InterruptController& interrupt_controller, core::MemoryView& ram): interrupt_controller{interrupt_controller}, ram{ram} {
        }

        template<typename Tm>
        auto tick_timer() {
            interrupt_controller.generate_interrupt<utils::ResetInterrupt, 0>();
        }

        template<typename ...Tms>
        auto tick_timers() {
            (tick_timer<Tms>(), ...);
        }

        auto tick_timers() {
            tick_timers<utils::timers::Wdt>();
        }
    };
}

#endif
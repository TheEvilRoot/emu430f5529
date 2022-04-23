//
// Created by user on 9.04.22.
//

#ifndef UNTITLED_INTERRUPTS_H
#define UNTITLED_INTERRUPTS_H

#include <variant>

namespace utils {
    struct interrupts {
        enum class InternalInterruptType {
            SINGLE_STEP,
            CONTINUE,
            BREAK,
            TERMINATE
        };
        struct InternalInterrupt {
            InternalInterruptType type;
        };

        typedef std::variant<InternalInterrupt> Interrupt;
    };
}

#endif//UNTITLED_INTERRUPTS_H

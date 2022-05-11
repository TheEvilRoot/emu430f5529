//
// Created by user on 9.04.22.
//

#ifndef UNTITLED_INTERRUPTS_H
#define UNTITLED_INTERRUPTS_H

#include <core/memoryView.h>

#include <variant>

namespace utils {

    struct Interrupt {
        constexpr static auto description = std::string_view{"Interrupt"};
        std::uint16_t vector_addr;
    };

    struct interrupts {
        static auto read_vector(const Interrupt& interrupt, core::MemoryView& ram) {
            const auto word_ref = ram.get_word(interrupt.vector_addr);
            return word_ref.get();
        }

        static auto check_masking(const Interrupt&, core::MemoryView&) {
            return true;
        }

        static auto to_string(const Interrupt&) noexcept {
            return Interrupt::description;
        }
    };
}

#endif//UNTITLED_INTERRUPTS_H

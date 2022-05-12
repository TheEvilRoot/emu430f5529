//
// Created by user on 9.04.22.
//

#ifndef UNTITLED_INTERRUPTCONTROLLER_H
#define UNTITLED_INTERRUPTCONTROLLER_H

#include <atomic>
#include <queue>
#include <optional>

#include <core/memoryView.h>
#include <core/memoryRef.h>
#include <core/registerFile.h>
#include <utils/interrupts.h>

struct InterruptController {

    core::RegisterFile& regs;
    core::MemoryView& ram;

    core::MemoryRef sr;

    explicit InterruptController(core::RegisterFile& regs, core::MemoryView& ram): regs{regs}, ram{ram}, sr{regs.get_ref(0x2)} {
    }

    template<typename Int>
    bool is_requested() {
        if constexpr (FlaggedInterrupt<Int>) {
            const auto is_any_requested = ram.get_byte(Int::ifg).get() != 0;
            return is_any_requested;
        }
        return false;
    }

    template<typename Int>
    void get_if_requested(std::optional<std::uint16_t>& ret) {
        if (!ret && is_requested<Int>()) {
            ret = std::optional<std::uint16_t>{Int::vector};
        }
    }

    template<typename ...Ints>
    std::optional<std::uint16_t> get_if_requested_with_priority() {
        std::optional<std::uint16_t> ret{};
        (get_if_requested<Ints>(ret), ...);
        return ret;
    }

    template<typename Int>
    void get_vector_label(std::uint16_t addr, std::optional<std::string>& ret) {
        if (!ret && Int::vector == addr) {
            ret = Int::description;
        }
    }

    template<typename ...Ints>
    std::optional<std::string> get_vector_label(std::uint16_t addr) {
        std::optional<std::string> ret{};
        (get_vector_label<Ints>(addr, ret), ...);
        return ret;
    }

    std::optional<std::string> get_vector_label(std::uint16_t addr) {
        return get_vector_label<utils::Port1Interrupt, utils::Port2Interrupt, utils::ResetInterrupt>(addr);
    }

    template<typename Int>
    auto get_vector_label(std::pair<std::uint16_t, std::string_view>& ret) {
        ret = std::pair<std::uint16_t, std::string_view>{ram.get_word(Int::vector).get(), Int::handler_description};
    }

    template<typename ...Ints>
    auto get_vectors_labels() {
        std::array<std::pair<std::uint16_t, std::string_view>, sizeof...(Ints)> ret;
        std::size_t index = 0;
        (get_vector_label<Ints>(ret[index++]), ...);
        return ret;
    }

    auto get_vectors_labels() {
        return get_vectors_labels<utils::ResetInterrupt, utils::Port1Interrupt, utils::Port2Interrupt>();
    }

    [[nodiscard]] std::optional<std::uint16_t> consume_interrupt() {
        const auto gie = (sr.get() & 0x8) != 0;
        if (!gie) {
            return std::optional<std::uint16_t>{};
        }
        const auto vector = get_if_requested_with_priority<utils::ResetInterrupt, utils::Port1Interrupt, utils::Port2Interrupt>();
        if (vector) {
            return ram.get_word(vector.value()).get();
        }
        return vector;
    }

    template<typename Int, std::uint16_t pin>
    bool generate_interrupt(bool is_rising_edge = false) {
        if constexpr (EnabledInterrupt<Int>) {
            const auto enabled = (ram.get_byte(Int::ie).get() & (1 << pin)) != 0;
            if (!enabled) {
                return false;
            }
        }
        if constexpr (InterruptWithEdgeSelection<Int>) {
            const auto edge_selection = (ram.get_byte(Int::ies).get() & (1 << pin)) != 0;
            if (!(edge_selection ^ is_rising_edge)) {
                return false;
            }
        }
        if constexpr (FlaggedInterrupt<Int>) {
            auto ref = ram.get_byte(Int::ifg);
            ref.set(ref.get() | (1 << pin));
        }
        return (sr.get() & 0x8) != 0;
    }
};


#endif//UNTITLED_INTERRUPTCONTROLLER_H

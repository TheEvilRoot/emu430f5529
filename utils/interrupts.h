//
// Created by user on 9.04.22.
//

#ifndef UNTITLED_INTERRUPTS_H
#define UNTITLED_INTERRUPTS_H

#include <core/memoryView.h>

#include <variant>

template<typename T>
concept FlaggedInterrupt = requires {
    { T::ifg };
};

template<typename T>
concept EnabledInterrupt = requires {
    { T::ie };
};

template<typename T>
concept InterruptWithEdgeSelection = requires {
    { T::ies };
};

namespace utils {
    struct Port1Interrupt {
        constexpr static auto description = std::string_view{"PORT1_VECTOR"};
        constexpr static auto handler_description = std::string_view{"PORT1_HANDLER"};
        constexpr static auto ie = std::uint16_t{0x021A};
        constexpr static auto ies = std::uint16_t{0x0218};
        constexpr static auto ifg = std::uint16_t{0x021C};
        constexpr static auto vector = std::uint16_t{0xFFDE};
    };

    struct Port2Interrupt {
        constexpr static auto description = std::string_view{"PORT2_VECTOR"};
        constexpr static auto handler_description = std::string_view{"PORT2_HANDLER"};
        constexpr static auto ie = std::uint16_t{0x021B};
        constexpr static auto ies = std::uint16_t{0x0219};
        constexpr static auto ifg = std::uint16_t{0x021D};
        constexpr static auto vector = std::uint16_t{0xFFD4};
    };

    struct ResetInterrupt {
        constexpr static auto description = std::string_view{"RESET_VECTOR"};
        constexpr static auto handler_description = std::string_view{"RESET_HANDLER"};
        constexpr static auto vector = std::uint16_t{0xFFFE};
    };
}

#endif//UNTITLED_INTERRUPTS_H

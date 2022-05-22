//
// Created by user on 18.05.22.
//

#ifndef UNTITLED_PORTS_H
#define UNTITLED_PORTS_H

#include <cstdint>
#include <string_view>

#include <core/memoryView.h>

namespace utils {
    struct ports {
        struct P1 {
            struct In {
                constexpr static auto label = std::string_view{"P1IN"};
                constexpr static auto addr = std::uint16_t{0x0200};
            };
            struct Out {
                constexpr static auto label = std::string_view{"P1OUT"};
                constexpr static auto addr = std::uint16_t{0x0202};
            };
            struct Dir {
                constexpr static auto label = std::string_view{"P1DIR"};
                constexpr static auto addr = std::uint16_t{0x0204};
            };
            struct Ren {
                constexpr static auto label = std::string_view{"P1REN"};
                constexpr static auto addr = std::uint16_t{0x0206};
            };
            struct Ds {
                constexpr static auto label = std::string_view{"P1DS"};
                constexpr static auto addr = std::uint16_t{0x0208};
            };
            struct Sel {
                constexpr static auto label = std::string_view{"P1SEL"};
                constexpr static auto addr = std::uint16_t{0x020A};
            };
            struct Iv {
                constexpr static auto label = std::string_view{"P1IV"};
                constexpr static auto addr = std::uint16_t{0x020E};
            };
            struct Ies {
                constexpr static auto label = std::string_view{"P1IES"};
                constexpr static auto addr = std::uint16_t{0x0218};
            };
            struct Ie {
                constexpr static auto label = std::string_view{"P1IE"};
                constexpr static auto addr = std::uint16_t{0x021A};
            };
            struct Ifg {
                constexpr static auto label = std::string_view{"P1FG"};
                constexpr static auto addr = std::uint16_t{0x021C};
            };
        };
        struct P2 {
            struct In {
                constexpr static auto label = std::string_view{"P2IN"};
                constexpr static auto addr = std::uint16_t{0x0201};
            };
            struct Out {
                constexpr static auto label = std::string_view{"P2OUT"};
                constexpr static auto addr = std::uint16_t{0x0203};
            };
            struct Dir {
                constexpr static auto label = std::string_view{"P2DIR"};
                constexpr static auto addr = std::uint16_t{0x0205};
            };
            struct Ren {
                constexpr static auto label = std::string_view{"P2REN"};
                constexpr static auto addr = std::uint16_t{0x0207};
            };
            struct Ds {
                constexpr static auto label = std::string_view{"P2DS"};
                constexpr static auto addr = std::uint16_t{0x0209};
            };
            struct Sel {
                constexpr static auto label = std::string_view{"P2SEL"};
                constexpr static auto addr = std::uint16_t{0x020B};
            };
            struct Iv {
                constexpr static auto label = std::string_view{"P2IV"};
                constexpr static auto addr = std::uint16_t{0x021E};
            };
            struct Ies {
                constexpr static auto label = std::string_view{"P2IES"};
                constexpr static auto addr = std::uint16_t{0x0219};
            };
            struct Ie {
                constexpr static auto label = std::string_view{"P2IE"};
                constexpr static auto addr = std::uint16_t{0x021B};
            };
            struct Ifg {
                constexpr static auto label = std::string_view{"P2FG"};
                constexpr static auto addr = std::uint16_t{0x021D};
            };
        };

        template<typename P>
        constexpr static auto get_byte(const core::MemoryView& ram) {
            return ram.get_byte(P::addr).get();
        }

        template<typename ...Ts>
        constexpr static auto for_each(const core::MemoryView& ram, const auto& func) requires (sizeof...(Ts) > 1) {
            (func(Ts::label, Ts::addr, get_byte<Ts>(ram)), ...);
        }

        template<typename ...Ps>
        constexpr static auto for_each_port(const core::MemoryView& ram, const auto& func) requires (sizeof...(Ps) > 1) {
            (for_each<typename Ps::In,
                      typename Ps::Out,
                      typename Ps::Dir,
                      typename Ps::Ren,
                      typename Ps::Ds,
                      typename Ps::Sel,
                      typename Ps::Iv,
                      typename Ps::Ies,
                      typename Ps::Ie,
                      typename Ps::Ifg>(ram, func), ...);
        }

        constexpr static auto for_each_port(const core::MemoryView& ram, const auto& func) {
            for_each_port<P1, P2>(ram, func);
        }
    };
}

#endif//UNTITLED_PORTS_H

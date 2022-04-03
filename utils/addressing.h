//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_UTILS_ADDRESSING_H_
#define UNTITLED_UTILS_ADDRESSING_H_

#include <core/memoryRef.h>
#include <core/memoryView.h>
#include <core/registerFile.h>
#include <utils/utils.h>

#include <variant>

namespace msp {
    struct RegisterDirectAddressing {
        std::uint16_t reg;
    };

    struct RegisterIndexedAddressing {
        std::uint16_t reg;
        core::MemoryRefType ref_type;
    };

    struct RegisterIndirectAddressing {
        std::uint16_t reg;
        std::uint16_t delta;
        core::MemoryRefType ref_type;
    };

    typedef std::variant<RegisterDirectAddressing, RegisterIndexedAddressing, RegisterIndirectAddressing>
            Addressing;

    struct addressing {

        static Addressing from_source(std::uint16_t reg, std::uint16_t mode, std::uint16_t bw) noexcept {
            const auto ref_type = bw == 0 ? core::MemoryRefType::WORD : core::MemoryRefType::BYTE;
            switch (mode) {
                case 0x00:
                    return RegisterDirectAddressing{reg};
                case 0x01:
                    return RegisterIndexedAddressing{reg, ref_type};
                case 0x02:
                    return RegisterIndirectAddressing{reg, 0x0, ref_type};
                case 0x03:
                    return RegisterIndirectAddressing{
                            .reg = reg,
                            .delta = std::uint16_t(ref_type == core::MemoryRefType::BYTE ? 1u : 2u),
                            .ref_type = ref_type};
                default:
                    assert(false);
            }
        }

        static Addressing from_destination(std::uint16_t reg, std::uint16_t mode, std::uint16_t bw) {
            const auto ref_type = bw == 0 ? core::MemoryRefType::WORD : core::MemoryRefType::BYTE;
            switch (mode) {
                case 0x0:
                    return RegisterDirectAddressing{reg};
                case 0x1:
                    return RegisterIndexedAddressing{reg, ref_type};
                default:
                    assert(false);
            }
        }

        [[nodiscard]] static core::MemoryRef get_ref(const RegisterDirectAddressing &addr, core::MemoryRef & /*pc*/, core::RegisterFile &regs, core::MemoryView & /*ram*/) noexcept {
            return regs.get_ref(addr.reg);
        }

        [[nodiscard]] static core::MemoryRef get_ref(const RegisterIndexedAddressing &addr, core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) noexcept {
            const auto next_word = ram.get_word(pc.get_and_increment(0x2)).get();
            const auto base = regs.get_ref(addr.reg).get();
            switch (addr.ref_type) {
                case core::MemoryRefType::BYTE:
                    return ram.get_byte(base + next_word);
                case core::MemoryRefType::WORD:
                    return ram.get_word(base + next_word);
            }
        }

        [[nodiscard]] static core::MemoryRef get_ref(const RegisterIndirectAddressing &addr, core::MemoryRef & /*pc*/, core::RegisterFile &regs, core::MemoryView &ram) noexcept {
            const auto address = regs.get_ref(addr.reg).get_and_increment(addr.delta);
            switch (addr.ref_type) {
                case core::MemoryRefType::BYTE:
                    return ram.get_byte(address);
                case core::MemoryRefType::WORD:
                    return ram.get_word(address);
            }
        }

        [[nodiscard]] static std::string to_string(const RegisterDirectAddressing &addr) noexcept {
            if (addr.reg == 0)
                return "PC";
            return "R" + std::to_string(addr.reg);
        }

        [[nodiscard]] static std::string to_string(const RegisterIndexedAddressing &addr) noexcept {
            return "x(R" + std::to_string(addr.reg) + ")";
        }

        [[nodiscard]] static std::string to_string(const RegisterIndirectAddressing &addr) noexcept {
            return "@R" + std::to_string(addr.reg);
        }

        [[nodiscard]] static core::MemoryRef get_ref(const Addressing &addr, core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) noexcept {
            return std::visit(overloaded{
                                      [&pc, &regs, &ram](const auto &a) { return get_ref(a, pc, regs, ram); }},
                              addr);
        }

        [[nodiscard]] static std::string to_string(const Addressing &addr) noexcept {
            return std::visit(overloaded{
                                      [](const auto &a) { return to_string(a); }},
                              addr);
        }
    };

}// namespace msp

#endif//UNTITLED_UTILS_ADDRESSING_H_

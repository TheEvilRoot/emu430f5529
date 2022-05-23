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

    struct Absolute {
    };

    struct ConstantAddressing {
        std::uint16_t value;
        std::uint16_t constant_reg;
    };

    struct Label {
        core::MemoryRefType ref_type;
    };

    typedef std::variant<RegisterDirectAddressing, RegisterIndexedAddressing, RegisterIndirectAddressing, ConstantAddressing, Absolute, Label>
            Addressing;

    struct addressing {

        static Addressing from_source(std::uint16_t reg, std::uint16_t mode, std::uint16_t bw) noexcept {
            const auto ref_type = bw == 0 ? core::MemoryRefType::WORD : core::MemoryRefType::BYTE;
            if (reg == 3) {
                constexpr static std::uint16_t constant_value[4] = {0x0, 0x1, 0x2, static_cast<std::uint16_t>(-1)};
                constexpr static std::uint16_t constant_reg[4] = {0x0, 0x2, 0x4, 0xA};
                return ConstantAddressing{constant_value[mode], constant_reg[mode]};
            } else if (reg == 2) {
                if (mode == 1) {
                    return Label{ref_type};
                } else if (mode > 1) {
                    constexpr static std::uint16_t constant_value[2] = {0x4, 0x8};
                    constexpr static std::uint16_t constant_reg[2] = {0x6, 0x8};
                    return ConstantAddressing{constant_value[mode - 2], constant_reg[mode - 2]};
                }
            }
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
                    return RegisterDirectAddressing{0}; // impossible choice
            }
        }

        static Addressing from_destination(std::uint16_t reg, std::uint16_t mode, std::uint16_t bw) {
            const auto ref_type = bw == 0 ? core::MemoryRefType::WORD : core::MemoryRefType::BYTE;
            switch (mode) {
                case 0x0:
                    return RegisterDirectAddressing{reg};
                case 0x1:
                    if (reg == 0x2) {
                        return Label{ref_type};
                    }
                    return RegisterIndexedAddressing{reg, ref_type};
                default:
                    return RegisterDirectAddressing{0}; // impossible choice
            }
        }

        [[nodiscard]] static core::MemoryRef get_ref(const Absolute &/* addr */, core::MemoryRef &pc, core::RegisterFile & /* regs */, core::MemoryView &ram) noexcept {
            return ram.get_word(pc.get_and_increment(0x2));
        }

        [[nodiscard]] static core::MemoryRef get_ref(const ConstantAddressing &addr, core::MemoryRef & /*pc*/, core::RegisterFile &regs, core::MemoryView & /*ram*/) noexcept {
            return regs.constants.get_word(addr.constant_reg);
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
            if (addr.reg == 0) {
                const auto address = regs.get_ref(addr.reg).get_and_increment(0x2);
                return ram.get_word(address);
            }
            const auto address = regs.get_ref(addr.reg).get_and_increment(addr.delta);
            switch (addr.ref_type) {
                case core::MemoryRefType::BYTE:
                    return ram.get_byte(address);
                case core::MemoryRefType::WORD:
                    return ram.get_word(address);
            }
        }

        [[nodiscard]] static core::MemoryRef get_ref(const Label &addr, core::MemoryRef &pc, core::RegisterFile &/*regs*/, core::MemoryView &ram) noexcept {
            const auto next_word = ram.get_word(pc.get_and_increment(0x2)).get();
            switch (addr.ref_type) {
                case core::MemoryRefType::BYTE:
                    return ram.get_byte(next_word);
                case core::MemoryRefType::WORD:
                    return ram.get_word(next_word);
            }
        }

        [[nodiscard]] static std::string reg_to_string(std::uint16_t reg) noexcept {
            if (reg == 0)
                return "PC";
            if (reg == 1)
                return "SP";
            if (reg == 2)
                return "SR";
            return "R" + std::to_string(reg);
        }

        [[nodiscard]] static std::string to_string(const Label &/*addr*/) noexcept {
            return "&LABEL";
        }

        [[nodiscard]] static std::string to_string(const Label &/*addr*/, const core::MemoryRef& ref) noexcept {
            return fmt::format("&{:04X}", ref.offset);
        }

        [[nodiscard]] static std::string to_string(const Absolute &/*addr*/) noexcept {
            return "&LABEL";
        }

        [[nodiscard]] static std::string to_string(const Absolute &/*addr*/, const core::MemoryRef& ref) noexcept {
            if (ref.offset > 0xFFFF)
                return "&INVALID_REF";
            return fmt::format("&{:4X}", ref.offset);
        }

        [[nodiscard]] static std::string to_string(const ConstantAddressing &addr) noexcept {
            return "#" + std::to_string(addr.value);
        }

        [[nodiscard]] static std::string to_string(const ConstantAddressing &addr, const core::MemoryRef& /*ref*/) noexcept {
            return "#" + std::to_string(addr.value);
        }

        [[nodiscard]] static std::string to_string(const RegisterDirectAddressing &addr) noexcept {
            return reg_to_string(addr.reg);
        }

        [[nodiscard]] static std::string to_string(const RegisterDirectAddressing &addr, const core::MemoryRef& /*ref*/) noexcept {
            return reg_to_string(addr.reg);
        }

        [[nodiscard]] static std::string to_string(const RegisterIndexedAddressing &addr) noexcept {
            return "x(" + reg_to_string(addr.reg) + ")";
        }

        [[nodiscard]] static std::string to_string(const RegisterIndexedAddressing &addr, const core::MemoryRef& ref) noexcept {
            return fmt::format("{}+{:04X}", reg_to_string(addr.reg), ref.offset);
        }

        [[nodiscard]] static std::string to_string(const RegisterIndirectAddressing &addr) noexcept {
            if (addr.delta > 0) {
                return "@" + reg_to_string(addr.reg) + "+" + std::to_string(addr.delta);
            }
            return "@" + reg_to_string(addr.reg);
        }

        [[nodiscard]] static std::string to_string(const RegisterIndirectAddressing &addr, const core::MemoryRef& ref) noexcept {
            if (addr.reg == 0) {
                return fmt::format("#{:04X}", ref.get());
            }
            if (addr.delta > 0) {
                return "@" + reg_to_string(addr.reg) + "+" + std::to_string(addr.delta);
            }
            return "@" + reg_to_string(addr.reg);
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

        [[nodiscard]] static std::string to_string(const Addressing &addr, const core::MemoryRef& ref) noexcept {
            return std::visit(overloaded{
                                      [&ref](const auto &a) { return to_string(a, ref); }},
                              addr);
        }
    };

}// namespace msp

#endif//UNTITLED_UTILS_ADDRESSING_H_

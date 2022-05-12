//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_UTILS_INSTRUCTIONS_H_
#define UNTITLED_UTILS_INSTRUCTIONS_H_

#include <core/registerFile.h>
#include <utils/opcodes.h>

#include <memory>

#include <spdlog/spdlog.h>

namespace msp {

    struct FlaggedResult {
        std::uint16_t value;
        std::uint16_t set_flags;
        std::uint16_t reset_flags;


         // implicit for a reason
        FlaggedResult(std::uint16_t v): value{v}, set_flags{0}, reset_flags{0} {
        }

        FlaggedResult(std::uint16_t v, std::uint16_t set, std::uint16_t reset): value{v}, set_flags{set}, reset_flags{reset} {
        }
    };

    struct UnaryInstruction {
        UnaryInstructionOpcode opcode;
        Addressing source_addressing;
        static FlaggedResult calculate(UnaryInstructionOpcode opcode, std::uint16_t value, std::uint16_t status) {
            switch (opcode.value) {
                case UnaryInstructionOpcode::RRC: {
                    std::uint16_t result = value >> 1;
                    if ((status & 0x1) != 0) result |= 0x8000; else result &= ~0x8000;
                    const bool c = (value & 0x1) != 0;
                    const bool n = (result & 0x8000) != 0;
                    const bool z = result == 0;
                    const bool v = false;

                    FlaggedResult fr{result, 0x0, 0x0};
                    fr.set_flags |= (c ? 0x1 : 0x0) | (z ? 0x2 : 0x0) | (n ? 0x4 : 0x0) | (v ? 0x80 : 0);
                    fr.reset_flags |= (c ? 0x0 : 0x1) | (z ? 0x0 : 0x2) | (n ? 0x0 : 0x4) | (v ? 0x0 : 0x80);
                    return fr;
                }
                case UnaryInstructionOpcode::RRA: {
                    std::uint16_t result = value >> 1;
                    result |= (value & 0x8000);
                    const bool c = (value & 0x1) != 0;
                    const bool n = (result & 0x8000) != 0;
                    const bool z = result == 0;
                    const bool v = false;

                    FlaggedResult fr{result, 0x0, 0x0};
                    fr.set_flags |= (c ? 0x1 : 0x0) | (z ? 0x2 : 0x0) | (n ? 0x4 : 0x0) | (v ? 0x80 : 0);
                    fr.reset_flags |= (c ? 0x0 : 0x1) | (z ? 0x0 : 0x2) | (n ? 0x0 : 0x4) | (v ? 0x0 : 0x80);
                    return fr;
                }
                case UnaryInstructionOpcode::SWPB:
                    return ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
                case UnaryInstructionOpcode::SXT: {
                    std::uint16_t result = value & 0xFF;
                    const auto sign = (value & 0x80) != 0;
                    if (sign) result |= 0xFF00; else result &= ~0xFF00;
                    const bool n = (result & 0x8000) != 0;
                    const bool z = result == 0;
                    const bool c = result != 0;
                    const bool v = false;

                    FlaggedResult fr{value, 0x0, 0x0};
                    fr.set_flags |= (c ? 0x1 : 0x0) | (z ? 0x2 : 0x0) | (n ? 0x4 : 0x0) | (v ? 0x80 : 0);
                    fr.reset_flags |= (c ? 0x0 : 0x1) | (z ? 0x0 : 0x2) | (n ? 0x0 : 0x4) | (v ? 0x0 : 0x80);
                    return fr;
                }
                case UnaryInstructionOpcode::PUSH:
                    return value;
                case UnaryInstructionOpcode::CALL:
                    return value;
                case UnaryInstructionOpcode::RETI:
                    return value;
                default:
                    assert(false);
            }
        }
    };

    struct JumpInstruction {
        JumpInstructionOpcode condition;
        std::int16_t signed_offset;

        static FlaggedResult calculate(std::uint16_t pc, std::uint16_t signed_offset) noexcept {
            return pc + signed_offset;
        }

        static bool check_condition(JumpInstructionOpcode cond, std::uint16_t status) noexcept {
            switch (cond.value) {
                case JumpInstructionOpcode::JNE_JNZ:
                    return (status & 0x2) == 0;
                case JumpInstructionOpcode::JEQ_JZ:
                    return (status & 0x2) != 0;
                case JumpInstructionOpcode::JNC_JLO:
                    return (status & 0x1) == 0;
                case JumpInstructionOpcode::JC_JHS:
                    return (status & 0x1) != 0;
                case JumpInstructionOpcode::JN:
                    return (status & 0x4) != 0;
                case JumpInstructionOpcode::JGE:
                    return (status & 0x4) == (status & 0x80);
                case JumpInstructionOpcode::JL:
                    return (status & 0x4) != (status & 0x80);
                case JumpInstructionOpcode::JMP:
                    return true;
            }
        }

        static std::int16_t unsigned_to_signed_offset(std::uint16_t unsigned_offset) noexcept {
            const std::uint16_t carry = (unsigned_offset) & 0x200;
            if (carry == 0) {
                return static_cast<std::int16_t>(unsigned_offset & 0x1FFu) * static_cast<std::int16_t>(2);
            }
            const std::uint16_t value = unsigned_offset & 0x1FFu;
            const std::uint16_t negated = value | 0xFE00u;
            return static_cast<std::int16_t>(negated) * static_cast<std::int16_t>(2);
        }
    };

    struct BinaryInstruction {
        BinaryInstructionOpcode opcode;
        Addressing source_addressing;
        Addressing destination_addressing;

        static FlaggedResult calculate(BinaryInstructionOpcode opcode, std::uint16_t source, std::uint16_t dest, std::uint16_t status) {
            spdlog::debug("calculate {:X} {:s} {:X}", source, BinaryInstructionOpcode::to_string(opcode), dest);
            switch (opcode.value) {
                case BinaryInstructionOpcode::ADD: {
                    const std::uint16_t result = source + dest;
                    const bool sourceNegative = (source & 0x8000) != 0;
                    const bool destNegative = (dest & 0x8000) != 0;
                    const bool resultNegative = (result & 0x8000) != 0;
                    const bool v = (sourceNegative && destNegative && !resultNegative) || (!sourceNegative && !destNegative && resultNegative);
                    const bool n = resultNegative;
                    const bool z = result == 0;
                    const bool c = false; // what is produce carry?

                    FlaggedResult fr{result, 0x0, 0x0};
                    fr.set_flags |= (c ? 0x1 : 0x0) | (z ? 0x2 : 0x0) | (n ? 0x4 : 0x0) | (v ? 0x80 : 0);
                    fr.reset_flags |= (c ? 0x0 : 0x1) | (z ? 0x0 : 0x2) | (n ? 0x0 : 0x4) | (v ? 0x0 : 0x80);
                    return fr;
                }
                case BinaryInstructionOpcode::ADDC: {
                    const bool carry = (status & 0x1) != 0;
                    const std::uint16_t result = source + dest + (carry ? 0x1 : 0x0);
                    const bool sourceNegative = (source & 0x8000) != 0;
                    const bool destNegative = (dest & 0x8000) != 0;
                    const bool resultNegative = (result & 0x8000) != 0;
                    const bool v = (sourceNegative && destNegative && !resultNegative) || (!sourceNegative && !destNegative && resultNegative);
                    const bool n = resultNegative;
                    const bool z = result == 0;
                    const bool c = false; // what is produce carry?

                    FlaggedResult fr{result, 0x0, 0x0};
                    fr.set_flags |= (c ? 0x1 : 0x0) | (z ? 0x2 : 0x0) | (n ? 0x4 : 0x0) | (v ? 0x80 : 0);
                    fr.reset_flags |= (c ? 0x0 : 0x1) | (z ? 0x0 : 0x2) | (n ? 0x0 : 0x4) | (v ? 0x0 : 0x80);
                    return fr;
                }
                case BinaryInstructionOpcode::AND: {
                    const std::uint16_t result = source & dest;

                    const bool n = (result & 0x8000) != 0;
                    const bool z = result == 0;
                    const bool c = result != 0;
                    const bool v = false;

                    FlaggedResult fr{result, 0x0, 0x0};
                    fr.set_flags |= (c ? 0x1 : 0x0) | (z ? 0x2 : 0x0) | (n ? 0x4 : 0x0) | (v ? 0x80 : 0);
                    fr.reset_flags |= (c ? 0x0 : 0x1) | (z ? 0x0 : 0x2) | (n ? 0x0 : 0x4) | (v ? 0x0 : 0x80);
                    return fr;
                }
                case BinaryInstructionOpcode::SUB: {
                    const std::uint16_t result = dest - source;
                    const bool sourceNegative = (source & 0x8000) != 0;
                    const bool destNegative = (dest & 0x8000) != 0;
                    const bool resultNegative = (result & 0x8000) != 0;
                    const bool v = (!destNegative && sourceNegative && resultNegative) || (destNegative && !sourceNegative && !resultNegative);
                    const bool n = resultNegative;
                    const bool z = result == 0;
                    const bool c = false; // what is produce carry?

                    FlaggedResult fr{result, 0x0, 0x0};
                    fr.set_flags |= (c ? 0x1 : 0x0) | (z ? 0x2 : 0x0) | (n ? 0x4 : 0x0) | (v ? 0x80 : 0);
                    fr.reset_flags |= (c ? 0x0 : 0x1) | (z ? 0x0 : 0x2) | (n ? 0x0 : 0x4) | (v ? 0x0 : 0x80);
                    return fr;
                }
                case BinaryInstructionOpcode::SUBC: {
                    const bool carry = (status & 0x1) != 0;
                    const std::uint16_t result = dest - source + (carry ? 0x1 : 0x0);
                    const bool sourceNegative = (source & 0x8000) != 0;
                    const bool destNegative = (dest & 0x8000) != 0;
                    const bool resultNegative = (result & 0x8000) != 0;
                    const bool v = (!destNegative && sourceNegative && resultNegative) || (destNegative && !sourceNegative && !resultNegative);
                    const bool n = resultNegative;
                    const bool z = result == 0;
                    const bool c = false; // what is produce carry?

                    FlaggedResult fr{result, 0x0, 0x0};
                    fr.set_flags |= (c ? 0x1 : 0x0) | (z ? 0x2 : 0x0) | (n ? 0x4 : 0x0) | (v ? 0x80 : 0);
                    fr.reset_flags |= (c ? 0x0 : 0x1) | (z ? 0x0 : 0x2) | (n ? 0x0 : 0x4) | (v ? 0x0 : 0x80);
                    return fr;
                }
                case BinaryInstructionOpcode::XOR: {
                    const std::uint16_t result = source ^ dest;
                    const bool n = (result & 0x8000) != 0;
                    const bool z = result == 0;
                    const bool c = result != 0;
                    const bool v = ((source & 0x8000) != 0) && ((dest & 0x8000) != 0);

                    FlaggedResult fr{result, 0x0, 0x0};
                    fr.set_flags |= (c ? 0x1 : 0x0) | (z ? 0x2 : 0x0) | (n ? 0x4 : 0x0) | (v ? 0x80 : 0);
                    fr.reset_flags |= (c ? 0x0 : 0x1) | (z ? 0x0 : 0x2) | (n ? 0x0 : 0x4) | (v ? 0x0 : 0x80);
                    return fr;
                }
                case BinaryInstructionOpcode::MOV:
                    return source;
                case BinaryInstructionOpcode::CMP: {
                    const std::uint16_t result = dest - source;
                    const bool sourceNegative = (source & 0x8000) != 0;
                    const bool destNegative = (dest & 0x8000) != 0;
                    const bool resultNegative = (result & 0x8000) != 0;
                    const bool v = (!destNegative && sourceNegative && resultNegative) || (destNegative && !sourceNegative && !resultNegative);
                    const bool n = resultNegative;
                    const bool z = result == 0;
                    const bool c = false; // what is produce carry?

                    FlaggedResult fr{dest, 0x0, 0x0};
                    fr.set_flags |= (c ? 0x1 : 0x0) | (z ? 0x2 : 0x0) | (n ? 0x4 : 0x0) | (v ? 0x80 : 0);
                    fr.reset_flags |= (c ? 0x0 : 0x1) | (z ? 0x0 : 0x2) | (n ? 0x0 : 0x4) | (v ? 0x0 : 0x80);
                    return fr;
                }
                case BinaryInstructionOpcode::DADD:
                    return dest + source;
                case BinaryInstructionOpcode::BIT: {
                    const std::uint16_t result = dest & source;
                    const bool n = (result & 0x8000) != 0;
                    const bool z = result == 0;
                    const bool c = result != 0;
                    const bool v = false;

                    FlaggedResult fr{dest, 0x0, 0x0};
                    fr.set_flags |= (c ? 0x1 : 0x0) | (z ? 0x2 : 0x0) | (n ? 0x4 : 0x0) | (v ? 0x80 : 0);
                    fr.reset_flags |= (c ? 0x0 : 0x1) | (z ? 0x0 : 0x2) | (n ? 0x0 : 0x4) | (v ? 0x0 : 0x80);
                    return fr;
                }
                case BinaryInstructionOpcode::BIC:
                    return dest & (~source);
                case BinaryInstructionOpcode::BIS:
                    return dest | source;
            }
        }
    };

    struct UnimplementedInstruction {
        std::uint16_t word;
    };

    typedef std::variant<BinaryInstruction, UnaryInstruction, JumpInstruction, UnimplementedInstruction> Instruction;

    struct BinaryInstructionDetail {
        std::string source;
        std::string dest;
    };

    struct UnaryInstructionDetail {
        std::string source;
    };

    struct JumpInstructionDetail {
        std::uint16_t jump_addr;
        std::int16_t jump_offset;
    };

    struct UnimplementedInstructionDetail {
        std::uint16_t pc;
        std::uint16_t word;
    };

    typedef std::variant<BinaryInstructionDetail, UnaryInstructionDetail, JumpInstructionDetail, UnimplementedInstructionDetail> InstructionDetail;

    struct instruction {

        static std::uint16_t apply_status(std::uint16_t flags, std::uint16_t set_flags, std::uint16_t reset_flags) {
            return (flags | set_flags) & (~reset_flags);
        }

        static void execute(const JumpInstruction &ins, core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView & /*ram*/) noexcept {
            const auto status_value = regs.get_ref(0x2).get();
            if (JumpInstruction::check_condition(ins.condition, status_value)) {
                const auto result = JumpInstruction::calculate(pc.get(), ins.signed_offset);
                pc.set(result.value);
            }
        }

        inline static InstructionDetail decompile(const JumpInstruction &ins, core::MemoryRef &pc, core::RegisterFile &/* regs */, core::MemoryView & /*ram*/) noexcept {
            const auto result = JumpInstruction::calculate(pc.get(), ins.signed_offset);
            return JumpInstructionDetail{result.value, ins.signed_offset};
        }

        static void execute(const UnaryInstruction &ins, core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) noexcept {
            auto status_ref = regs.get_ref(0x2);
            auto source_ref = addressing::get_ref(ins.source_addressing, pc, regs, ram);
            auto status_value = status_ref.get();
            const auto source_value = source_ref.get();
            const auto result = UnaryInstruction::calculate(ins.opcode, source_value, status_ref.get());
            const auto res_value = result.value;
            if (ins.opcode.value == UnaryInstructionOpcode::CALL) {
                const auto dst = source_ref.get();
                auto sp = regs.get_ref(0x1);
                sp.get_and_increment(-0x2);
                auto stack = ram.get_word(sp.get());
                stack.set(pc.get());
                pc.set(dst);
            } else if (ins.opcode.value == UnaryInstructionOpcode::RETI) {
                auto sp = regs.get_ref(0x1);
                // pop sr
                const auto sr_pop = ram.get_word(sp.get_and_increment(0x2)).get();
                status_ref.set(sr_pop);
                // pop pc
                const auto pc_pop = ram.get_word(sp.get_and_increment(0x2)).get();
                pc.set(pc_pop);

                status_value = sr_pop;
            } else if (ins.opcode.value == UnaryInstructionOpcode::PUSH) {
                auto sp = regs.get_ref(0x1);
                sp.get_and_increment(-0x2);
                auto stack = ram.get_word(sp.get());
                stack.set(source_value);
            } else {
                if (res_value != source_value) {
                    source_ref.set(res_value);
                }
            }
            const auto new_status = apply_status(status_value, result.set_flags, result.reset_flags);
            status_ref.set(new_status);
        }

        inline static InstructionDetail decompile(const UnaryInstruction &ins, core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) noexcept {
            auto source_ref = addressing::get_ref(ins.source_addressing, pc, regs, ram);
            return UnaryInstructionDetail{addressing::to_string(ins.source_addressing, source_ref)};
        }

        static void execute(const BinaryInstruction &ins, core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) noexcept {
            auto status_ref = regs.get_ref(0x2);
            auto source_ref = addressing::get_ref(ins.source_addressing, pc, regs, ram);
            auto dst_ref = addressing::get_ref(ins.destination_addressing, pc, regs, ram);

            spdlog::debug("execute {:s} = {:X}, {:s} = {:X}", addressing::to_string(ins.source_addressing),
                          source_ref.get(), addressing::to_string(ins.destination_addressing), dst_ref.get());
            const auto status_value = status_ref.get();
            const auto result = BinaryInstruction::calculate(ins.opcode, source_ref.get(), dst_ref.get(), status_value);
            spdlog::debug("write-back {:X} => {:s}", result.value, addressing::to_string(ins.destination_addressing));
            dst_ref.set(result.value);
            const auto new_status = apply_status(status_value, result.set_flags, result.reset_flags);
            status_ref.set(new_status);
        }

        inline static InstructionDetail decompile(const BinaryInstruction &ins, core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) noexcept {
            auto source_ref = addressing::get_ref(ins.source_addressing, pc, regs, ram);
            auto dst_ref = addressing::get_ref(ins.destination_addressing, pc, regs, ram);
            return BinaryInstructionDetail{addressing::to_string(ins.source_addressing, source_ref), addressing::to_string(ins.destination_addressing, dst_ref)};
        }

        [[nodiscard]] static std::string to_string(const UnaryInstruction &ins) noexcept {
            return UnaryInstructionOpcode::to_string(ins.opcode) + " " + addressing::to_string(ins.source_addressing);
        }

        [[nodiscard]] static std::string to_string(const BinaryInstruction &ins) noexcept {
            return BinaryInstructionOpcode::to_string(ins.opcode) + " " + addressing::to_string(ins.source_addressing) +
                   " " + addressing::to_string(ins.destination_addressing);
        }

        [[nodiscard]] static std::string to_string(const JumpInstruction &ins) noexcept {
            return JumpInstructionOpcode::to_string(ins.condition) + " +(" + std::to_string(ins.signed_offset) + ")";
        }

        [[nodiscard]] static std::string to_string(const BinaryInstructionDetail& detail) {
            return fmt::format("{} {}", detail.source, detail.dest);
        }

        [[nodiscard]] static std::string to_string(const UnaryInstructionDetail& detail) {
            return detail.source;
        }

        [[nodiscard]] static std::string to_string(const JumpInstructionDetail& detail) {
            return fmt::format("{:04X} <{}{:04X}>", detail.jump_addr, detail.jump_offset >= 0 ? '+' : '-', abs(detail.jump_offset));
        }

        [[nodiscard]] static std::string to_string(const Instruction &ins) noexcept {
            return std::visit(overloaded{
                                      [](const UnimplementedInstruction& i) { return fmt::format("Unimplemented instruction {:04X}", i.word); },
                                      [](const auto &i) { return to_string(i); }},
                              ins);
        }

        [[nodiscard]] static std::string opcode_to_string(const Instruction& ins) noexcept {
            return std::visit(overloaded{
                    [](const BinaryInstruction& bi) { return BinaryInstructionOpcode::to_string(bi.opcode); },
                    [](const UnaryInstruction& ui) { return UnaryInstructionOpcode::to_string(ui.opcode); },
                    [](const JumpInstruction& ji) { return JumpInstructionOpcode::to_string(ji.condition); },
                    [](const UnimplementedInstruction& i) { return fmt::format("Unimplemented instruction {:04x}", i.word); }
            }, ins);
        }

        [[nodiscard]] static std::string to_string(const InstructionDetail& detail) {
            return std::visit(overloaded{
                                      [](const UnimplementedInstructionDetail& d) { return fmt::format("Unimplemented instruction {:04X} on {:04X}", d.word, d.pc); },
                                      [](const auto &i) { return to_string(i); }},
                              detail);
        }

        static void execute(const Instruction &ins, core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) noexcept {
            std::visit(overloaded{
                               [](const UnimplementedInstruction&) { },
                               [&pc, &regs, &ram](const auto &i) { execute(i, pc, regs, ram); }}, ins);
        }

        static InstructionDetail decompile(const Instruction &ins, core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) noexcept {
            return std::visit(overloaded{
                               [&pc](const UnimplementedInstruction& i) { return InstructionDetail{UnimplementedInstructionDetail{i.word, pc.get()}}; },
                               [&pc, &regs, &ram](const auto &i) { return decompile(i, pc, regs, ram); }}, ins);
        }
    };
}// namespace msp

#endif//UNTITLED_UTILS_INSTRUCTIONS_H_

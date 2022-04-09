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

    struct UnaryInstruction {
        UnaryInstructionOpcode opcode;
        Addressing source_addressing;
        static std::uint16_t calculate(UnaryInstructionOpcode opcode, std::uint16_t value) {
            switch (opcode.value) {
                case UnaryInstructionOpcode::RRC:
                    return value >> 1;
                case UnaryInstructionOpcode::RRA:
                    return value >> 1;
                case UnaryInstructionOpcode::SWPB:
                    return ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
                case UnaryInstructionOpcode::SXT:
                    return value;
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
        static std::uint16_t calculate(std::uint16_t pc, std::uint16_t signed_offset) noexcept {
            return pc + signed_offset;
        }

        static bool check_condition(JumpInstructionOpcode cond, core::RegisterFile & /*regs*/) noexcept {
            return cond.value == JumpInstructionOpcode::JMP;
        }

        static std::int16_t unsigned_to_signed_offset(std::uint16_t unsigned_offset) noexcept {
            const std::uint16_t carry = (unsigned_offset) & 0x200;
            if (carry == 0)
                return static_cast<std::int16_t>(unsigned_offset & 0x1FFu);
            const std::uint16_t value = unsigned_offset & 0x1FFu;
            const std::uint16_t negated = value | 0xFE00u;
            return static_cast<std::int16_t>(negated) * static_cast<std::int16_t>(2);
        }
    };

    struct BinaryInstruction {
        BinaryInstructionOpcode opcode;
        Addressing source_addressing;
        Addressing destination_addressing;

        static std::uint16_t calculate(BinaryInstructionOpcode opcode, std::uint16_t source, std::uint16_t dest) {
            spdlog::debug("calculate {:X} {:s} {:X}", source, BinaryInstructionOpcode::to_string(opcode), dest);
            switch (opcode.value) {
                case BinaryInstructionOpcode::ADD:
                    return source + dest;
                case BinaryInstructionOpcode::ADDC:
                    return source + dest;
                case BinaryInstructionOpcode::AND:
                    return source & dest;
                case BinaryInstructionOpcode::SUB:
                    return dest - source;
                case BinaryInstructionOpcode::SUBC:
                    return dest - source;
                case BinaryInstructionOpcode::XOR:
                    return source ^ dest;
                case BinaryInstructionOpcode::MOV:
                    return source;
                case BinaryInstructionOpcode::CMP:
                    return source;
                case BinaryInstructionOpcode::DADD:
                    return dest + source;
                case BinaryInstructionOpcode::BIT:
                    return source;
                case BinaryInstructionOpcode::BIC:
                    return source;
                case BinaryInstructionOpcode::BIS:
                    return source;
            }
        }
    };

    typedef std::variant<BinaryInstruction, UnaryInstruction, JumpInstruction> Instruction;

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

    typedef std::variant<BinaryInstructionDetail, UnaryInstructionDetail, JumpInstructionDetail> InstructionDetail;

    struct instruction {

        static void execute(const JumpInstruction &ins, core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView & /*ram*/) noexcept {
            if (JumpInstruction::check_condition(ins.condition, regs)) {
                pc.set(JumpInstruction::calculate(pc.get(), ins.signed_offset));
            }
        }

        inline static InstructionDetail decompile(const JumpInstruction &ins, core::MemoryRef &pc, core::RegisterFile &/* regs */, core::MemoryView & /*ram*/) noexcept {
            const auto jump_addr = JumpInstruction::calculate(pc.get(), ins.signed_offset);
            return JumpInstructionDetail{jump_addr, ins.signed_offset};
        }

        static void execute(const UnaryInstruction &ins, core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) noexcept {
            auto source_ref = addressing::get_ref(ins.source_addressing, pc, regs, ram);
            const auto source_value = source_ref.get();
            const auto res_value = UnaryInstruction::calculate(ins.opcode, source_value);
            if (ins.opcode.value == UnaryInstructionOpcode::CALL) {
                const auto dst = source_ref.get();
                auto sp = regs.get_ref(0x1);
                sp.get_and_increment(-0x2);
                auto stack = ram.get_word(sp.get());
                stack.set(pc.get());
                pc.set(dst);
            } else {
                if (res_value != source_value) {
                    source_ref.set(res_value);
                }
            }
        }

        inline static InstructionDetail decompile(const UnaryInstruction &ins, core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) noexcept {
            auto source_ref = addressing::get_ref(ins.source_addressing, pc, regs, ram);
            return UnaryInstructionDetail{addressing::to_string(ins.source_addressing, source_ref)};
        }

        static void execute(const BinaryInstruction &ins, core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) noexcept {
            auto source_ref = addressing::get_ref(ins.source_addressing, pc, regs, ram);
            auto dst_ref = addressing::get_ref(ins.destination_addressing, pc, regs, ram);

            spdlog::debug("execute {:s} = {:X}, {:s} = {:X}", addressing::to_string(ins.source_addressing),
                          source_ref.get(), addressing::to_string(ins.destination_addressing), dst_ref.get());
            const auto result = BinaryInstruction::calculate(ins.opcode, source_ref.get(), dst_ref.get());
            spdlog::debug("write-back {:X} => {:s}", result, addressing::to_string(ins.destination_addressing));
            dst_ref.set(result);
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
                                      [](const auto &i) { return to_string(i); }},
                              ins);
        }

        [[nodiscard]] static std::string opcode_to_string(const Instruction& ins) noexcept {
            return std::visit(overloaded{
                    [](const BinaryInstruction& bi) { return BinaryInstructionOpcode::to_string(bi.opcode); },
                    [](const UnaryInstruction& ui) { return UnaryInstructionOpcode::to_string(ui.opcode); },
                    [](const JumpInstruction& ji) { return JumpInstructionOpcode::to_string(ji.condition); },
            }, ins);
        }

        [[nodiscard]] static std::string to_string(const InstructionDetail& detail) {
            return std::visit(overloaded{
                                      [](const auto &i) { return to_string(i); }},
                              detail);
        }

        static void execute(const Instruction &ins, core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) noexcept {
            std::visit(overloaded{
                               [&pc, &regs, &ram](const auto &i) { execute(i, pc, regs, ram); }},
                       ins);
        }

        static InstructionDetail decompile(const Instruction &ins, core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) noexcept {
            return std::visit(overloaded{
                               [&pc, &regs, &ram](const auto &i) { return decompile(i, pc, regs, ram); }},
                       ins);
        }
    };
}// namespace msp

#endif//UNTITLED_UTILS_INSTRUCTIONS_H_

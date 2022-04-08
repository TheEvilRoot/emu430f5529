//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_CORE_PIPELINE_H_
#define UNTITLED_CORE_PIPELINE_H_

#include <core/memoryRef.h>
#include <core/memoryView.h>
#include <core/registerFile.h>
#include <utils/addressing.h>
#include <utils/instructions.h>
#include <utils/opcodes.h>

#include <utility>

#include <spdlog/spdlog.h>

namespace core {

    class Pipeline {
    private:
        core::RegisterFile &regs;
        core::MemoryView &ram;

        core::MemoryRef pc;

    public:
        Pipeline(core::RegisterFile &regs, core::MemoryView &ram) : regs{regs}, ram{ram}, pc{regs.get_ref(0)} {}

        static msp::Instruction decode(std::uint16_t instruction) {
            const auto format = InstructionFormat::from_value((instruction & 0xF000) >> 12);
            switch (format.value) {
                case InstructionFormat::JUMP_OP: {
                    const auto condition = JumpInstructionOpcode::from_value((instruction >> 10) & 0x7);
                    const auto unsigned_offset = std::uint16_t(instruction & 0x03FF);
                    const auto signed_offset = msp::JumpInstruction::unsigned_to_signed_offset(unsigned_offset);
                    return msp::JumpInstruction{condition, signed_offset};
                }
                case InstructionFormat::BINARY_OP: {
                    const auto opcode = BinaryInstructionOpcode::from_value((instruction & 0xF000) >> 12);
                    const auto source_register_num = (instruction & 0x0F00) >> 8;
                    const auto destination_register_num = instruction & 0xF;
                    const auto source_addressing_mode = (instruction & 0x0030) >> 4;
                    const auto byte_word_mode = (instruction & 0x0040) >> 6;
                    const auto destination_addressing_mode = (instruction & 0x0080) >> 7;
                    return msp::BinaryInstruction{
                            .opcode = opcode,
                            .source_addressing = msp::addressing::from_source(source_register_num, source_addressing_mode, byte_word_mode),
                            .destination_addressing = msp::addressing::from_destination(destination_register_num, destination_addressing_mode, byte_word_mode)};
                }
                case InstructionFormat::UNARY_OP: {
                    const auto register_num = instruction & 0xF;
                    const auto source_addressing_mode = (instruction & 0x0030) >> 4;
                    const auto byte_word_mode = (instruction & 0x0040) >> 6;
                    const auto opcode = UnaryInstructionOpcode::from_value((instruction & 0x0380) >> 7);
                    return msp::UnaryInstruction{
                            .opcode = opcode,
                            .source_addressing = msp::addressing::from_source(register_num, source_addressing_mode, byte_word_mode)};
                }
                case InstructionFormat::UNIMPL_OP: {
                    spdlog::debug("implemented instruction {:04X} format {}", instruction, InstructionFormat::to_string(format));
                    throw false;
                }
            }
        }

        void step() {
            const auto pc_val = pc.get_and_increment(0x2);
            try {
                const auto instruction_word = ram.get_word(pc_val).get();
                const auto instruction = decode(instruction_word);
                spdlog::info("{:04X} instruction {:04X} => {:s}",
                             pc_val, instruction_word,
                             msp::instruction::to_string(instruction));
                regs.dump();
                msp::instruction::execute(instruction, pc, regs, ram);
            } catch (...) {
                spdlog::info("{:04X} instruction => error", pc_val);
            }
        }
    };

}// namespace core
#endif//UNTITLED_CORE_PIPELINE_H_

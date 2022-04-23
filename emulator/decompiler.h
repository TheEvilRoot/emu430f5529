//
// Created by user on 7.04.22.
//

#ifndef UNTITLED_DECOMPILER_H
#define UNTITLED_DECOMPILER_H

#include <cstdint>

#include <core/pipeline.h>
#include <utils/utils.h>

struct Decompiler {

    struct DecompiledInstruction {
        std::uint16_t pc;
        msp::Instruction instruction;
        msp::InstructionDetail detail;
        std::string repr;

        explicit DecompiledInstruction(std::uint16_t pc, const msp::Instruction& i, const msp::InstructionDetail& detail, const std::string& repr) : pc{pc}, instruction{i}, detail{detail}, repr{repr} {
        }
    };

    static auto get_decompiled(const std::uint8_t* data, const std::size_t size) {
        std::vector<DecompiledInstruction> instructions;
        core::MemoryView ram{size};
        core::RegisterFile regs{0x10};
        auto pc = regs.get_ref(0x0);

        const std::size_t ram_addr = 0x0;
        for (std::size_t i = 0; i < size; i++) {
            ram.set_byte(ram_addr + i, data[i]);
        }
        pc.set(ram_addr);

        const auto start = std::chrono::steady_clock::now();
        while (pc.get() < 0xFFFE) {
            const auto pc_val = pc.get_and_increment(0x2);
            try {
                const auto instruction_word = ram.get_word(pc_val).get();
                const auto instruction = core::Pipeline::decode(instruction_word);
                const auto detail = msp::instruction::decompile(instruction, pc, regs, ram);
                const auto representation = fmt::format("{} {}", msp::instruction::opcode_to_string(instruction),
                                                        msp::instruction::to_string(detail));
                instructions.emplace_back(pc_val, instruction, detail, representation);
            } catch (...) {
            }
        }
        const auto end = std::chrono::steady_clock::now();
        const auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        spdlog::info("decompiler ran {}ms", delta);
        return instructions;
    }
};


#endif//UNTITLED_DECOMPILER_H

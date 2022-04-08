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
        std::string repr;

        explicit DecompiledInstruction(std::uint16_t pc, const msp::Instruction& i) : pc{pc}, instruction{i}, repr{msp::instruction::to_string(i)} {
        }
    };

    static auto get_decompiled(const std::uint8_t* data, const std::size_t size) {
        std::vector<DecompiledInstruction> instructions;
        core::MemoryView ram{size};
        core::RegisterFile regs{0x10};
        auto pc = regs.get_ref(0x0);

        const std::size_t ram_addr = 0x0;
        for (std::size_t i = 0; i < size; i++) {
            ram.get_byte(ram_addr + i).set(data[i]);
        }
        pc.set(0x0);

        while (pc.get() < 0xFFFE) {
            const auto pc_val = pc.get_and_increment(0x2);
            try {
                const auto instruction_word = ram.get_word(pc_val).get();
                const auto instruction = core::Pipeline::decode(instruction_word);
                spdlog::info("decompiled {:04X} => {:04X}", pc_val, instruction_word);
                instructions.emplace_back(pc_val, instruction);
            } catch (...) {
            }
        }
        return instructions;
    }
};


#endif//UNTITLED_DECOMPILER_H

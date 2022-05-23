//
// Created by user on 7.04.22.
//

#ifndef UNTITLED_DECOMPILER_H
#define UNTITLED_DECOMPILER_H

#include <cstdint>
#include <utility>
#include <map>

#include <core/pipeline.h>
#include <utils/utils.h>
#include <utils/measure.h>
#include <core/interruptController.h>

namespace emu {
    struct Decompiler {

        struct DecompiledInstruction {
            std::uint16_t pc;
            msp::Instruction instruction;
            msp::InstructionDetail detail;
            std::string repr;
            std::string label{};

            explicit DecompiledInstruction(std::uint16_t pc, const msp::Instruction &i, msp::InstructionDetail detail, const std::string &repr) : pc{pc}, instruction{i}, detail{std::move(detail)}, repr{repr} {
            }
        };

        std::vector<DecompiledInstruction> instructions{};
        std::map<std::uint16_t, std::string> labels{};

        core::MemoryView ram;
        core::RegisterFile regs;
        InterruptController interrupt_controller;
        core::MemoryRef pc;

        Decompiler(const std::uint8_t* data, std::size_t size): ram{size}, regs{0x10}, interrupt_controller{regs, ram}, pc{regs.get_ref(0x0)} {
            const std::size_t ram_addr = 0x0;
            for (std::size_t i = 0; i < size; i++) {
                ram.set_byte(ram_addr + i, data[i]);
            }
            pc.set(ram_addr);
        }

        auto get_detail(msp::Instruction instruction, std::uint16_t pc_val) {
            return std::visit(overloaded{
                    [](const msp::UnimplementedInstruction&) { 
                        return msp::InstructionDetail{msp::UnimplementedInstructionDetail{}}; 
                    },
                    [this, pc_val](const auto& instruction) {
                        auto detail = msp::instruction::decompile(instruction, pc, regs, ram);
                        const auto representation = fmt::format("{} {}", msp::instruction::opcode_to_string(instruction),
                                                                msp::instruction::to_string(detail));
                        instructions.emplace_back(pc_val, instruction, detail, representation);
                        return detail;
                    }
            }, instruction);
        }

        void decompile_instructions() {
            while (pc.get() <= 0xFFFE) {
                const auto pc_val = pc.get_and_increment(0x2);
                const auto instruction_word = ram.get_word(pc_val).get();
                const auto instruction = core::Pipeline::decode(instruction_word);
                const auto detail = get_detail(instruction, pc_val);
                std::visit(overloaded{
                        [this](const msp::JumpInstructionDetail& j) {
                            labels[j.jump_addr] = fmt::format("LABEL_{:04X}", j.jump_addr);
                        },
                        [](const auto&) { }
                }, detail);
                if (pc.get() == 0x0) break;
            }
        }

        void postprocess_jump_labels() {
            const auto vectors_labels = interrupt_controller.get_handlers_labels();
            std::for_each(vectors_labels.begin(), vectors_labels.end(), [this](const auto& pair) {
                const auto &[addr, label] = pair;
                labels[addr] = std::string{label};
            });
        }

        void postprocess_interrupt_labels() {
            std::for_each(instructions.begin(), instructions.end(), [this](auto& i) {
                const auto vector_label = interrupt_controller.get_vector_label(i.pc);
                if (vector_label) {
                    i.label = vector_label.value();
                } else if (labels.contains(i.pc)) {
                    i.label = labels[i.pc];
                }
            });
        }

        void decompile() {
            measure measure{};
            decompile_instructions();
            postprocess_jump_labels();
            postprocess_interrupt_labels();
            spdlog::warn("decompiler ran {}ms", measure.get_time<std::chrono::milliseconds>());
        }

        static auto get_decompiled(const std::uint8_t *data, const std::size_t size) {
            Decompiler decompiler(data, size);
            decompiler.decompile();
            return decompiler.instructions;
        }
    };
}


#endif//UNTITLED_DECOMPILER_H

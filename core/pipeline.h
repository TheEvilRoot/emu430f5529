//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_CORE_PIPELINE_H_
#define UNTITLED_CORE_PIPELINE_H_

#include <core/memoryRef.h>
#include <core/memoryView.h>
#include <utils/opcodes.h>
#include <utils/addressing.h>
#include <utils/instructions.h>

#include <utility>

namespace core {

class Pipeline {
 private:
  core::MemoryView &regs;
  core::MemoryView &ram;

  core::MemoryRef pc;
 public:
  Pipeline(core::MemoryView &regs, core::MemoryView &ram) : regs {regs}, ram {ram}, pc {regs.get_word(0x0)} {}

  static Instruction *decode(std::uint16_t instruction) {
    const auto format = InstructionFormat::from_value((instruction & 0xF000) >> 12);
    switch (format.value) {
      case InstructionFormat::JUMP_OP: {
        const auto condition = JumpInstructionOpcode::from_value((instruction >> 10) & 0x7);
        const auto unsigned_offset = std::uint16_t(instruction & 0x03FF);
        return new JumpInstruction {
            condition,
            unsigned_offset
        };
      }
      case InstructionFormat::BINARY_OP: {
        const auto opcode = BinaryInstructionOpcode::from_value((instruction & 0xF000) >> 12);
        const auto source_register_num = (instruction & 0x0F00) >> 16;
        const auto destination_register_num = instruction & 0xF;
        const auto source_addressing_mode = (instruction & 0x0030) >> 4;
        const auto byte_word_mode = (instruction & 0x0040) >> 6;
        const auto destination_addressing_mode = (instruction & 0x0080) >> 7;
        return new BinaryInstruction(
            opcode,
            std::unique_ptr<SourceAddressing>(SourceAddressing::from_source(source_register_num,
                                                                            source_addressing_mode,
                                                                            byte_word_mode)),
            std::unique_ptr<SourceAddressing>(SourceAddressing::from_destination(destination_register_num,
                                                                                 destination_addressing_mode,
                                                                                 byte_word_mode))
        );
      }
      case InstructionFormat::UNARY_OP: {
        const auto register_num = instruction & 0xF;
        const auto source_addressing_mode = (instruction & 0x0030) >> 4;
        const auto byte_word_mode = (instruction & 0x0040) >> 6;
        const auto opcode = UnaryInstructionOpcode::from_value((instruction & 0x0380) >> 7);
        return new UnaryInstruction {
            opcode,
            std::unique_ptr<SourceAddressing>(SourceAddressing::from_source(register_num,
                                                                            source_addressing_mode,
                                                                            byte_word_mode))
        };
      }
      case InstructionFormat::UNIMPL_OP:
          return nullptr;
    }
  }

  void step() {
    const auto pc_val = pc.get_and_increment(0x2);
    const auto instruction_word = ram.get_word(pc_val).get();
    const auto instruction = std::shared_ptr<Instruction>(decode(instruction_word));

    fprintf(stderr, "%04x instruction %04x => %s %s\n",
            pc_val, instruction_word, InstructionFormat::to_string(instruction->format).c_str(),
            instruction->to_string().c_str());

#ifdef DRY
    assert(instruction != nullptr);
#endif

    if (instruction != nullptr) {
#ifndef DRY
        instruction->execute(pc, regs, ram);
#endif
    }
  }
};

}
#endif //UNTITLED_CORE_PIPELINE_H_

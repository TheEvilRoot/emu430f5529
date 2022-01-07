//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_UTILS_INSTRUCTIONS_H_
#define UNTITLED_UTILS_INSTRUCTIONS_H_

#include <core/registerFile.h>

#include <memory>

#include <spdlog/spdlog.h>

class Instruction {
 public:
  const InstructionFormat format;
 protected:
  explicit Instruction(InstructionFormat fmt): format(fmt) { }
 public:
  virtual void execute(core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) = 0;

  [[nodiscard]] virtual std::string to_string() const = 0;
};

class UnaryInstruction : public Instruction {
 private:
  UnaryInstructionOpcode opcode;
  std::unique_ptr<SourceAddressing> addressing;

 public:
  UnaryInstruction(UnaryInstructionOpcode opcode, std::unique_ptr<SourceAddressing> addressing):
   Instruction(InstructionFormat::UNARY_OP), opcode{opcode}, addressing{std::move(addressing)} { }

  static std::uint16_t calculate(UnaryInstructionOpcode opcode, std::uint16_t value) {
    switch (opcode.value) {
      case UnaryInstructionOpcode::RRC: return value >> 1;
      case UnaryInstructionOpcode::RRA: return value >> 1;
      case UnaryInstructionOpcode::SWPB: return ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
      case UnaryInstructionOpcode::SXT: return value;
      case UnaryInstructionOpcode::PUSH: return value;
      case UnaryInstructionOpcode::CALL: return value;
      case UnaryInstructionOpcode::RETI: return value;
      default: assert(false);
    }
  }

  void execute(core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) override {
    auto source_ref = addressing->get_ref(pc, regs, ram);
    const auto source_value = source_ref.get();
    const auto res_value = UnaryInstruction::calculate(opcode, source_value);
    if (res_value != source_value) {
      source_ref.set(res_value);
    }
  }

  [[nodiscard]] std::string to_string() const override {
      return UnaryInstructionOpcode::to_string(opcode) + " " + addressing->to_string();
  }
};

class JumpInstruction : public Instruction {
 private:
  JumpInstructionOpcode condition;
  std::uint16_t signed_offset;

 public:
  JumpInstruction(JumpInstructionOpcode opcode, std::uint16_t unsigned_offset):
   Instruction(InstructionFormat::JUMP_OP), condition{opcode} {
    signed_offset = JumpInstruction::unsigned_to_signed_offset(unsigned_offset);
  }

  void execute(core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &/*ram*/) override {
    if (JumpInstruction::check_condition(condition, regs)) {
      pc.set(JumpInstruction::calculate(pc.get(), signed_offset));
    }
  }

  [[nodiscard]] std::string to_string() const override {
      return JumpInstructionOpcode::to_string(condition) + " " + std::to_string(signed_offset);
  }

  static std::uint16_t calculate(std::uint16_t pc, std::uint16_t signed_offset) {
    return pc + signed_offset;
  }

  static bool check_condition(JumpInstructionOpcode /*cond*/, core::RegisterFile &/*regs*/) {
    return false;
  }

  static std::uint16_t unsigned_to_signed_offset(std::uint16_t unsigned_offset) {
    return unsigned_offset;
  }
};

class BinaryInstruction : public Instruction {
 private:
  BinaryInstructionOpcode opcode;
  std::unique_ptr<SourceAddressing> source_addressing;
  std::unique_ptr<SourceAddressing> destination_addressing;

 public:
  BinaryInstruction(BinaryInstructionOpcode opcode, std::unique_ptr<SourceAddressing> source, std::unique_ptr<SourceAddressing> destination):
   Instruction(InstructionFormat::BINARY_OP), opcode{opcode}, source_addressing{std::move(source)}, destination_addressing{std::move(destination)} { }

  void execute(core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) override {
      const auto source_ref = source_addressing->get_ref(pc, regs, ram);
      auto dst_ref = destination_addressing->get_ref(pc, regs, ram);

      spdlog::debug("execute {:s} = {:X}, {:s} = {:X}", source_addressing->to_string(),
                    source_ref.get(), destination_addressing->to_string(), dst_ref.get());
      const auto result = BinaryInstruction::calculate(opcode, source_ref.get(), dst_ref.get());
      spdlog::debug("write-back {:X} => {:s}", result, destination_addressing->to_string());
      dst_ref.set(result);
  }

  [[nodiscard]] std::string to_string() const override {
      return BinaryInstructionOpcode::to_string(opcode) + " " +
        source_addressing->to_string() + " " + destination_addressing->to_string();
  }

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
              return source - dest;
          case BinaryInstructionOpcode::SUBC:
              return source - dest;
          case BinaryInstructionOpcode::XOR:
              return source ^ dest;
          case BinaryInstructionOpcode::MOV:
              return source;
          default: assert(false);
      }
  }
};

#endif //UNTITLED_UTILS_INSTRUCTIONS_H_

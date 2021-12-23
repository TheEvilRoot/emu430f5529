//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_UTILS_OPCODES_H_
#define UNTITLED_UTILS_OPCODES_H_

#include <string>
#include <cassert>

class InstructionFormat {
 public:
  enum Value { UNARY_OP, BINARY_OP, JUMP_OP };

  Value value;

  InstructionFormat(Value v): value{v} { }

  static std::string to_string(InstructionFormat fmt) {
    switch (fmt.value) {
      case UNARY_OP:
        return "Unary";
      case BINARY_OP:
        return "Binary";
      case JUMP_OP:
        return "Jump";
    }
  }

  static InstructionFormat from_value(std::uint16_t value) {
    switch (value) {
      case 0:
        assert(false);
      case 1:
        return Value::UNARY_OP;
      case 2:
      case 3:
        return Value::JUMP_OP;
      default:
        return Value::BINARY_OP;
    }
  }
};

class BinaryInstructionOpcode {
 public:
  enum Values { MOV, ADD, ADDC, SUBC, SUB, CMP, DADD, BIT, BIC, BIS, XOR, AND };

  Values value;

  BinaryInstructionOpcode(Values v): value{v} { }

  static std::string to_string(BinaryInstructionOpcode opc) {
    switch (opc.value) {
      case Values::MOV: return "MOV";
      case Values::ADD: return "ADD";
      case Values::ADDC: return "ADDC";
      case Values::SUBC: return "SUBC";
      case Values::SUB: return "SUB";
      case Values::CMP: return "CMP";
      case Values::DADD: return "DADD";
      case Values::BIT: return "BIT";
      case Values::BIC: return "BIC";
      case Values::BIS: return "BIS";
      case Values::XOR: return "XOR";
      case Values::AND: return "AND";
    }
  }

  static BinaryInstructionOpcode from_value(std::uint16_t val) {
    switch (val) {
      case 0x4: return Values::MOV;
      case 0x5: return Values::ADD;
      case 0x6: return Values::ADDC;
      case 0x7: return Values::SUBC;
      case 0x8: return Values::SUB;
      case 0x9: return Values::CMP;
      case 0xA: return Values::DADD;
      case 0xB: return Values::BIT;
      case 0xC: return Values::BIC;
      case 0xD: return Values::BIS;
      case 0xE: return Values::XOR;
      case 0xF: return Values::AND;
      default: assert(false);
    }
  }

};

class JumpInstructionOpcode {
 public:
  enum Values { JNE_JNZ, JEQ_JZ, JNC_JLO, JC_JHS, JN, JGE, JL, JMP };

  Values value;

  JumpInstructionOpcode(Values v): value{v} { }

  static std::string to_string(JumpInstructionOpcode opc) {
    switch (opc.value) {
      case Values::JNE_JNZ: return "JNE/JNZ";
      case Values::JEQ_JZ: return "JEQ/JZ";
      case Values::JNC_JLO: return "JNC/JLO";
      case Values::JC_JHS: return "JC/JHS";
      case Values::JN: return "JN";
      case Values::JGE: return "JGE";
      case Values::JL: return "JL";
      case Values::JMP: return "JMP";
    }
  }

  static JumpInstructionOpcode from_value(std::uint16_t val) {
    switch (val) {
      case 0x0: return Values::JNE_JNZ;
      case 0x1: return Values::JEQ_JZ;
      case 0x2: return Values::JNC_JLO;
      case 0x3: return Values::JC_JHS;
      case 0x4: return Values::JN;
      case 0x5: return Values::JGE;
      case 0x6: return Values::JL;
      case 0x7: return Values::JMP;
      default: assert(false);
    }
  }
};

class UnaryInstructionOpcode {
 public:
 enum Values { RRC, SWPB, RRA, SXT, PUSH, CALL, RETI };

 Values value;

 UnaryInstructionOpcode(Values v): value{v} { }

 static std::string to_string(UnaryInstructionOpcode opc) {
   switch (opc.value) {
     case Values::RRC: return "RRA";
     case Values::SWPB: return "SWPB";
     case Values::RRA: return "RRA";
     case Values::SXT: return "SXT";
     case Values::PUSH: return "PUSH";
     case Values::CALL: return "CALL";
     case Values::RETI: return "RETI";
   }
 }

 static UnaryInstructionOpcode from_value(std::uint16_t val) {
   switch (val) {
     case 0x0: return Values::RRC;
     case 0x1: return Values::SWPB;
     case 0x2: return Values::RRA;
     case 0x3: return Values::SXT;
     case 0x4: return Values::PUSH;
     case 0x5: return Values::CALL;
     case 0x6: return Values::RETI;
     default: assert(false);
   }
 }

};

#endif //UNTITLED_UTILS_OPCODES_H_

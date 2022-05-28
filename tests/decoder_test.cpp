#include <core/pipeline.h>
#include <gtest/gtest.h>

auto get_format(const msp::Instruction& i) {
    return std::visit(overloaded{
          [](const msp::BinaryInstruction&) { return InstructionFormat::BINARY_OP; },
          [](const msp::UnaryInstruction&) { return InstructionFormat::UNARY_OP; },
          [](const msp::JumpInstruction&) { return InstructionFormat::JUMP_OP; },
          [](const msp::UnimplementedInstruction&) { return InstructionFormat::UNIMPL_OP; }
    }, i);
}

template<typename T>
auto expect_opcode(const msp::Instruction& i, T opcode) {
    return std::visit(overloaded{
          [opcode](const msp::BinaryInstruction& b) { if constexpr (std::is_same_v<T, BinaryInstructionOpcode::Values>) EXPECT_EQ(opcode, b.opcode.value); else EXPECT_TRUE(false); },
          [opcode](const msp::UnaryInstruction& b) { if constexpr (std::is_same_v<T, UnaryInstructionOpcode::Values>) EXPECT_EQ(opcode, b.opcode.value); else EXPECT_TRUE(false); },
          [opcode](const msp::JumpInstruction& b) { if constexpr (std::is_same_v<T, JumpInstructionOpcode::Values>) EXPECT_EQ(opcode, b.condition.value); else EXPECT_TRUE(false); },
          [opcode](const msp::UnimplementedInstruction&) { if constexpr (std::is_same_v<T, InstructionFormat::Value>) EXPECT_EQ(InstructionFormat::UNIMPL_OP, opcode); else EXPECT_TRUE(false); }
    }, i);
}

template<int reg>
auto expect_direct_register_addressing(const msp::Addressing& addressing) {
    std::visit(overloaded {
            [](const msp::RegisterDirectAddressing& r) { EXPECT_EQ(reg, r.reg); },
            [](const auto& r) { EXPECT_TRUE(false); }
    }, addressing);
}

template<typename F>
auto expect_source_addressing(const msp::Instruction& i, const F& f) {
    return std::visit(overloaded{
          [&f](const msp::BinaryInstruction& b) { return f(b.source_addressing); },
          [&f](const msp::UnaryInstruction& b) { return f(b.source_addressing); },
          [&f](const msp::JumpInstruction&) { EXPECT_TRUE(false); },
          [&f](const msp::UnimplementedInstruction&) { EXPECT_TRUE(false); }
    }, i);
}

template<std::uint16_t off>
auto expect_signed_offset(const msp::Instruction& i) {
    return std::visit(overloaded{
          [](const msp::JumpInstruction& j) { EXPECT_EQ(off, j.signed_offset); },
          [](const auto& r) { EXPECT_TRUE(false); }
    }, i);
}

TEST(Decoder, decode_unary_instruction) {
    const auto instruction = core::Pipeline::decode(0x118C);
    const auto format = get_format(instruction);
    EXPECT_EQ(InstructionFormat::UNARY_OP, format);
    expect_opcode(instruction, UnaryInstructionOpcode::SXT);
    expect_source_addressing(instruction, expect_direct_register_addressing<12>);
}

TEST(Decoder, decode_binary_instruction) {
    const auto instruction = core::Pipeline::decode(0x4cc2);
    const auto format = get_format(instruction);
    EXPECT_EQ(InstructionFormat::BINARY_OP, format);
    expect_opcode(instruction, BinaryInstructionOpcode::MOV);
    expect_source_addressing(instruction, expect_direct_register_addressing<12>);
}

TEST(Decoder, decode_jump_instruction) {
    const auto instruction = core::Pipeline::decode(0x2016);
    const auto format = get_format(instruction);
    EXPECT_EQ(InstructionFormat::JUMP_OP, format);
    expect_opcode(instruction, JumpInstructionOpcode::JNE_JNZ);
    expect_signed_offset<0x2c>(instruction);
}

TEST(Decoder, decode_unmpl_instruction) {
    const auto instruction = core::Pipeline::decode(0x0016);
    const auto format = get_format(instruction);
    EXPECT_EQ(InstructionFormat::UNIMPL_OP, format);
    expect_opcode(instruction, InstructionFormat::UNIMPL_OP);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
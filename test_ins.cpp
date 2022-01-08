//
// Created by Доктор Кларик on 8.01.22.
//

#include <gtest/gtest.h>

#include <utils/addressing.h>
#include <utils/opcodes.h>
#include <utils/instructions.h>

TEST(Binary, add) {
  const BinaryInstructionOpcode opcode = BinaryInstructionOpcode::ADD;
  const auto res = BinaryInstruction::calculate(opcode, 1337, 337);
  EXPECT_EQ(res, 1337 + 337);
}

TEST(Binary, sub) {
  const BinaryInstructionOpcode opcode = BinaryInstructionOpcode::SUB;
  const auto res = BinaryInstruction::calculate(opcode, 1337, 337);
  EXPECT_EQ(res, 1337 - 337);
}

TEST(Binary, op_xor) {
  const BinaryInstructionOpcode opcode = BinaryInstructionOpcode::XOR;
  const auto res = BinaryInstruction::calculate(opcode, 1337, 337);
  EXPECT_EQ(res, 1337 ^ 337);
}
int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
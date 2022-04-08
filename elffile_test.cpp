//
// Created by Доктор Кларик on 8.01.22.
//

#include <gtest/gtest.h>

#include <utils/elffile.h>

TEST(ElfFileLoading, open_elf) {
  std::ifstream file("../msp430/out/simple_1.out", std::ios::binary);
  elf::ElfFile32 elf(file);
  EXPECT_EQ(elf.header.e_ident[0], 0x7F);
  EXPECT_EQ(elf.header.e_ident[1], 'E');
  EXPECT_EQ(elf.header.e_ident[2], 'L');
  EXPECT_EQ(elf.header.e_ident[3], 'F');

  EXPECT_FALSE(elf.programs.empty());
  for (std::size_t index = 0; const auto& program : elf.programs) {
    EXPECT_NE(program.p_vaddr, 0);
    EXPECT_EQ(program.p_type, 1);

    const auto code = elf.get_program(index++);
    EXPECT_TRUE(code != nullptr);
  }
  file.close();
}

int main() {
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}
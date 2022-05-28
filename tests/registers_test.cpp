//
// Created by theevilroot on 22.12.2021.
//
#include <core/registerFile.h>
#include <gtest/gtest.h>

TEST(Register, registers_get_and_increment) {
    core::RegisterFile mem{0x2};
    auto ref = mem.get_ref(0x0);
    for (std::uint16_t i = 0; i < 0xFFFF; i++) {
        const auto value = ref.get_and_increment(0x1);
        EXPECT_EQ(i, value);
        EXPECT_EQ(i + 1, ref.get());
    }
}

TEST(Register, registers_set_get) {
    core::RegisterFile mem{0x2};
    auto ref = mem.get_ref(0x0);
    for (std::uint16_t i = 0; i < 0xFFFF; i++) {
        ref.set(i);
        const auto value = ref.get();
        EXPECT_EQ(i, value);
    }
}

TEST(Register, registers_initialized_with_zeros) {
    core::RegisterFile mem{0x100};
    for (std::size_t i = 0; i < 0x100; i++) {
        EXPECT_EQ(0, mem.get_ref(i).get());
    }
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
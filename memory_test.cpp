//
// Created by theevilroot on 22.12.2021.
//
#include <core/memoryView.h>
#include <gtest/gtest.h>

TEST(Memory, memory_get_and_increment_test) {
    core::MemoryView mem{0x2};
    auto ref = mem.get_word(0x0);
    for (std::uint16_t i = 0; i < 0xFFFF; i++) {
        const auto value = ref.get_and_increment(0x1);
        EXPECT_EQ(i, value);
    }
}

TEST(Memory, memory_set_get_test) {
    core::MemoryView mem{0x2};
    auto ref = mem.get_word(0x0);
    for (std::uint16_t i = 0; i < 0xFFFF; i++) {
        ref.set(i);
        const auto value = ref.get();
        EXPECT_EQ(value, i);
    }
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
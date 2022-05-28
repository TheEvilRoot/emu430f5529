//
// Created by theevilroot on 22.12.2021.
//
#include "core/memoryView.h"
#include <gtest/gtest.h>

TEST(Memory, memory_get_and_increment_test) {
    core::MemoryView mem{0x2};
    auto ref = mem.get_word(0x0);
    for (std::uint16_t i = 0; i < 0xFFFF; i++) {
        const auto value = ref.get_and_increment(0x1);
        EXPECT_EQ(i, value);
        EXPECT_EQ(i + 1, ref.get());
    }
}

TEST(Memory, memory_set_get_test) {
    core::MemoryView mem{0x2};
    auto ref = mem.get_word(0x0);
    for (std::uint16_t i = 0; i < 0xFFFF; i++) {
        ref.set(i);
        const auto value = ref.get();
        EXPECT_EQ(i, value);
    }
}

TEST(Memory, memory_initialized_with_zeros) {
    core::MemoryView mem{0x100};
    for (std::size_t i = 0; i < 0x100; i++) {
        EXPECT_EQ(0, mem.get_byte(i).get());
    }
}

TEST(Memory, memory_region_access_legal_word) {
    core::MemoryView mem{0x100};
    core::MemoryRegion reg{"region", core::MemoryRegionAccess::ONLY_WORD, 0x0, 0x20};
    mem.add_region(reg);
    EXPECT_NO_THROW((void)mem.get_word(0x10).get());
}

TEST(Memory, memory_region_access_legal_byte) {
    core::MemoryView mem{0x100};
    core::MemoryRegion reg{"region", core::MemoryRegionAccess::ONLY_BYTE, 0x0, 0x20};
    mem.add_region(reg);
    EXPECT_NO_THROW((void)mem.get_byte(0x10).get());
}

TEST(Memory, memory_region_access_legal_any) {
    core::MemoryView mem{0x100};
    core::MemoryRegion reg{"region", core::MemoryRegionAccess::WORD_BYTE, 0x0, 0x20};
    mem.add_region(reg);
    EXPECT_NO_THROW((void)mem.get_byte(0x10).get());
    EXPECT_NO_THROW((void)mem.get_word(0x10).get());
}

TEST(Memory, memory_region_access_illegal_word) {
    core::MemoryView mem{0x100};
    core::MemoryRegion reg{"region", core::MemoryRegionAccess::ONLY_WORD, 0x0, 0x20};
    mem.add_region(reg);
    EXPECT_THROW((void)mem.get_byte(0x10).get(), MemoryViolationException);
}

TEST(Memory, memory_region_access_illegal_byte) {
    core::MemoryView mem{0x100};
    core::MemoryRegion reg{"region", core::MemoryRegionAccess::ONLY_BYTE, 0x0, 0x20};
    mem.add_region(reg);
    EXPECT_THROW((void)mem.get_word(0x10).get(), MemoryViolationException);
}

TEST(Memory, memory_region_access_legal_undefined) {
    core::MemoryView mem{0x100};
    core::MemoryRegion reg{"region", core::MemoryRegionAccess::ONLY_BYTE, 0x0, 0x20};
    mem.add_region(reg);
    EXPECT_NO_THROW((void)mem.get_word(0x30).get());
    EXPECT_NO_THROW((void)mem.get_byte(0x30).get());
}

TEST(Memory, memory_set_bytes_get_word) {
    core::MemoryView mem{0x2};
    mem.set_byte(0x0, 0xAB);
    mem.set_byte(0x1, 0xCD);
    EXPECT_EQ(0xABCD, mem.get_word(0x0).get());
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
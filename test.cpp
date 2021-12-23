//
// Created by theevilroot on 22.12.2021.
//

#include <iostream>
#include <cassert>

#include <core/memoryView.h>

void memory_get_and_increment_test() {
    core::MemoryView mem{0x2};
    auto ref = mem.get_word(0x0);
    for (std::uint16_t i = 0; i < 0xFFFF; i++) {
        const auto value = ref.get_and_increment(0x1);
        assert(i == value);
    }
}

void memory_set_get_test() {
    core::MemoryView mem{0x2};
    auto ref = mem.get_word(0x0);
    for (std::uint16_t i = 0; i < 0xFFFF; i++) {
        ref.set(i);
        const auto value = ref.get();
        assert(value == i);
    }
}

int main() {
    std::cout << "memory_get_and_increment_test()\n";
    memory_get_and_increment_test();
    std::cout << "memory_get_and_increment_test() pass\n";

    std::cout << "memory_set_get_test()\n";
    memory_set_get_test();
    std::cout << "memory_set_get_test() pass\n";
}
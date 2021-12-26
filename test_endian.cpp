//
// Created by theevilroot on 23.12.2021.
//

#define BIG

#include <cassert>

#include <utils/utils.h>
#include <core/memoryView.h>

int main() {
    core::MemoryView mem{0x4};
    mem.get_byte(0x0).set(0xaa);
    mem.get_byte(0x1).set(0xbb);
    mem.get_byte(0x2).set(0xcc);
    mem.get_byte(0x3).set(0xdd);

    const auto first_word = mem.get_word(0x00).get();
    const auto second_word = mem.get_word(0x02).get();

    const auto byte_a = mem.get_byte(0x0).get();
    const auto byte_b = mem.get_byte(0x1).get();
    const auto byte_c = mem.get_byte(0x2).get();
    const auto byte_d = mem.get_byte(0x3).get();

    const auto first_check_big = ((static_cast<std::uint16_t>(byte_a) << 8) | (static_cast<std::uint16_t>(byte_b)));
    const auto second_check_big = ((static_cast<std::uint16_t>(byte_c) << 8) | (static_cast<std::uint16_t>(byte_d)));

    fprintf(stderr, "%s\n", ENDIANESS);
    fprintf(stderr, "check\n");
    fprintf(stderr, "%04x\n%04x\n", first_check_big, second_check_big);
    fprintf(stderr, "result\n");
    fprintf(stderr, "%04x\n%04x\n", first_word, second_word);
}
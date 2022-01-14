//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_UTILS_UTILS_H_
#define UNTITLED_UTILS_UTILS_H_

#ifdef LITTLE
#define ENDIANESS "LITTLE"
#define WORD_AT_OFFSET(b, o) ((static_cast<std::uint16_t>(b[o]) << 8) | (static_cast<std::uint16_t>(b[o + 1])))
#define WORD_TO_OFFSET(b, o, w) { b[o] = static_cast<std::uint8_t>((w & 0xFF00) >> 8); b[o + 1] = static_cast<std::uint8_t>(w & 0xFF); }
#else
#define ENDIANESS "BIG"
#define WORD_AT_OFFSET(b, o) ((static_cast<std::uint16_t>(b[o + 1]) << 8) | (static_cast<std::uint16_t>(b[o])))
#define WORD_TO_OFFSET(b, o, w) { b[o + 1] = static_cast<std::uint8_t>((w & 0xFF00) >> 8); b[o] = static_cast<std::uint8_t>(w & 0xFF); }
#endif

// from cppreference
// helper type for the visitor #4
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

#endif //UNTITLED_UTILS_UTILS_H_

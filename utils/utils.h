//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_UTILS_UTILS_H_
#define UNTITLED_UTILS_UTILS_H_

#define WORD_AT_OFFSET(b, o) ((static_cast<std::uint16_t>(b[o]) << 8) | (static_cast<std::uint16_t>(b[o + 1])))
#define WORD_TO_OFFSET(b, o, w) { b[o] = static_cast<std::uint8_t>((w & 0xFF00) >> 8); b[o + 1] = static_cast<std::uint8_t>(w & 0xFF); }

#endif //UNTITLED_UTILS_UTILS_H_

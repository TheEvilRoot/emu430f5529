//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_UTILS_UTILS_H_
#define UNTITLED_UTILS_UTILS_H_

template<typename T>
concept SubscriptableContainer = requires(T t) {
    { t[std::size_t{}] } -> std::unsigned_integral;
};

template<typename T>
concept SubscriptablePointer = requires(T t) {
    { *t } -> std::unsigned_integral;
};

template<typename T>
concept Subscriptable = requires(T t) {
    SubscriptableContainer<T> || SubscriptablePointer<T>;
};

namespace endian {
    template<Subscriptable Container>
    struct little {
        static std::uint16_t word_at(const Container &b, const std::size_t o) {
            return ((static_cast<std::uint16_t>(b[o]) << 8) | (static_cast<std::uint16_t>(b[o + 1])));
        }
        static void word_to(const Container &b, const std::size_t o, const std::uint16_t w) {
            b[o] = static_cast<std::uint8_t>((w & 0xFF00) >> 8);
            b[o + 1] = static_cast<std::uint8_t>(w & 0xFF);
        }
    };
    template<Subscriptable Container>
    struct big {
        static std::uint16_t word_at(const Container &b, const std::size_t o) {
            return ((static_cast<std::uint16_t>(b[o + 1]) << 8) | (static_cast<std::uint16_t>(b[o])));
        }
        static void word_to(const Container &b, const std::size_t o, const std::uint16_t w) {
            b[o + 1] = static_cast<std::uint8_t>((w & 0xFF00) >> 8);
            b[o] = static_cast<std::uint8_t>(w & 0xFF);
        }
    };
    template<Subscriptable T>
    struct msp : public little<T> {};
}// namespace endian

// from cppreference
// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

#endif//UNTITLED_UTILS_UTILS_H_

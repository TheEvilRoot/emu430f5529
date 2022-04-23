//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_UTILS_TICKCONTROLLER_H_
#define UNTITLED_UTILS_TICKCONTROLLER_H_

#include <chrono>

#include <unistd.h>

using namespace std::literals;

class TickController {
private:
    long tick_us;
    std::chrono::time_point<std::chrono::steady_clock> tick_state;

    std::uint64_t tick_counter;
    std::chrono::time_point<std::chrono::steady_clock> tick_counter_state;

public:
    std::uint64_t frequency;

    explicit TickController(long tick_us) : tick_us{tick_us}, tick_state{std::chrono::steady_clock::now()}, tick_counter{0}, tick_counter_state{std::chrono::steady_clock::now()}, frequency{0} {}

    inline void count_tick() {
    }

    inline void generate_tick() {
        const auto n = std::chrono::steady_clock::now();
        const auto delta = (n - tick_state) / 1us;
        if (delta < tick_us)
            usleep(tick_us - delta);
        tick_state = std::chrono::steady_clock::now();
        count_tick();
    }
};

#endif//UNTITLED_UTILS_TICKCONTROLLER_H_

//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_UTILS_TICKCONTROLLER_H_
#define UNTITLED_UTILS_TICKCONTROLLER_H_

#include <chrono>
#include <thread>

#include <unistd.h>

using namespace std::literals;

class TickController {
private:
    using tick_clock = std::chrono::steady_clock;
    const long tick_us;

    tick_clock::time_point tick_start;

    std::uint64_t tick_counter;
    tick_clock::time_point tick_counter_state;

public:
    std::uint64_t frequency;

    explicit TickController(long tick_us) : tick_us{tick_us}, tick_start{tick_clock::now()}, tick_counter{0}, tick_counter_state{tick_clock::now()}, frequency{0} {}

    inline void count_tick() {
    }

    inline void generate_tick() {
        count_tick();
    }
};

#endif//UNTITLED_UTILS_TICKCONTROLLER_H_
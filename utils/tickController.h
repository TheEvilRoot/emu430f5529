//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_UTILS_TICKCONTROLLER_H_
#define UNTITLED_UTILS_TICKCONTROLLER_H_

#include <chrono>
#include <thread>

#include <unistd.h>


// #define MEASURE_FREQ
// #define MEASURE_CPU

using namespace std::literals;

class TickController {
private:
    using tick_clock = std::chrono::steady_clock;
    const std::uint64_t expected_frequency;

    tick_clock::time_point tick_start;

    std::uint64_t tick_timer;
    std::uint64_t tick_counter;
    tick_clock::time_point tick_counter_state;

public:
    std::uint64_t frequency;

    explicit TickController(std::uint64_t expected_frequency) : expected_frequency{expected_frequency}, tick_start{tick_clock::now()}, tick_timer{0}, tick_counter{0}, tick_counter_state{tick_clock::now()}, frequency{0} {}

    inline void count_tick() {
        tick_counter++;
        if (tick_counter == 0) {
            tick_counter_state = tick_clock::now();
        }
        if (tick_timer++ == 100) {
            tick_timer = 0;
            const auto dt = std::chrono::duration_cast<std::chrono::seconds>(tick_clock::now() - tick_counter_state).count();
            frequency = tick_counter / (dt);
#ifdef MEASURE_FREQ
          spdlog::warn("freq dt = {}s ticks = {}t", dt, tick_counter);
#endif
        }
    }

    inline void generate_tick() const {
#ifdef MEASURE_CPU
    inline void generate_tick() {
        tick_start = tick_clock::now();
#endif
        if (frequency > expected_frequency) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    }

    inline void finalize_tick() {
#ifdef MEASURE_CPU
        const auto delta = (tick_clock::now() - tick_start).count();
        spdlog::warn("tick took {}ns", delta);
#endif
        count_tick();
    }
};

#endif//UNTITLED_UTILS_TICKCONTROLLER_H_
//
// Created by user on 18.05.22.
//

#ifndef UNTITLED_MEASURE_H
#define UNTITLED_MEASURE_H

#include <chrono>

template<typename Clock = std::chrono::steady_clock>
struct measure {
    using time_point = typename Clock::time_point;

    time_point start;
    measure() : start{Clock::now()} {
    }

    template<typename Dur = std::chrono::nanoseconds>
    auto get_time(bool reset = false) {
        auto end = Clock::now();
        auto delta = std::chrono::duration_cast<Dur>(end - start).count();
        if (reset) start = Clock::now();
        return delta;
    }
};

#endif//UNTITLED_MEASURE_H

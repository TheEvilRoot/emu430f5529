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
  std::chrono::time_point<std::chrono::steady_clock>tick_state;

 public:
  TickController(long tick_us): tick_us{tick_us}, tick_state{std::chrono::steady_clock::now()} { }


  void tick_control_sleep() {
    const auto n = std::chrono::steady_clock::now();
    const auto delta = (n - tick_state) / 1us;
    fprintf(stderr, "%08ld\n", delta);
    if (delta < tick_us)
      usleep(tick_us - delta);
    tick_state = n;
  }
};

#endif //UNTITLED_UTILS_TICKCONTROLLER_H_

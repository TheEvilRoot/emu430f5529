//
// Created by user on 12.05.22.
//

#ifndef UNTITLED_BREAKPOINTCONTROLLER_H
#define UNTITLED_BREAKPOINTCONTROLLER_H

#include <map>

namespace utils {

    struct Breakpoint {
        std::uint16_t addr;
    };

    struct BreakpointController {
        std::map<std::uint16_t, Breakpoint> breakpoints;

        bool has_breakpoint(std::uint16_t addr) {
            return breakpoints.contains(addr);
        }

        bool toggle_breakpoint(std::uint16_t addr) {
            const auto contains = breakpoints.contains(addr);
            if (contains) {
                breakpoints.erase(addr);
            } else {
                breakpoints[addr] = Breakpoint{addr};
            }
            return !contains;
        }
    };
}

#endif//UNTITLED_BREAKPOINTCONTROLLER_H
